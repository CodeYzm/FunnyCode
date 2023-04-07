#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
    python_v: python3.9.1
    author: 梦想是优秀社畜 from CSDN
    date: 2023/03/07
'''

from BasicAlgorithm import BA
import numpy as np

if __name__ == "__main__":
    algorithmCreator = BA()
    # 初始化空地图
    map = np.zeros((15,15)).astype(int)
    # 起点、终点
    start = (12,2)
    end = (1,11)
    # path, explored = algorithmCreator.bfs(start,end,map)
    # path, explored = algorithmCreator.maze(start,end,map)
    # path, explored = algorithmCreator.dijkstra(start,end,map)
    # path, explored = algorithmCreator.greedy_best_first_searsh(start,end,map)
    # 路径搜索
    path, explored = algorithmCreator.astar(start,end,map)
    print(path)
        

    


