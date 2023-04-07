#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
    python_v: python3.9.1
    author: 梦想是优秀社畜 from CSDN
    date: 2023/03/19
'''
from selenium import webdriver
import time

if __name__ == "__main__":
    options = webdriver.ChromeOptions() # chrome浏览器配置
    options.add_argument('--incognito') # 设置启动参数：无痕模式
    # 启动参数概览
    # options.add_argument('--headless')                     # 开启无界面模式
    # options.add_argument("--disable-gpu")                  # 禁用gpu
    # options.add_argument('--user-agent=Mozilla/5.0 HAHA')  # 配置对象添加替换User-Agent的命令
    # options.add_argument('--window-size=1366,768')         # 设置浏览器分辨率（窗口大小）
    # options.add_argument('--start-maximized')              # 最大化运行（全屏窗口）,不设置，取元素会报错
    # options.add_argument('--disable-infobars')             # 禁用浏览器正在被自动化程序控制的提示
    # options.add_argument('--incognito')                    # 隐身模式（无痕模式）
    # options.add_argument('--disable-javascript')           # 禁用javascript
    # options.add_argument(f"--proxy-server=http://115.239.102.149:4214")  # 使用代理
    # options.add_argument('blink-settings=imagesEnabled=false')  # 不加载图片, 提升速度
    driver = webdriver.Chrome(chrome_options=options, executable_path='./chromedriver') # 加载chrome浏览器
    driver.get("这里替换成投票链接！！！") # 模拟上网
    driver.find_element('class name', 'vote').click() # 通过html解析找到投票按键对应的分类，模拟单击按键
    driver.quit() # 退出浏览器

