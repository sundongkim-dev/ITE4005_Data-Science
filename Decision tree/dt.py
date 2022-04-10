import sys
import os
import pandas as pd
import numpy as np
import random

def Generate_decision_tree(training_set):    
    # Training data로부터 entropy 계산
    def Get_entropy(training_tuples):
        attribute, counts = np.unique(training_tuples, return_counts=True)
        print(attribute)
        entropy = float(0)
        for i in range(len(attribute)):
            p_i = counts[i]/np.sum(counts)
            entropy -= p_i * np.log2(counts[i]/np.sum(counts))
        return entropy
    
    # Information gain = entropy(parent) - [weighted average]entropy(children)
    def Get_information_gain(training_tuples, attribute_name, target_attribute):
        parent_entropy = Get_entropy(training_tuples[target_attribute])
        attributes, counts = np.unique(training_tuples[attribute_name], return_counts=True)
        children_entropy = float(0)
        for attribute, count in zip(attributes, counts):
            children_entropy += np.sum([count * (Get_entropy(training_tuples[training_tuples[attribute_name] == attribute][target_attribute]))]) / np.sum(counts)
        return parent_entropy - children_entropy

    def Get_best_attribute(training_tuples, target_attribute):
        attribute_list = list(training_tuples.columns[:-1])
        best_information_gain = float(0)
        best_attribute = attribute_list[0]

        for attribute in attribute_list:
            cur_information_gain = Get_information_gain(training_tuples, attribute, target_attribute)
            if(best_information_gain < cur_information_gain):
                best_attribute = attribute
                best_information_gain = cur_information_gain

        return best_attribute

    # create root node for the tree
    decision_tree = {}
    class_label = training_set.columns[-1] # Class:buys_computer
    
    # if all members of training set are in the same class C
    if len(np.unique(training_set[class_label])) <= 1:
        return np.unique(training_set[class_label])[0]
    
    # if Attributes are empty then single-node tree with label
    # most common value of Target_attribute in training_set
    elif len(training_set.columns) <= 1: 
        key, counts = np.unique(training_set[class_label], return_counts=True)
        return key[counts.argmax()]
    
    else:
        # Get Best attribute(decision attribute)
        best_attribute = Get_best_attribute(training_set, class_label)
        decision_tree[best_attribute] = {}

        # Get best attribute's values
        best_attribute_values = np.unique(training_set[best_attribute])

        # For each possible value of best attribute's values
        for attribute_value in best_attribute_values:
            splitting_subset = training_set[training_set[best_attribute] == attribute_value].drop([best_attribute], axis=1)
            sub_tree = Generate_decision_tree(splitting_subset)
            decision_tree[best_attribute][attribute_value] = sub_tree

        return decision_tree

def Classifier(decision_tree, test_set, class_label):
    predicted_value = []
    for test_data in range(len(test_set.index)):
        sub_tree = decision_tree
        # class_label(leaf node) 만날 때까지 탐색
        while type(sub_tree) == type(dict()):
            # 첫 번째 분류 지점
            attribute = list(sub_tree.keys())[0]
            
            # 분류 지점의 값들
            attribute_value = test_set.loc[test_data, attribute]

            # 탐색해야 하는 Attribute value가 분류 지점의 값들 중에 있을 경우, 1 depth만큼 전진해서 탐색
            if attribute_value in sub_tree[attribute]:
                sub_tree = sub_tree[attribute][attribute_value]
            
            # decision_tree에 탐색해야 하는 attribute value가 없는 경우
            else:
                nodes = list(sub_tree[attribute].values())

                candidates = []
                nextNode_candidates = []

                # leaf node가 있을 경우
                for value in nodes:
                    if type(value) == type(str()):
                        candidates.append(value)
                
                # leaf node가 없을 경우 대비
                for node in nodes:
                    if type(node) == type(dict()):
                        nextNode_candidates.append(node)
                
                # leaf node하나라도 있으면 
                if len(candidates) > 0:
                    value, counts = np.unique(candidates, return_counts=True)
                    sub_tree = value[counts.argmax()]

                # leaf node가 없을 경우 그냥 다른 노드로
                else:
                    sub_tree = random.choice(nextNode_candidates)

                pass
            pass
        predicted_value.append(sub_tree)
    test_set[class_label] = predicted_value
    return test_set

# ./dt.py dt_train.txt dt_test.txt dt_result.txt
# Attribute_selection_method: information gain
if __name__ == '__main__':    
    if len(sys.argv) != 4:
        print("Execute the program with tree arguments: training file name, test file name, output file name")
        print("Training file name = 'dt_train.txt', test file name = 'dt_test.txt', output file name = 'dt_result.txt'")
    
    training_file_name = sys.argv[1]
    test_file_name = sys.argv[2]
    output_file_name = sys.argv[3]

    training_set = pd.read_csv(training_file_name, sep="\t")
    test_set = pd.read_csv(test_file_name, sep="\t")
    class_label = training_set.columns[-1] # Class:buys_computer
    
    decision_tree = Generate_decision_tree(training_set)
    Classifier(decision_tree, test_set, class_label).to_csv(output_file_name, sep='\t', index=False)

    print(decision_tree)

'''
Algorithm: Generate_decision_tree
Generate a decision tree from the training tuples of data partition, D.

Input:
  1. Data partition, D, which is a set of training tuples and their associated class labels.
  2. attribute_list, the set of candidate attributes;
  3. Attribute_selection_method, a procedure to determine the splitting criterion that "best" partitions the data tuples into individual classes.
  This criterion consists of a splitting_attribute and, possibly, either a split-point or splitting subset.

Output: A decision tree
Method:
  create a node N;
  if tuples in D are all of the same class, C, then
      return N as a leaf node labeled with the class C;
  if attribute_list is empty then
      return N as a leaf node labeled with the majority class in D; // Majority voting
  apply Attribute_selection_method(D, attribute_list) to find the "best" splitting_criterion;
  label node N with splitting_criterion;
  if splitting_attribute is discrete-valued and
      multiway splits allowed then // not restricted to binary trees
      attribute_list := attribute_list-splitting_attribute // remove splitting_attribute
  for each outcome j of splitting_criterion
  // partition the tuples and grow subtrees for each partition
      let Dj be the set of data tuples in D satisfying outcome j; // a partition
      if Dj is empty then
          attach a leaf labeled with the majority class in D to node N;
      else
          attach the node returned by Generate_decision_tree(Dj, attribute_list) to node N;
  endfor
  return N;
'''