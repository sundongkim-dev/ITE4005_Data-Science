import sys
import pandas as pd
import numpy as np

def getDist(a, b):
    return np.sqrt(np.dot(a-b, a-b))

def printFiles(input_file_name, n, clusters):
    for cluster_id in range(n):
        cluster = []
        if cluster_id < len(clusters):
            cluster = clusters[cluster_id]
        cluster.sort()
        output_file_name = input_file_name.split('.')[0] + '_cluster_' + str(cluster_id)+'.txt'
        np.savetxt(output_file_name, cluster, fmt='%d')

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
    coordinates_list = []
    neighbor_list = {}
    for object in objects:
        object_id = int(object[0])
        object_coordinate = np.array([object[1], object[2]])
        neighbor_list[object_id] = []
        
        # 가까운 점 먼저 이웃 리스트에 추가
        for neighbor in reversed(coordinates_list):
            neighbor_id = neighbor[0]
            neighbor_coordinate = neighbor[1]
            # x 좌표의 차이가 eps 보다 크다면 무조건 eps를 초과하기 때문에 검사할 필요 없음
            if object_coordinate[0] - neighbor_coordinate[0] > eps:
                break
            # x 좌표의 차이가 eps보다 같거나 작다면 eps 보다 작거나 같을 수 있기 때문에 검사할 필요가 있음
            if getDist(object_coordinate, neighbor_coordinate) <= eps:                
                # eps보다 작거나 같다면 그 두 점의 이웃 리스트에 각각 추가
                neighbor_list[object_id].append(neighbor_id)
                neighbor_list[neighbor_id].append(object_id)
        # 전체 좌표 집합에 추가하기
        coordinates_list.append((object_id, object_coordinate))

    # Generate clusters
    cluster_id = 0
    cluster_list = []
    visited = []
    q = []

    for id in neighbor_list:
        # 이미 방문했으면 continue
        if id in visited:
            continue
        # (현재 점 + 이웃의 수)가 minPts보다 작다면 continue
        if len(neighbor_list[id]) + 1 < minPts:
            continue
        
        visited.append(id)
        cur_cluster = [id]
        
        q.append(id)
        while q:
            cur_id = q.pop(0)
            # 해당 오브젝트의 이웃들 탐색
            for sub_id in neighbor_list[cur_id]:
                if sub_id in visited:
                    continue
                else:
                    visited.append(sub_id)      # 방문 mask
                    cur_cluster.append(sub_id)  # 현재 cluster에 추가
                    # minPts 이상이라면 해당 오브젝트의 이웃들도 조사
                    if len(neighbor_list[sub_id]) + 1 >= minPts:    
                        q.append(sub_id)
        # 탐색할 수 있는 모든 오브젝트 탐색했다면 cluster_list에 추가
        cluster_list.append(cur_cluster)
    
    cluster_list.sort(key=lambda x : len(x))

    # If your algorithm finds m clusters for an input data and m is greater than n (n = the
    # number of clusters given), you can remove (m-n) clusters based on the number of objects
    # within each cluster. In order to remove (m-n) clusters, for example, you can select (m-n)
    # clusters with the small sizes in ascending order
    # You can remove outlier. In other words, you don't need to include outlier in a specific cluster
    while len(cluster_list) > n:
        cluster_list.pop(0)
    while len(cluster_list) < n:
        cluster_list.insert(0, [])
    
    # Save file
    printFiles(input_file_name, n, cluster_list)


