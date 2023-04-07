import tkinter as tk
import time
import winsound

#这里设置参数
#开始时间(时，分，秒)
time_start=[0,1,5]
#文字格式（字体，文字大小）
text_font = ( "宋体", 150 )
#文字颜色
text_color = "#0099CC"
#背景颜色
color_bg = "#CCCCCC"
global count_state,win_main

class Countdown(object):
    def __init__(self) -> None:
        pass

    #键盘事件
    def event_key(self,event):
        #print('key: ' + event.keysym+" event:"+str(event) )
        if event.keysym=='Escape':
            win_main.attributes("-fullscreen", False)
        elif event.keysym=='F11':
            win_main.attributes("-fullscreen", True)
        elif event.keysym=='Left':
            if count_state == 2:
                count_state = 3
        elif event.keysym=='Right':
            if count_state != 2:
                count_state = 1
        elif event.keysym=='Up':
            count_state = 0
        win_main.mainloop()

    #更新时间
    def time_update(self):
        global time_start,count_state,count_length,count_gap,count_start,text_time,win_main
        if count_state == 0:
            count_length = int(time_start[0])*3600+int(time_start[1])*60+int(time_start[2])
            count_gap = count_length
        elif count_state == 1:
            count_start = time.time()
            count_state = 2
        elif count_state == 2:
            count_gap = max( 0, count_length-int(time.time()-count_start) )
            if count_gap <= 0:
                winsound.Beep( 2000, 1000 )
                count_state = 4
        elif count_state == 3:
            count_length = count_gap
            count_state = 4
        count_now = str(int(((count_gap)%86400)/3600))+":"
        count_now = count_now + str(int(((count_gap)%3600)/60))+":"
        count_now = count_now + str(int((count_gap)%60))
        text_time["text"] = count_now
        win_main.after(100, time_update)
    
    def set_win(self):
        #时间初始化
        count_state = 0
        #设置窗口
        win_main = tk.Tk()
        win_main.title("倒计时")
        win_main.configure(bg=color_bg)
        win_main.geometry("%dx%d" %(win_main.winfo_screenwidth()/2, win_main.winfo_screenheight()/2))
        win_main.bind('<Key>', self.event_key)
        text_time = tk.Label(win_main, text="TIME SHOW", font=text_font, fg=text_color, bg=color_bg )
        text_time.pack( anchor="center", expand=True )
        win_main.after(100, self.time_update)
        win_main.mainloop()
