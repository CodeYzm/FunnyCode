import time
s1 = "2023年01月31日 00:00:00"  # 自定义格式
target_t = int(time.mktime(time.strptime(s1, "%Y年%m月%d日 %H:%M:%S")))
hours = (target_t - time.time()) // 3600
minutes = (target_t - time.time()) % 3600 // 60
seconds = (target_t - time.time()) % 3600 % 60 
s = time.strftime("%H:%M:%S",time.gmtime(target_t-time.time()))
print(s)