import sys
import os
import pandas as pd
import numpy as np
from collections import defaultdict

def getDist(a, b):
    return np.sqrt(np.dot(a-b, a-b))

def printFiles(input_file_name, n, clusters):
    for cluster_id in range(n):
        cluster = []
        if cluster_id < len(clusters):
            cluster = clusters[cluster_id]
        output_file_name = input_file_name.split('.')[0] + '_cluster_' + str(cluster_id)+'.txt'
        np.savetxt(output_file_name, cluster, fmt='%d\n')

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Execute the program with 4 arguments: input file name, n, Eps, MinPts")

    input_file_name = sys.argv[1]
    n = int(sys.argv[2])         # Num of clusters
    eps = float(sys.argv[3])       # Epsilon 
    minPts = int(sys.argv[4])    # Minimum number of points

    # Read Data 
    objects = pd.read_csv(input_file_name, sep='\t', header=None, names=['id', 'x_coordinate', 'y_coordinate'])
    objects = objects.sort_values('x_coordinate').values
    
    # Sort by x coordinate
    points_list = []
    neighbor_list = []
    for object in objects:
        object_id = int(object[0])
        object_coordinate = np.array([object[1], object[2]])
        #print(object_id, object_coordinate)

        for neighbor in reversed(points_list):
            neighbor_id = neighbor[0]
            neighbor_coordinate = neighbor[1]
            if object_coordinate[0] - neighbor_coordinate[0] > eps:
                break
            if getDist(object_coordinate, neighbor_coordinate) <= eps:
                print(object_coordinate, neighbor_coordinate)
                print(object_id)
                if object_id in neighbor_list:
                    neighbor_list[object_id].append(neighbor_id)
                else:
                    neighbor_list[object_id] = neighbor_id

                if neighbor_id in neighbor_list:
                    neighbor_list[neighbor_id].append(neighbor_id)
                else:
                    neighbor_list[neighbor_id] = neighbor_id
                
                # neighbor_list[object_id].append(neighbor_id)
                # neighbor_list[neighbor_id].append(object_id)

        points_list.append((object_id, object_coordinate))

    # Generate clusters
    cluster_id = 0
    cluster_list = []

    visited = []
    q = []
    for id in neighbor_list:
        if id in visited:
            continue
        if len(neighbor_list[id]) < minPts:
            continue
        
        cur_cluster = [id]
        visited.add(id)
        q.append(id)

        while q:
            cur = q.pop(0)
            for i in neighbor_list[cur]:
                if i in visited:
                    continue
                else:
                    visited.add(i)
                    cur_cluster.append(i)
                    if len(neighbor_list[i]) >= minPts:
                        q.append(i)
        cluster_list.append(cur_cluster)
    
    cluster_list.sort(key=lambda x : len(x), reverse=True)
    while len(cluster_list) > n:
        cluster_list.pop(-1)

    # Save file
    printFiles(input_file_name, n, cluster_list)


