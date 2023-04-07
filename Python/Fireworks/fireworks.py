import math, random,time
import threading

class firework(object):
    def __init__(self,color,speed,width,height):
        #uid=uuid.uuid1()
        self.radius=random.randint(2,4)  #粒子半径为2~4像素
        self.color=color   #粒子颜色
        self.speed=speed  #speed是1.5-3.5秒
        self.status=0   #在烟花未爆炸的情况下，status=0；爆炸后，status>=1；当status>100时，烟花的生命期终止
        self.nParticle=random.randint(20,30)  #粒子数量
        self.center=[random.randint(0,width-1),random.randint(0,height-1)]   #烟花随机中心坐标
        self.oneParticle=[]    #原始粒子坐标（100%状态时）
        self.rotTheta=random.uniform(0,2*math.pi)  #椭圆平面旋转角
 
        #椭圆参数方程：x=a*cos(theta),y=b*sin(theta)
        #ellipsePara=[a,b]
 
        self.ellipsePara=[random.randint(30,40),random.randint(20,30)]   
        theta=2*math.pi/self.nParticle
        for i in range(self.nParticle):
            t=random.uniform(-1.0/16,1.0/16)  #产生一个 [-1/16,1/16) 的随机数
            x,y=self.ellipsePara[0]*math.cos(theta*i+t), self.ellipsePara[1]*math.sin(theta*i+t)    #椭圆参数方程
            xx,yy=x*math.cos(self.rotTheta)-y*math.sin(self.rotTheta),  y*math.cos(self.rotTheta)+x*math.sin(self.rotTheta)     #平面旋转方程
            self.oneParticle.append([xx,yy])
        
        self.curParticle = self.oneParticle[0:]     #当前粒子坐标
        self.thread = threading.Thread(target=self.extend)   #建立线程对象
        
 
    def extend(self):         #粒子群状态变化函数线程
        for i in range(100):
            self.status+=1    #更新状态标识
            self.curParticle=[[one[0]*self.status/100, one[1]*self.status/100] for one in self.oneParticle]   #更新粒子群坐标
            time.sleep(self.speed/50)
    
    def explode(self):
        self.thread.setDaemon(True)    #把现程设为守护线程
        self.thread.start()          #启动线程
            
 
    def __repr__(self):
        return ('color:{color}\n'  
                'speed:{speed}\n'
                'number of particle: {np}\n'
                'center:[{cx} , {cy}]\n'
                'ellipse:a={ea} , b={eb}\n'
                'particle:\n{p}\n'
                ).format(color=self.color,speed=self.speed,np=self.nParticle,cx=self.center[0],cy=self.center[1],p=str(self.oneParticle),ea=self.ellipsePara[0],eb=self.ellipsePara[1])