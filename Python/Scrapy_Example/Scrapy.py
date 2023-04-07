import requests
from fake_useragent import UserAgent
import json
from jsonpath import jsonpath


class Scrapy_fun(object):
    def __init__(self) -> None:
        super().__init__()
        # UA伪装: 将当前请求载体的身份标识伪装成某款浏览器
        # UA: User-Agent, 请求载体的身份标识
        self.headers = {
            'User-Agent':UserAgent().chrome
        }

    # 模拟网页进行搜狗搜索
    def SogouSearch_scrapy(self)->None:
        url = 'https://www.sogou.com/web'
        # 输入搜索词
        query = input('enter a word:')
        # get请求参数部分
        param = {
            'query':query
        }
        # 对指定url发起get请求
        response = requests.get(url=url, params=param, headers=self.headers)
        # 获取html数据
        page_text = response.text
        # 存储html数据
        with open("sogou_" + query + '.html', 'w', encoding='utf-8') as fp:
            fp.write(page_text)

    # 模拟百度翻译进行单词搜索
    def BaiDuTranslator_scrapy(self)->None:
        # post请求网址
        url = 'https://fanyi.baidu.com/sug'
        # 输入一个单词
        word = input('enter a word: ')
        # post请求数据部分
        data = {
            'kw':word
        }
        # 对指定url发起post请求
        response = requests.post(url=url, data = data, headers=self.headers)
        # 获取json对象，必须确认服务器返回数据(content-type)是json类型才能使用
        dic_obj = response.json() 
        # print(dic_obj)
        # 持久化存储成json文件
        fp = open(word + '.json','w',encoding='utf-8') # 文件描述符
        json.dump(dic_obj,fp =fp, ensure_ascii=False ) # 中文不能使用ASCII码格式
    
    # 获取豆瓣电影排行榜喜剧类页面的电影信息
    def DouBanMovie_scrapy(self)->None:
        # get请求网址
        url = 'https://movie.douban.com/j/chart/top_list'
        parm = {
            'type': '24',
            'interval_id': '100:90',
            'action': '',
            'start': '0',  # 从第一个开始
            'limit': '20' # 获取20个
        }
        # 对指定url发起post请求
        response = requests.get(url=url, params=parm, headers=self.headers)
        # 获取json对象，必须确认服务器返回数据(content-type)是json类型才能使用
        dic_obj = response.json() 
        # print(dic_obj)
        # 持久化存储成json文件
        fp = open('douban.json','w',encoding='utf-8') # 文件描述符
        json.dump(dic_obj,fp =fp, ensure_ascii=False ) # 中文不能使用ASCII码格式

    # 获取肯德基官网的地区餐厅信息
    def KFC_scrapy(self)->None:
        # get请求网址
        url = 'http://www.kfc.com.cn/kfccda/ashx/GetStoreList.ashx'
        # 输入一个地区
        area = input('输入一个地区: ')
        parm = {
            'op': 'keyword',
        }
        data = {
            'cname': '',
            'pid': '',
            'keyword': area,
            'pageIndex': '1', # 页码
            'pageSize': '10'  # 单页显示项数
        }
        # 对指定url发起post请求
        response = requests.post(url=url, params=parm,data=data, headers=self.headers)
        # 获取json对象，必须确认服务器返回数据(content-type)是json类型才能使用
        text = response.text
        # 持久化存储成json文件
        fp = open('kfc.json','w',encoding='utf-8') # 文件描述符
        json.dump(text,fp =fp, ensure_ascii=False ) # 中文不能使用ASCII码格式

    # 获取Bilibili某up主所有视频的评论
    def Bilibili_scrapy(self)->None:
        headers = {
            'User-Agent':UserAgent().chrome,
            'authority': 'api.bilibili.com',
        }
        # get请求网址
        url = 'https://api.bilibili.com/x/space/wbi/arc/search'
        mid = input('输入up主id: ')
        parm = {
            'mid': mid, # up主id
            'ps': '30', # 视频页面视频数 page size
            'tid': '0',
            'pn': '1', # up主视频页面页码 page number
        }

        # 对指定url发起post请求
        response = requests.get(url=url, params=parm, headers=headers)
        # 获取json对象，必须确认服务器返回数据(content-type)是json类型才能使用
        text = response.json()
        aids = jsonpath(text, "$..aid") # 获取该页面中所有视频的aid


        all_comment = []
        # get请求网址
        url = 'https://api.bilibili.com/x/v2/reply/main'

        for i in range(0,10): # 就爬十个视频
        # for aid in aids:
            parm = {
                'jsonp': 'jsonp',
                'next': '1', # 评论页码
                'type': '1',
                'oid': aids[i], # up视频页面的aid就是视频的oid
                'mode': '3',
                'plat': '1',
            }
            # 对指定url发起post请求
            response = requests.get(url=url, params=parm, headers=headers)
            # 获取json对象
            text = response.json()
            comment = jsonpath(text, "$.data.replies[*].content.message") # 解析，获取data节点下面，replies节点中所有对象的content节点的message属性值
            all_comment.append(comment)
            print(comment)
    