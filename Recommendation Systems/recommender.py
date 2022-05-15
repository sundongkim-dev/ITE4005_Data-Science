from unittest import result
import numpy as np
import sys
import pandas as pd

li = []

def get_cosine_similarity(a, i):
    mat_a, mat_b = np.mat(a), np.mat(i)
    denom = np.linalg.norm(mat_a) * np.linalg.norm(mat_b)
    li.append(denom)
    return (0.5 + 0.5 * (float(mat_a * mat_b.T) / denom))

# ./recommender.py u1.base u1.test
if __name__ == '__main__':    
    if len(sys.argv) != 3:
        print("Execute the program with two arguments: train file name, test file name")
        print("Train file name = 'u#.base, test file name = 'u#.test'")
    
    train_file_name = sys.argv[1]
    test_file_name = sys.argv[2]
    output_file_name = sys.argv[1][0:2] + ".base_prediction.txt"

    # Dataframe로 표현
    train_set = pd.read_csv(train_file_name, sep="\t", names=["user_id", "item_id", "rating", "time_stamp"])
    test_set = pd.read_csv(test_file_name, sep="\t", names=["user_id", "item_id", "rating", "time_stamp"])

    # Time stamp는 drop
    train_set = train_set.drop(columns="time_stamp")
    test_set = test_set.drop(columns="time_stamp")

    # rating matrix 만들기
    rating_matrix = train_set.pivot_table(index='user_id', columns='item_id')['rating']

    # rating이 없다면 -1로 초기화
    rating_matrix.fillna(-1, inplace = True) 

    # 연산의 편의를 위해 numpy로 치환
    rating_matrix = rating_matrix.to_numpy()

    num_of_users = rating_matrix.shape[0]
    num_of_items = rating_matrix.shape[1]

    # 각 유저가 모든 item에 부여한 rating의 평균
    avg_rating = np.zeros(num_of_users)
    for user in range(num_of_users):
        rated_item = rating_matrix[user] >= 0        
        avg_rating[user] = rating_matrix[user][rated_item].sum() / len(rated_item[rated_item])

    # 유저 간 cosine similarity로 neighbors 구하기
    user_similarity_matrix = np.ones([num_of_users, num_of_users], dtype=np.float64)
    for i in range(num_of_users):
        for j in range(num_of_users):
            user_similarity_matrix[i][j] = get_cosine_similarity(rating_matrix[i], rating_matrix[j])
    
    print(li)

    print(user_similarity_matrix)

    res = list()
    for user_id, item_id, rating in test_set.values.tolist():
        try:
            itemInfo = rating_matrix[:, item_id-1]
            #print(itemInfo)
            rated = itemInfo >= 0
            rated_user_similarity = user_similarity_matrix[user_id-1, rated]
            # 적절한 Neighbor가 없는 경우 유저가 부여한 rating의 평균값을 취한다.
            if rated_user_similarity.sum() == 0:
                res.append([user_id, item_id, avg_rating[user_id-1]])
            else:
                user_similarity_ratings = rated_user_similarity * (itemInfo[rated] - avg_rating[rated])
                rat = avg_rating[user_id-1] + user_similarity_ratings.sum() / rated_user_similarity.sum()
                #print(rat)
                res.append([user_id, item_id, np.clip(rat, 1, 5)])
        except IndexError as e:
            res.append([user_id, item_id, avg_rating[user_id-1]])

    # save file
    np.savetxt(output_file_name, res, fmt='%d\t%d\t%s')
    