import numpy as np
import sys
import pandas as pd

def get_cosine_similarity(a, i):
    same_item_idx = (a>= 0) * (i >= 0)
    if len(same_item_idx[same_item_idx]) == 0:
        return 0
    vec1 = a[same_item_idx]
    vec2 = i[same_item_idx]
    return vec1.dot(vec2) / (sum(np.sqrt(vec1 ** 2)) + sum(np.sqrt(vec2 ** 2)))
  
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

    # 유저와 아이템의 수 파악
    num_of_users = max(train_set.iloc[:, 0])
    num_of_items = max(train_set.iloc[:, 1])

    # rating matrix 만들기 - Df사용하면 없는 정보 누락할 수 있음
    rating_matrix = np.full((num_of_users, num_of_items), 0)

    for user_id, item_id, rating in train_set.values.tolist():
        rating_matrix[user_id-1][item_id-1] = rating

    # 각 유저가 모든 item에 부여한 rating의 평균
    avg_rating = np.zeros(num_of_users)
    for user in range(num_of_users):
        rated_item = rating_matrix[user] >= 0        
        avg_rating[user] = rating_matrix[user][rated_item].sum() / len(rated_item[rated_item])
    print("DONE")
    # 유저 간 cosine similarity로 neighbors 구하기
    user_similarity_matrix = np.ones([num_of_users, num_of_users])
    for i in range(num_of_users):
        for j in range(num_of_users):
            user_similarity_matrix[i][j] = get_cosine_similarity(rating_matrix[i], rating_matrix[j])
    print("DONE")
    # u#.base_prediction.txt에 넘겨줄 정보 만들기
    res = list()
    for user_id, item_id, rating in test_set.values.tolist():
        try:
            itemInfo = rating_matrix[:, item_id-1]
            rated = itemInfo >= 0
            rated_user_similarity = user_similarity_matrix[user_id-1, rated]
            # 적절한 Neighbor가 없는 경우 유저가 부여한 rating의 평균값을 취한다.
            if rated_user_similarity.sum() == 0:
                res.append([user_id, item_id, avg_rating[user_id-1]])
            else:
                user_similarity_ratings = rated_user_similarity * (itemInfo[rated] - avg_rating[rated])
                rat = avg_rating[user_id-1] + user_similarity_ratings.sum() / rated_user_similarity.sum()
                res.append([user_id, item_id, round(np.clip(rat, 1, 5))])
        except IndexError as e:
            res.append([user_id, item_id, avg_rating[user_id-1]])
    print("DONE")
    # save file
    np.savetxt(output_file_name, res, fmt='%d\t%d\t%s')
    