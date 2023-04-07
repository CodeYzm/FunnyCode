import tkinter as tk
import time

class Countdown(object):
    def __init__(self, time_s) -> None:
        self.get_start_time(time_s)

    # 根据目标日期获取倒计时起始时间
    def get_start_time(self, time_s):
        self.target_t = time.mktime(time.strptime(time_s, "%Y年%m月%d日 %H:%M:%S"))

    # 获取当前时间，计算时间差
    def get_time_string(self):
        # 计算当前时间与目标时间差
        time_len = self.target_t-time.time()
        # 将整数时间差转化为 时:分:秒
        timestr = time.strftime("%H:%M:%S",time.gmtime(time_len))
        # 重新设置文本
        self.lb.configure(text = timestr)   
        # 每隔1s调用自身获取时间
        self.root.after(1000, self.get_time_string) 

    # 设置窗口
    def set_win(self):
        self.root = tk.Tk()
        self.root.title('倒计时')
        self.lb = tk.Label(self.root, text='',fg='blue',font=("黑体",80))
        self.lb.pack()

    # 运行窗口
    def run(self):
        self.set_win()
        self.get_time_string()
        self.root.mainloop()