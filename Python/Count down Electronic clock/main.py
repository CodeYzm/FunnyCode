#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
    python_v: python3.9.1
    author: 梦想是优秀社畜 from CSDN
    date: 2023/1/30
'''

from clock import Countdown

if __name__ == "__main__":
    s1 = "2023年01月31日 14:00:00"  # 自定义时间格式，如有调整，Countdown类中的get_start_time()函数也要调整
    cd = Countdown(s1)
    cd.run()
