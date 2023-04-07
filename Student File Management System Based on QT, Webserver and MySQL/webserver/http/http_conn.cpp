#include "http_conn.h"

#include <mysql/mysql.h>
#include <fstream>

//定义http响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";
const char *ok_string = "1";
const char *dup_string = "2";
const char *error_string = "0";
locker m_lock;
map<string, string> users;

void http_conn::initmysql_result(connection_pool *connPool)
{
    //先从连接池中取一个连接
    MYSQL *mysql = NULL;
    connectionRAII mysqlcon(&mysql, connPool);

    //在user表中检索username，passwd数据，浏览器端输入
    if (mysql_query(mysql, "SELECT username,passwd FROM user"))
    {
        LOG_ERROR("SELECT error:%s\n", mysql_error(mysql));
    }

    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(mysql);

    //返回结果集中的列数
    int num_fields = mysql_num_fields(result);

    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        string temp1(row[0]);
        string temp2(row[1]);
        users[temp1] = temp2;
    }
}


char* row_to_string(MYSQL_ROW row, int num_fields) {
    int total_len = 0;
    char *str;

    // 计算总字符串长度
    for (int i = 0; i < num_fields; i++) {
        total_len += (strlen(row[i])+1);
    }

    // 为字符串分配内存空间
    str = (char*) malloc(total_len + 1);

    // 拼接字符串，用&连接
    for (int i = 0; i < num_fields; i++) {
        strcat(str, row[i]);
        strcat(str, "&");
    }
    strcat(str, "\0");
    // printf("row = %s\n", str);
    return str;
}

//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//从内核时间表删除描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

//关闭连接，关闭一个连接，客户总量减一
void http_conn::close_conn(bool real_close)
{
    if (real_close && (m_sockfd != -1))
    {
        printf("close %d\n", m_sockfd);
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
        free(m_content);
    }
}

//初始化连接,外部调用初始化套接字地址
void http_conn::init(int sockfd, const sockaddr_in &addr, char *root, int TRIGMode,
                     int close_log, string user, string passwd, string sqlname)
{
    m_sockfd = sockfd;
    m_address = addr;

    addfd(m_epollfd, sockfd, true, m_TRIGMode);
    m_user_count++;

    //当浏览器出现连接重置时，可能是网站根目录出错或http响应格式出错或者访问的文件中内容完全为空
    doc_root = root;
    m_TRIGMode = TRIGMode;
    m_close_log = close_log;

    strcpy(sql_user, user.c_str());
    strcpy(sql_passwd, passwd.c_str());
    strcpy(sql_name, sqlname.c_str());

    init();
}

//初始化新接受的连接
//check_state默认为分析请求行状态
void http_conn::init()
{
    mysql = NULL;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_linger = false;
    m_method = GET;
    m_url = 0;
    m_response = RES_ERROR;
    m_version = 0;
    m_content = (char *)malloc(sizeof(char) * 200);
    m_content_length = 0;
    m_host = 0;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    cgi = 0;
    m_state = 0;
    timer_flag = 0;
    improv = 0;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

//从状态机，用于分析出一行内容
//返回值为行的读取状态，有LINE_OK,LINE_BAD,LINE_OPEN
http_conn::LINE_STATUS http_conn::parse_line()
{
    char temp;
    for (; m_checked_idx < m_read_idx; ++m_checked_idx)
    {
        temp = m_read_buf[m_checked_idx];
        // 因为HTTP报文使用\r\n作为行与行之间的分隔符，因此这里考虑\r和\n的情况
        if (temp == '\r')
        {
            
            if ((m_checked_idx + 1) == m_read_idx) // 这行还没读完
                return LINE_OPEN;
            else if (m_read_buf[m_checked_idx + 1] == '\n')  // 已经读完一行完整的数据
            {
                m_read_buf[m_checked_idx++] = '\0'; // 将\r\n替换为C字符串的分隔符\0
                m_read_buf[m_checked_idx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if (m_checked_idx > 1 && m_read_buf[m_checked_idx - 1] == '\r') // 已经读完一行完整的数据
            {
                m_read_buf[m_checked_idx - 1] = '\0';
                m_read_buf[m_checked_idx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

//循环读取客户数据，直到无数据可读或对方关闭连接
//非阻塞ET工作模式下，需要一次性将数据读完
bool http_conn::read_once()
{
    if (m_read_idx >= READ_BUFFER_SIZE)
    {
        return false;
    }
    int bytes_read = 0;

    //LT读取数据
    if (0 == m_TRIGMode)
    {
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        m_read_idx += bytes_read;

        if (bytes_read <= 0)
        {
            return false;
        }

        return true;
    }
    //ET读数据（边缘触发，所以要一次性用while循环读完缓冲区的数据）
    else
    {
        while (true)
        {
            bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
            if (bytes_read == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                return false;
            }
            else if (bytes_read == 0)
            {
                return false;
            }
            m_read_idx += bytes_read;
        }
        return true;
    }
}

//解析http请求行，获得请求方法，目标url及http版本号
http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{
    // 到达第一个制表符的位置
    m_url = strpbrk(text, " \t");
    if (!m_url)
    {
        return BAD_REQUEST;
    }
    // 将制表符替换为 \0，使请求方法成为独立的C字符串
    *m_url++ = '\0';
    // 提取请求方法
    char *method = text;
    if (strcasecmp(method, "GET") == 0)
        m_method = GET;
    else if (strcasecmp(method, "POST") == 0)
    {
        m_method = POST;
        cgi = 1;
    }
    else
        return BAD_REQUEST;
    // 跳过制表符
    m_url += strspn(m_url, " \t");
    // 到达下一个制表符的位置
    m_version = strpbrk(m_url, " \t");
    if (!m_version)
        return BAD_REQUEST;
    // 使请求url成为独立的C字符串
    *m_version++ = '\0';
    // 提取HTTP版本号
    m_version += strspn(m_version, " \t");
    if (strcasecmp(m_version, "HTTP/1.1") != 0)
        return BAD_REQUEST;
    if (strncasecmp(m_url, "http://", 7) == 0)
    {
        m_url += 7;
        m_url = strchr(m_url, '/');
    }

    if (strncasecmp(m_url, "https://", 8) == 0)
    {
        m_url += 8;
        m_url = strchr(m_url, '/');
    }

    if (!m_url || m_url[0] != '/')
        return BAD_REQUEST;

    // 请求行状态跳转到请求头部状态
    m_check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

//解析http请求的一个头部信息
http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{
    if (text[0] == '\0')
    {
        if (m_content_length != 0) // 如果Content-length不为0，则说明客户端的请求携带数据，需要进行获取
        {
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive") == 0)
        {
            m_linger = true;
        }
    }
    else if (strncasecmp(text, "Content-length:", 15) == 0)
    {
        text += 15;
        text += strspn(text, " \t");
        m_content_length = atol(text);
    }
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        text += 5;
        text += strspn(text, " \t");
        m_host = text;
    }
    else
    {
        LOG_INFO("oop!unknow header: %s", text);
    }
    return NO_REQUEST;
}

//判断http请求是否被完整读入
http_conn::HTTP_CODE http_conn::parse_content(char *text)
{
    if (m_read_idx >= (m_content_length + m_checked_idx))
    {
        text[m_content_length] = '\0';
        //POST请求中最后为输入的管理员用户名和密码，或者学生档案信息
        m_string = text;
        return GET_REQUEST;
    }
    return NO_REQUEST;
}

// 处理读取的HTTP报文内容
http_conn::HTTP_CODE http_conn::process_read()
{
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char *text = 0;
    // 解析请求报文的载荷 || 解析行
    while ((m_check_state == CHECK_STATE_CONTENT && line_status == LINE_OK) || ((line_status = parse_line()) == LINE_OK))
    {
        text = get_line(); // 获取一行
        m_start_line = m_checked_idx; // 更新下一行的起始位置
        LOG_INFO("%s", text);
        switch (m_check_state)
        {
        case CHECK_STATE_REQUESTLINE: // 解析请求行
        {
            ret = parse_request_line(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            break;
        }
        case CHECK_STATE_HEADER: // 解析请求头
        {
            ret = parse_headers(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            else if (ret == GET_REQUEST)
            {
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT: // 解析请求实体
        {
            ret = parse_content(text);
            if (ret == GET_REQUEST)
                return do_request();
            line_status = LINE_OPEN;
            break;
        }
        default:
            return INTERNAL_ERROR;
        }
    }
    return NO_REQUEST;
}

// 根据读取到的m_url判断服务器需要进行的操作，并且结合请求实体的数据完成操作
http_conn::HTTP_CODE http_conn::do_request()
{
    strcpy(m_real_file, doc_root);
    int len = strlen(doc_root);
    //printf("m_url:%s\n", m_url);
    const char *p = strrchr(m_url, '/'); 
    printf("*p:%s\n", p);
    // POST请求（项目中只用了POST请求跟服务器互动）
    // 0,1,2,3,4,5,6对应了7种操作，跟QT客户端tcpclient.h中定义的HANDLE枚举常量一一对应
    if (cgi == 1)
    {
        if(*(p + 1) == '0' || *(p + 1) == '1') // 注册或者登录
        {
            // 提取客户端发送过来的用户密码user=xxx&password=xxx   
            char name[100], password[100];
            int i;
            for (i = 5; m_string[i] != '&'; ++i)
                name[i - 5] = m_string[i];
            name[i - 5] = '\0';

            int j = 0;
            for (i = i + 10; m_string[i] != '\0'; ++i, ++j)
                password[j] = m_string[i];
            password[j] = '\0';
            if (*(p + 1) == '0')
            {
                //如果是注册，先检测数据库中是否有重名的
                //没有重名的，进行增加数据
                char *sql_query = (char *)malloc(sizeof(char) * 200);
                strcpy(sql_query, "INSERT INTO user(username, passwd) VALUES(");
                strcat(sql_query, "'");
                strcat(sql_query, name);
                strcat(sql_query, "', '");
                strcat(sql_query, password);
                strcat(sql_query, "')");

                if (users.find(name) == users.end())
                {
                    m_lock.lock();
                    int res = mysql_query(mysql, sql_query);
                    users.insert(pair<string, string>(name, password));
                    m_lock.unlock();
                    m_response = RES_OK;
                    printf("%s\n", "register");
                }
                else{
                    m_response = RES_DUPLICATION;
                    printf("%s\n", "register_dup");
                }
                free(sql_query);

            }
            //如果是登录，直接判断
            //若浏览器端输入的用户名和密码在表中可以查找到，返回1，否则返回0
            else
            {
                if (users.find(name) != users.end() && users[name] == password)
                    m_response = RES_OK;
                else
                    m_response = RES_ERROR;
            }
            return FILE_REQUEST;
        }
        else if(*(p + 1) == '2')  // 新建学生档案
        {
            // 提取姓名、学号、出生日期、出生地点
            char name[100], id[100], birthday[100], birthplace[100];
            int i;
            for (i = 0; m_string[i] != '&'; ++i)
                name[i] = m_string[i];
            name[i] = '\0';
            printf("name=%s\n",name);
            int j = 0;
            for (i = i + 1; m_string[i] != '&'; ++i, ++j)
                id[j] = m_string[i];
            id[j] = '\0';
            printf("id=%s\n",id);
            j = 0;
            for (i = i + 1; m_string[i] != '&'; ++i, ++j)
                birthday[j] = m_string[i];
            birthday[j] = '\0';
            printf("birthday=%s\n",birthday);
            j = 0;
            for (i = i + 1; m_string[i] != '\0'; ++i, ++j)
                birthplace[j] = m_string[i];
            birthplace[j] = '\0';
            printf("birthplace=%s\n",birthplace);
            char *sql_query = (char *)malloc(sizeof(char) * 200);
            strcpy(sql_query, "SELECT * FROM studentInfo where id=");
            strcat(sql_query, "'");
            strcat(sql_query, id);
            strcat(sql_query, "'"); // select * from studentInfo where id = 'xxx';
            // printf("sql_query=%s\n", sql_query);
            //在studentInfo表中检索学生id，检测学生档案是否已存在
            m_lock.lock();
            if (mysql_query(mysql, sql_query)) // 如果访问失败返回给客户端错误
            {
                m_response = RES_ERROR;
            }
            else // 访问成功，则看返回的是否是空集
            {
                MYSQL_RES *mysql_result = mysql_store_result(mysql);
                if (mysql_num_rows(mysql_result) == 0) // 获取结果集行数，如果是空集，说明学生档案未建立
                {
                    char *sql_insert = (char *)malloc(sizeof(char) * 200);
                    strcpy(sql_insert, "INSERT INTO studentInfo VALUES(");
                    strcat(sql_insert, "'");
                    strcat(sql_insert, name);
                    strcat(sql_insert, "', '");
                    strcat(sql_insert, id);
                    strcat(sql_insert, "','");
                    strcat(sql_insert, birthday);
                    strcat(sql_insert, "', '");
                    strcat(sql_insert, birthplace);
                    strcat(sql_insert, "')");
                    mysql_query(mysql, "set names 'utf8'"); // 进行插入前一定要设定编码格式，否则数据会乱码
                    if(mysql_query(mysql, sql_insert)) m_response = RES_ERROR;
                    m_response = RES_OK;
                    free(sql_insert);
                } 
                else // 如果不是空集，则返回重复操作
                {
                    m_response = RES_DUPLICATION;
                }
            }
            free(sql_query);
            m_lock.unlock();
            return FILE_REQUEST;
        }
        else if(*(p + 1) == '3') // 查找学生信息
        {
            // 提取学号
            char id[100];
            for(int i = 0; m_string[i] != '\0'; ++i) id[i] = m_string[i];

            // 查询语句
            char *sql_query = (char *)malloc(sizeof(char) * 200);
            strcpy(sql_query, "SELECT * FROM studentInfo where id=");
            strcat(sql_query, "'");
            strcat(sql_query, id);
            strcat(sql_query, "'"); // select * from studentInfo where id = 'xxx';
            // printf("%s\n",sql_query);

            m_lock.lock();
            // 进行查询
            mysql_query(mysql, "set names 'utf8'");
            if(mysql_query(mysql, sql_query)) m_response = RES_ERROR;
            MYSQL_RES *mysql_result = mysql_store_result(mysql); // 存储查询结果
            unsigned int column = mysql_num_fields(mysql_result); // 获取列数
            while (MYSQL_ROW row = mysql_fetch_row(mysql_result)) // 获取查询结果中每一行
            {
                char* p = row_to_string(row, column); // 将每行查询结果都拼接为C字符串
                strcpy(m_content, p);
                free(p);
                printf("m_content=%s\n",m_content);
            }
            free(sql_query);
            m_lock.unlock();
            if(strlen(m_content) == 0)
            {
                m_response = RES_DUPLICATION;
            }
            else
            {
                 m_response = RES_OK;
            }
            return FILE_REQUEST;
        }
        else if(*(p + 1) == '4') // 修改学生信息
        {
            // 提取姓名、学号、出生日期、出生地点
            char name[100], id[100], birthday[100], birthplace[100];
            int i;
            for (i = 0; m_string[i] != '&'; ++i)
                name[i] = m_string[i];
            name[i] = '\0';
            printf("name=%s\n",name);
            int j = 0;
            for (i = i + 1; m_string[i] != '&'; ++i, ++j)
                id[j] = m_string[i];
            id[j] = '\0';
            printf("id=%s\n",id);
            j = 0;
            for (i = i + 1; m_string[i] != '&'; ++i, ++j)
                birthday[j] = m_string[i];
            birthday[j] = '\0';
            printf("birthday=%s\n",birthday);
            j = 0;
            for (i = i + 1; m_string[i] != '\0'; ++i, ++j)
                birthplace[j] = m_string[i];
            birthplace[j] = '\0';
            printf("birthplace=%s\n",birthplace);
            char *sql_query = (char *)malloc(sizeof(char) * 200);
            strcpy(sql_query, "UPDATE studentInfo SET name='");
            strcat(sql_query, name);
            strcat(sql_query, "', birthday='"); // select * from studentInfo where id = 'xxx';
            strcat(sql_query, birthday);
            strcat(sql_query, "', birthplace='");
            strcat(sql_query, birthplace);
            strcat(sql_query, "' where id='");
            strcat(sql_query, id);
            strcat(sql_query, "'");
            // printf("sql_query=%s\n", sql_query);
            //在studentInfo表中检索学生id，检测学生档案是否已存在
            m_lock.lock();
            mysql_query(mysql, "set names 'utf8'");
            if (mysql_query(mysql, sql_query)) // 如果修改失败返回给客户端错误
            {
                m_response = RES_ERROR;
            }
            else // 修改成功，则返回成功
            {
                m_response = RES_OK;
            }
            free(sql_query);
            m_lock.unlock();
            return FILE_REQUEST;
        }
        else if(*(p + 1) == '5') // 删除学生信息
        {
            // 提取学号
            char id[100];
            for(int i = 0; m_string[i] != '\0'; ++i) id[i] = m_string[i];

            // 查询语句
            char *sql_query = (char *)malloc(sizeof(char) * 200);
            strcpy(sql_query, "SELECT * FROM studentInfo where id=");
            strcat(sql_query, "'");
            strcat(sql_query, id);
            strcat(sql_query, "'"); // select * from studentInfo where id = 'xxx';
            // printf("%s\n",sql_query);

            m_lock.lock();
            mysql_query(mysql, "set names 'utf8'");
            // 进行查询
            if(mysql_query(mysql, sql_query))
            {
                m_response = RES_ERROR;
                return FILE_REQUEST; 
            }
            MYSQL_RES *mysql_result = mysql_store_result(mysql);
            if(mysql_num_rows(mysql_result) == 0) // 如果查无此人，返回重复操作
            {
                m_response = RES_DUPLICATION;
            }
            else // 有此人，则删除档案
            {
                char *sql_delete = (char *)malloc(sizeof(char) * 200);
                strcpy(sql_delete, "DELETE FROM studentInfo where id=");
                strcat(sql_delete, "'");
                strcat(sql_delete, id);
                strcat(sql_delete, "'"); // select * from studentInfo where id = 'xxx';
                // 进行删除
                if(mysql_query(mysql, sql_delete)) // 删除失败
                {
                    m_response = RES_ERROR;
                }
                else
                {
                    m_response = RES_OK;
                }
                free(sql_delete);
                
            }
            m_lock.unlock();
            free(sql_query);
            return FILE_REQUEST; 
        }
        else if(*(p + 1) == '6') // 清空学生信息
        {
            m_lock.lock();
            if(mysql_query(mysql, "TRUNCATE TABLE studentInfo")) // 清空数据表
            {
                m_response = RES_ERROR;
            }
            else
            {
                m_response = RES_OK;
            }
            m_lock.unlock();
            return FILE_REQUEST; 
        }
        else // 未知请求
        {
            return BAD_REQUEST;
        } 
        

    }
    else
    {
        return BAD_REQUEST;
    }
    
}

void http_conn::unmap()
{
    if (m_file_address)
    {
        munmap(m_file_address, m_file_stat.st_size);
        m_file_address = 0;
    }
}
bool http_conn::write()
{
    int temp = 0;

    if (bytes_to_send == 0)
    {
        modfd(m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);
        init();
        return true;
    }

    while (1)
    {
        temp = writev(m_sockfd, m_iv, m_iv_count);

        if (temp < 0)
        {
            if (errno == EAGAIN)
            {
                modfd(m_epollfd, m_sockfd, EPOLLOUT, m_TRIGMode);
                return true;
            }
            unmap();
            return false;
        }

        bytes_have_send += temp;
        bytes_to_send -= temp;
        if (bytes_have_send >= m_iv[0].iov_len)
        {
            m_iv[0].iov_len = 0;
            m_iv[1].iov_base = m_file_address + (bytes_have_send - m_write_idx);
            m_iv[1].iov_len = bytes_to_send;
        }
        else
        {
            m_iv[0].iov_base = m_write_buf + bytes_have_send;
            m_iv[0].iov_len = m_iv[0].iov_len - bytes_have_send;
        }

        if (bytes_to_send <= 0)
        {
            unmap();
            modfd(m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);

            if (m_linger)
            {
                init();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

// 向HTTP响应报文中添加内容
bool http_conn::add_response(const char *format, ...)
{
    if (m_write_idx >= WRITE_BUFFER_SIZE)
        return false;
    va_list arg_list;
    va_start(arg_list, format);
    int len = vsnprintf(m_write_buf + m_write_idx, WRITE_BUFFER_SIZE - 1 - m_write_idx, format, arg_list);
    if (len >= (WRITE_BUFFER_SIZE - 1 - m_write_idx))
    {
        va_end(arg_list);
        return false;
    }
    printf("%s\n","add sucess");
    m_write_idx += len;
    va_end(arg_list);

    LOG_INFO("request:%s", m_write_buf);

    return true;
}
bool http_conn::add_status_line(int status, const char *title) // 添加响应状态行
{
    return add_response("%s %d %s\r\n", "HTTP/1.1", status, title);
}
bool http_conn::add_headers(int content_len) // 添加响应头部
{
    return add_content_length(content_len) && add_linger() &&
           add_blank_line();
}
bool http_conn::add_content_length(int content_len)
{
    return add_response("Content-Length:%d\r\n", content_len);
}
bool http_conn::add_content_type()
{
    return add_response("Content-Type:%s\r\n", "text/html");
}
bool http_conn::add_linger()
{
    return add_response("Connection:%s\r\n", (m_linger == true) ? "keep-alive" : "close");
}
bool http_conn::add_blank_line() // 添加空行（HTTP报文中请求实体和响应实体之前均有空行）
{
    return add_response("%s", "\r\n");
}
bool http_conn::add_content(const char *content) // 添加报文载荷（实体）
{
    return add_response("%s", content);
}
// 处理写操作
bool http_conn::process_write(HTTP_CODE ret)
{
    switch (ret)
    {
    case INTERNAL_ERROR:
    {
        add_status_line(500, error_500_title);
        add_headers(strlen(error_500_form));
        if (!add_content(error_500_form))
            return false;
        break;
    }
    case BAD_REQUEST:
    {
        add_status_line(404, error_404_title);
        add_headers(strlen(error_404_form));
        if (!add_content(error_404_form))
            return false;
        break;
    }
    case FORBIDDEN_REQUEST:
    {
        add_status_line(403, error_403_title);
        add_headers(strlen(error_403_form));
        if (!add_content(error_403_form))
            return false;
        break;
    }
    case FILE_REQUEST:
    {
        add_status_line(200, ok_200_title);
        
        if (m_file_stat.st_size != 0) // 如果有文件传输（学生档案管理系统中主要是字符信息的交互，这部分代码保留，可根据需要进行扩展）
        {
            // printf("%s\n","file");
            add_headers(m_file_stat.st_size);
            m_iv[0].iov_base = m_write_buf;
            m_iv[0].iov_len = m_write_idx;
            m_iv[1].iov_base = m_file_address;
            m_iv[1].iov_len = m_file_stat.st_size;
            m_iv_count = 2;
            bytes_to_send = m_write_idx + m_file_stat.st_size;
            return true;
        }
        else // 如果没有文件传输
        {
            // printf("%s\n","unfile");
            if(m_response == RES_OK)
            {
                // printf("%s\n", "ok");
                if(strlen(m_content) == 0) // 不需要返回内容
                {
                    add_headers(strlen(ok_string));
                    if (!add_content(ok_string))
                    {
                        printf("%s\n", "error add content");
                        return false; 
                    }
                }
                else // 返回给客户端内容
                {
                    add_headers(strlen(ok_string) + strlen(m_content) + 1); // + 1 是因为内容与ok_string 中间还有个&
                    if (!add_content(ok_string) || !add_content("&") || !add_content(m_content))
                    {
                        printf("%s\n", "error add content");
                        return false; 
                    }
                }
            }
            else if(m_response == RES_DUPLICATION) 
            {
                // printf("%s\n", "dup");
                add_headers(strlen(dup_string));
                if (!add_content(dup_string)) {
                    printf("%s\n", "error add content");
                    return false; 
                }
            }
            else
            {
                // printf("%s\n", "error");
                add_headers(strlen(error_string));
                if (!add_content(error_string)) {
                    printf("%s\n", "error add content");
                    return false; 
                }  
            }
        }
        break;
    }
    default:
        return false;
    }
    // 如果没有文件传输，就只使用了一块内存
    m_iv[0].iov_base = m_write_buf;
    m_iv[0].iov_len = m_write_idx;
    m_iv_count = 1;
    bytes_to_send = m_write_idx;
    return true;
}

// 报文处理程序，包括了报文读操作和报文写操作，被工作线程调用
void http_conn::process()
{
    HTTP_CODE read_ret = process_read();
    if (read_ret == NO_REQUEST)
    {
        modfd(m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);
        return;
    }
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        close_conn();
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT, m_TRIGMode);
}
