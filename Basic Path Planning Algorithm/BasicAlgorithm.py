from queue import PriorityQueue, Queue, LifoQueue
import numpy as np

moves = [[0,1],[1,0],[-1,0],[0,-1]]

class BA:
    def __init__(self) -> None:
        pass

    def isInside(self,x, y, shp)->bool:
        return x >= 0 and x < shp[0] and y >= 0 and y < shp[1]
            
    def bfs(self, start, end, map):
        xsize = map.shape[0]
        ysize = map.shape[1]
        # 开启列表，用于记录已搜索过的节点
        openList = np.zeros((xsize, ysize)).astype(int)
        # 初始化路径回溯矩阵，记录每个栅格的父节点
        fatherx = np.zeros((xsize, ysize)).astype(int)
        fathery = np.zeros((xsize, ysize)).astype(int)
        explored = []
        path = []
        # 初始化队列
        q = Queue()
        q.put(start)
        while not q.empty():
            point = q.get()
            xcur = point[0]
            ycur = point[1]
            openList[xcur][ycur] = 1
            if xcur == end[0] and ycur == end[1]:
                break
            for move in moves:
                xnxt = xcur + move[0]
                ynxt = ycur + move[1]
                # 检查是否在地图内、是否不是障碍栅格、是否未搜索过
                if self.isInside(xnxt, ynxt, map.shape) and map[xnxt][ynxt] == 0 and openList[xnxt][ynxt] == 0:
                    fatherx[xnxt][ynxt] = xcur
                    fathery[xnxt][ynxt] = ycur
                    q.put((xnxt,ynxt))
                    explored.append((xnxt,ynxt))
        
        xcur = end[0] 
        ycur = end[1]
        # 回溯得到路径
        while xcur != start[0] or ycur != start[1]:
            path.append((xcur,ycur))
            xpre = fatherx[xcur,ycur]
            ypre = fathery[xcur,ycur]
            xcur = xpre
            ycur = ypre
        path.append((xcur,ycur))
        path.reverse()
        return path, explored
    
    def dijkstra(self, start, end, map):
        xsize = map.shape[0]
        ysize = map.shape[1]
        fatherx = np.zeros((xsize, ysize)).astype(int)
        fathery = np.zeros((xsize, ysize)).astype(int)
        # 将栅格代价都初始化为无穷
        D = np.ones((xsize, ysize)) * float("inf")
        D[start[0]][start[1]] = 0
        explored = []
        path = []
        # 优先队列，方便取到代价最低的节点
        q = PriorityQueue()
        q.put((0,start))
        while not q.empty():
            p = q.get()
            point = p[1]
            dis = p[0]
            xcur = point[0]
            ycur = point[1]
            if dis > D[xcur][ycur]:
                continue
            if xcur == end[0] and ycur == end[1]:
                break
            for move in moves:
                xnxt = xcur + move[0]
                ynxt = ycur + move[1]
                disnxt = dis + 1
                # 检查是否在地图内、是否不是障碍栅格、是否未搜索过以及代价较低
                if self.isInside(xnxt, ynxt, map.shape) and map[xnxt][ynxt] == 0 and D[xnxt][ynxt] > disnxt:
                    explored.append((xnxt,ynxt))
                    fatherx[xnxt][ynxt] = xcur
                    fathery[xnxt][ynxt] = ycur
                    D[xnxt][ynxt] = disnxt
                    q.put((disnxt,(xnxt,ynxt)))
        xcur = end[0] 
        ycur = end[1]
        # 回溯得到路径
        while xcur != start[0] or ycur != start[1]:
            path.append((xcur,ycur))
            xpre = fatherx[xcur,ycur]
            ypre = fathery[xcur,ycur]
            xcur = xpre
            ycur = ypre
        path.append((xcur,ycur))
        path.reverse()
        return path, explored    

    def greedy_best_first_searsh(self, startPoint, endPoint, map):
        # 获取地图大小
        x_size = map.shape[0]
        y_size = map.shape[1]

        # 初始化路径记录矩阵，每个记录坐标(x,y,z)的父亲坐标值
        came_fromx = np.zeros((x_size, y_size)).astype(int)
        came_fromy = np.zeros((x_size, y_size)).astype(int)

        # 获取起点终点xyz
        x0 = startPoint[0]
        y0 = startPoint[1]
        xe = endPoint[0]
        ye = endPoint[1]

        # 起点的父亲是它自己
        came_fromx[x0, y0] = x0
        came_fromy[x0, y0] = y0

        # 起点的G = 0, f0 = 0 + H
        f0 = abs(xe - x0) + abs(ye - y0)

        # 将起点的损失以及坐标点加入优先队列
        q = PriorityQueue()
        q.put((f0,(x0,y0)))

        # 初始化开列表和关列表，初值为0表示还没被加入
        openList = np.zeros((x_size, y_size)).astype(int)
        closeList = np.zeros((x_size, y_size)).astype(int)
        explored = []
        # 初始化记录每个栅格的路径损失G和总损失F值矩阵
        F = float("inf") * np.ones((x_size, y_size))
        F[x0, y0] = f0

        # 终点还没被加入到开表并且开表中还有元素
        while not q.empty() :
            p = q.get()
            xcur = p[1][0]
            ycur = p[1][1]
            # 优先队列采用了延迟删除，出现以下情况说明对应栅格的损失已经被更新过，直接进入下一轮循环
            if F[xcur, ycur] < p[0]:
                continue
            if xcur == xe and ycur == ye:
                break
            # 从开启列表删除，并移入删除列表
            openList[xcur,ycur] = 0
            closeList[xcur,ycur] = 1
            
            # 检查附近所有可用栅格
            for move in moves:
                xnxt = xcur + move[0]
                ynxt = ycur + move[1]
                # 判断是否越界、障碍物、已经遍历的栅格
                if self.isInside(xnxt,ynxt, map.shape) and map[xnxt,ynxt] == 0 and closeList[xnxt,ynxt] == 0:
                    hnxt = abs(xnxt - xe) + abs(ynxt - ye) 
                    fnxt = hnxt
                    # 如果当前栅格还未在开列表内
                    if openList[xnxt,ynxt] == 0:     
                        F[xnxt, ynxt] = fnxt
                        came_fromx[xnxt, ynxt] = xcur
                        came_fromy[xnxt, ynxt] = ycur
                        explored.append((xnxt,ynxt))
                        q.put((fnxt,(xnxt,ynxt)))
                        openList[xnxt,ynxt] = 1
                    # 如果当前栅格已经在开列表内
                    else:
                        if F[xnxt, ynxt] > fnxt:
                            F[xnxt, ynxt] = fnxt
                            came_fromx[xnxt, ynxt] = xcur
                            came_fromy[xnxt, ynxt] = ycur
                            # 这里继续加入队列，对于原来的损失值不进行更新，后面会进行延迟删除
                            q.put((fnxt,(xnxt,ynxt)))


        # 利用路径存储矩阵从终点回溯到起点  
        path = []
        path.append((xe, ye))
        xcur = xe
        ycur = ye

        while xcur != x0 or ycur != y0:
            xnxt = came_fromx[xcur, ycur]
            ynxt = came_fromy[xcur, ycur]
            path.append((xnxt,ynxt))
            xcur = xnxt
            ycur = ynxt


        path.reverse()

        return path, explored

    def astar(self, startPoint, endPoint, map):
        # 获取地图大小
        x_size = map.shape[0]
        y_size = map.shape[1]

        # 初始化路径记录矩阵，每个记录坐标(x,y,z)的父亲坐标值
        came_fromx = np.zeros((x_size, y_size)).astype(int)
        came_fromy = np.zeros((x_size, y_size)).astype(int)

        # 获取起点终点xyz
        x0 = startPoint[0]
        y0 = startPoint[1]
        xe = endPoint[0]
        ye = endPoint[1]

        # 起点的父亲是它自己
        came_fromx[x0, y0] = x0
        came_fromy[x0, y0] = y0

        # 起点的G = 0, f0 = 0 + H
        f0 = abs(xe - x0) + abs(ye - y0)

        # 将起点的损失以及坐标点加入优先队列
        q = PriorityQueue()
        q.put((f0,(x0,y0)))

        # 初始化开列表和关列表，初值为0表示还没被加入
        openList = np.zeros((x_size, y_size)).astype(int)
        closeList = np.zeros((x_size, y_size)).astype(int)
        explored = []
        # 初始化记录每个栅格的路径损失G和总损失F值矩阵
        G = float("inf") * np.ones((x_size, y_size))
        F = float("inf") * np.ones((x_size, y_size))
        F[x0, y0] = f0
        G[x0, y0] = 0

        # 终点还没被加入到开表并且开表中还有元素
        while not q.empty() :
            p = q.get()
            xcur = p[1][0]
            ycur = p[1][1]
            # 优先队列采用了延迟删除，出现以下情况说明对应栅格的损失已经被更新过，直接进入下一轮循环
            if F[xcur, ycur] < p[0]:
                continue
            if xcur == xe and ycur == ye:
                # print("终点",xcur,ycur,zcur)
                # print("滴滴",came_fromx[xcur, ycur, zcur],came_fromy[xcur, ycur, zcur],came_fromz[xcur, ycur, zcur])
                break
            # 从开启列表删除，并移入删除列表
            openList[xcur,ycur] = 0
            closeList[xcur,ycur] = 1
            
            # 检查附近所有可用栅格
            for move in moves:
                xnxt = xcur + move[0]
                ynxt = ycur + move[1]
                # 判断是否越界、障碍物、已经遍历的栅格
                if self.isInside(xnxt,ynxt, map.shape) and map[xnxt,ynxt] == 0 and closeList[xnxt,ynxt] == 0:
                    # 计算F = kg * G + kh * H
                    gnxt = G[xcur, ycur] + 1
                    hnxt = (abs(xnxt - xe) + abs(ynxt - ye) )
                    fnxt = gnxt + hnxt
                    # 如果当前栅格还未在开列表内
                    if openList[xnxt,ynxt] == 0:     
                        G[xnxt, ynxt] = gnxt
                        F[xnxt, ynxt] = fnxt

                        came_fromx[xnxt, ynxt] = xcur
                        came_fromy[xnxt, ynxt] = ycur
                        explored.append((xnxt,ynxt))
                        q.put((fnxt,(xnxt,ynxt)))
                        openList[xnxt,ynxt] = 1
                    # 如果当前栅格已经在开列表内
                    else:
                        if G[xnxt, ynxt] > gnxt:
                            G[xnxt, ynxt] = gnxt
                            F[xnxt, ynxt] = fnxt
                            came_fromx[xnxt, ynxt] = xcur
                            came_fromy[xnxt, ynxt] = ycur
                            # 这里继续加入队列，对于原来的损失值不进行更新，后面会进行延迟删除
                            q.put((fnxt,(xnxt,ynxt)))


        # 利用路径存储矩阵从终点回溯到起点  
        path = []
        path.append((xe, ye))
        xcur = xe
        ycur = ye

        while xcur != x0 or ycur != y0:
            xnxt = came_fromx[xcur, ycur]
            ynxt = came_fromy[xcur, ycur]
            # print(xcur, ycur, zcur)
            #print(xnxt,ynxt,znxt)
            path.append((xnxt,ynxt))
            xcur = xnxt
            ycur = ynxt


        path.reverse()

        return path, explored


    def maze(self, startPoint, endPoint, map):
        x_size = map.shape[0]
        y_size = map.shape[1]
        # 用于记录已搜索的节点
        visited = np.zeros((x_size, y_size))
        # 栈
        stk = LifoQueue()
        path = []
        explored = []
        if self.dfs(startPoint[0], startPoint[1], endPoint[0], endPoint[1], stk, visited, map):
            # 依次出栈得到路径
            while not stk.empty():
                path.append(stk.get())
            path.reverse()
            for i in range(x_size):
                for j in range(y_size):
                    if visited[i][j] == 1:
                        explored.append((i,j))
        
        return path, explored

    
    def dfs(self, x, y, endx, endy, stk, visited, map)->bool:
        stk.put((x,y))
        visited[x][y] = 1
        if x == endx and y == endy:
            return True
        order = [2,1,0,3]
        for i in order:
            move = moves[i]
            newx = x + move[0]
            newy = y + move[1]
            # 如果未搜索过且节点合法，则继续进行深度优先搜索
            if self.isInside(newx, newy, map.shape) and map[newx][newy] == 0 and visited[newx][newy] == 0:
                if self.dfs(newx, newy, endx, endy, stk, visited, map):
                    return True
        stk.get()
        return False
    

