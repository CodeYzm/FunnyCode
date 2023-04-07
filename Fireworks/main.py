#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
    python_v: python3.9.1
    author: 梦想是优秀社畜 from CSDN
    date: 2023/1/30
'''

import fireworks
import random
import tkinter as tk
import re
 
Fireworks=[]
maxFireworks=8
height,width=600,600
 

 
def colorChange(fire):
    rgb=re.findall(r'(.{2})',fire.color[1:])
    cs=fire.status
    
    f=lambda x,c: hex(int(int(x,16)*(100-c)/30))[2:]    #当粒子寿命到70%时，颜色开始线性衰减
    if cs>70:
        ccr,ccg,ccb=f(rgb[0],cs),f(rgb[1],cs),f(rgb[2],cs)
    else:
        ccr,ccg,ccb=rgb[0],rgb[1],rgb[2]
        
    return '#{0:0>2}{1:0>2}{2:0>2}'.format(ccr,ccg,ccb)
 
 
 
def appendFirework(n=1):   #递归生成烟花对象
    if n>maxFireworks or len(Fireworks)>maxFireworks:
        pass
    elif n==1:
        cl='#{0:0>6}'.format(hex(int(random.randint(0,16777215)))[2:])   # 产生一个0~16777215（0xFFFFFF）的随机数，作为随机颜色
        a=fireworks.firework(cl,random.uniform(1.5,3.5),width,height)
        Fireworks.append( {'particle':a,'points':[]} )   #建立粒子显示列表，‘particle’为一个烟花对象，‘points’为每一个粒子显示时的对象变量集
        a.explode()
    else:
        appendFirework()
        appendFirework(n-1)
 
 
def show(c):
    for p in Fireworks:                #每次刷新显示，先把已有的所以粒子全部删除
        for pp in p['points']:
            c.delete(pp)
    
    for p in Fireworks:                #根据每个烟花对象，计算其中每个粒子的显示对象
        oneP=p['particle']
        if oneP.status==100:        #状态标识为100，说明烟花寿命结束
            Fireworks.remove(p)     #移出当前烟花
            appendFirework()           #新增一个烟花
            continue
        else:
            li=[[int(cp[0]*2)+oneP.center[0],int(cp[1]*2)+oneP.center[1]] for cp in oneP.curParticle]       #把中心为原点的椭圆平移到随机圆心坐标上
            color=colorChange(oneP)   #根据烟花当前状态计算当前颜色
            for pp in li:
                p['points'].append(c.create_oval(pp[0]-oneP.radius,  pp[1]-oneP.radius,  pp[0]+oneP.radius,  pp[1]+oneP.radius,  fill=color))  #绘制烟花每个粒子
 
    root.after(50, show,c)  #回调，每50ms刷新一次
 
if __name__=='__main__':
    appendFirework(maxFireworks)
    
    root = tk.Tk()
    cv = tk.Canvas(root, height=height, width=width)
    cv.create_rectangle(0, 0, width, height, fill="black")
 
    cv.pack()
 
    root.after(50, show,cv)
    root.mainloop()