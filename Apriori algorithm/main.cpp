#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip> 

using namespace std;

// ========================
// 종료 코드
// -1: 비정상적인 인자 개수
// ========================

typedef map<set<int>, int> map_itemset_sup;
typedef pair<set<int>, int> itemset_sup;
typedef set<set<int>> candidates;
vector< set<int> > DB;
map_itemset_sup all_frequent_set;

struct information
{
    map_itemset_sup all_L;
    map_itemset_sup::iterator it;
    set<int> right_powerset;
    set<int> result;
};

// ==========================================
// ***************Initial Scan***************
// DB를 스캔하며 frequent한 1-itemset 얻는다.
// ==========================================
vector<map_itemset_sup> initialScan(double min_support)
{
    vector<map_itemset_sup> L(2);
    for (auto& itemset : DB)
    {
        for (auto& item : itemset)
        {
            set<int> frequent_1_itemset;    // 1개 아이템을 지닌 아이템 셋 저장을 위한 set
            int item_id = item;
            int sup = 0;
            frequent_1_itemset.insert(item_id);

            if (L[1].find(frequent_1_itemset) != L[1].end())
                continue;

            for (auto& is : DB)             // DB scan하면서 sup 저장
                if (is.find(item) != is.end())
                    sup++;

            double cur_sup = (double)sup / DB.size();
            if (cur_sup >= min_support)
                L[1].insert(itemset_sup(frequent_1_itemset, sup));
        }
    }
    return L;
}

// ==========================================================
// ********생성한 candidate가 적합한 지 확인하는 함수********
// 생성한 candidate가 frequent하려면
// 그의 부분 집합은 무조건 frequent하므로, L에 존재해야 한다.
// ==========================================================
bool chkItemset(map_itemset_sup Lk, set<int> candidate)
{
    for (auto item : candidate)
    {
        set<int> chk_itemset(candidate);
        chk_itemset.erase(item);

        if (Lk.find(chk_itemset) == Lk.end())
            return false;
    }
    return true;
}

// ===========================================
// ********Generate Candidates from Lk********
// Frequent한 길이 k의 itemset으로 부터
// k+1 길이의 Candidates 생성
// ===========================================
candidates genCandidates(map_itemset_sup Lk)
{
    candidates Ck;
    for (auto i : Lk)
    {
        for (auto j : Lk)
        {
            if (i == j)
                continue;
            set<int> candidate(i.first);
            candidate.insert(j.first.begin(), j.first.end());

            long long k = i.first.size();
            if (candidate.size() == k + 1)
            {
                if (chkItemset(Lk, candidate))
                    Ck.insert(candidate);
            }
        }
    }
    return Ck;
}

// ===========================================
// *********Subset인 지 확인하는 함수*********
// Frequent한 길이 k의 itemset으로 부터
// k+1 길이의 Candidates 생성
// ===========================================
bool isSubset(set<int> parent, set<int> pattern)
{
    // pattern과 parent 일치하면 is subset
    if (parent == pattern)     
        return true;
    // pattern이 parent보다 길면 is not subset
    else if (parent.size() < pattern.size()) 
        return false;
    // pattern 안의 item이 parent에 없다면 is not subset
    else                       
    {
        for (auto item : pattern)
            if (parent.find(item) == parent.end())
                return false;
        return true;
    }
}

// ============================================
// *************멱집합 구하는 함수*************
// Association Rule을 얻으려면 어떤 집합의
// 자기 자신과 공집합을 제외한 집합을 얻어야 함.
// 우선 멱집합을 구하고 후처리 한다.
// ============================================
candidates getPowerset(const set<int>& Lk)
{
    candidates powerset;

    for (auto& item : Lk)
    {
        // 새로 만든 부분 집합과 새로운 원소 사용해서 부분집합 생성
        for (auto& subset : powerset) 
        {
            set<int> tmp(subset);
            tmp.insert(item);
            powerset.insert(tmp);
        }       
        // 1개짜리 원소도 부분집합에 추가
        powerset.insert({ item });
    }
    return powerset;
}

// ============================================
// ********Association Rule 구하는 함수********
// Output에 출력해줄 각 Association Rule과
// support, confidence 값을 구한다.
// ============================================
vector<information> find_Association_Rules(map_itemset_sup& all_L)
{
    vector<information> res;
    for (auto it = all_L.begin(); it != all_L.end(); it++)
    {
        set<int> Lk = it->first;
        int sup = it->second;

        if (Lk.size() < 2) // 1개 짜리 원소는 패스
            continue;

        set<set<int>> powersets = getPowerset(Lk);
        powersets.erase(Lk); // 자기 자신은 삭제

        for (auto& right_powerset : powersets)
        {
            set<int> left_powerset(Lk);
            set<int> result;
            set_difference(left_powerset.begin(), left_powerset.end(), right_powerset.begin(), right_powerset.end(), inserter(result, result.end()));
          
            res.push_back({ all_L, it, right_powerset, result });
        }
    }
    return res;
}

// ===========================================
// ****Output.txt에 결과물 출력해주는 함수****
// find_Association_Rules에서 얻은 정보로
// 출력 파일에 저장해준다.
// ===========================================
void write_Output(vector<information>& v, ofstream& writeFile, string outputFileName)
{
    for (auto& idx : v)
    {
        string item_set, associative_item_set;
        item_set = associative_item_set = "{";
        for (auto& item : idx.right_powerset)
            item_set += (to_string(item) + ",");
        item_set.pop_back();

        for (auto& item : idx.result)
            associative_item_set += (to_string(item) + ",");
        associative_item_set.pop_back();

        double sup = (double)idx.it->second / DB.size() * 100;
        //double confidence = (double)idx.it->second / idx.all_L[idx.right_powerset] * 100;
        double confidence = (double)idx.it->second / all_frequent_set[idx.right_powerset] * 100;

        writeFile << item_set << "}\t" << associative_item_set << "}\t" << sup << '\t' << confidence << '\n';
    }
}

// =======================================================
// ****주어진 입력의 조건들을 터미널에 출력해주는 함수****
// 입력 값인 minimum support 값과 입-출력 파일의 이름 출력
// =======================================================
void printArgv(char** argv)
{
    cout << "Minimum support value: " << stod(argv[1]) / 100 << "\n\n";
    cout << argv[2] << "의 정보를 바탕으로 " << argv[3] << "에 결과를 저장하겠습니다." << "\n";
}

int main(int argc, char** argv)
{
    // ==========================================================================================
    // 실행은 3 개의 arguments가 주어진다. 3 개가 아니면 에러 문구 출력하고 종료한다.
    // 3 Arguments = {Minimum support, Input file name, Output file name}
    // Ex) Minimum support = 5%, Input file name = 'input.txt', Output file name = 'output.txt'
    // ==========================================================================================
    if (argc != 4)
    {
        cout << "다음 3 개의 arguments를 순서대로 입력해주세요. Minimum support, Input file name, Output file name" << "\n";
        return -1;
    }
    ifstream readFile;
    ofstream writeFile;
    double minimumSupport = stod(argv[1]) / 100; // 백분율 기준으로 입력하기 때문에 100으로 나눠줘야 함
    string inputFileName = argv[2];
    string outputFileName = argv[3];
    
    printArgv(argv);
    
    // ==========================================
    // input.txt 열어서 item_id의 set을 얻는다.
    // 얻은 정보를 DB에 저장
    // ==========================================
    readFile.open(inputFileName);
    if (readFile.is_open())
    {
        while (!readFile.eof())
        {
            string line;
            getline(readFile, line); // 각 Xact마다 중복된 item은 input으로 주어지지 않는다.
            if (line == "") break;   // 마지막이 개행으로 끝나기 때문에 input이 공백이면 break.
            
            int idx = 0;
            set<int> Xact;           // 중복된 item 있을수도 있으니 set으로 input 받는다. (오름차순)
            for (int pos = 0; pos < line.length(); pos++)
            {
                if (line[pos] == '\t')
                {
                    Xact.insert(stoi(line.substr(idx, pos - idx + 1)));
                    idx = pos + 1;
                }
            }
            Xact.insert(stoi(line.substr(idx)));
            DB.push_back(Xact);      // input.txt안의 정보들 중 '\t'없애서 item_id만을 담는 DB
        }
        readFile.close();
    }

    // =================================================================
    // L: k-length의 Frequent itemset과 그에 따른 sup을 저장하는 vector
    // 최초에 DB를 스캔하며 frequent한 1-itemset 얻어서 L에 저장.
    // =================================================================
    vector<map_itemset_sup> L = initialScan(minimumSupport);

    // ==========================================
    // Frequent한 길이 k의 itemset으로 부터
    // 길이 k+1의 candidate itemset 생성한다.
    // 더이상 candidate 생성되지 않으면 종료
    // ==========================================
    long long length = 1;
    for (int k = 2; !L[length++].empty(); k++)
    {
        candidates Ck = genCandidates(L[k - 1]);
        map_itemset_sup tmp;
        for (auto& itemset : DB)
        {
            for (auto& candidate : Ck)
            {
                if (isSubset(itemset, candidate))
                {
                    if (tmp.find(candidate) == tmp.end())
                        tmp.insert({ candidate,1 });
                    else
                        tmp[candidate]++;
                }
            }
        }
        // ==============================================
        // DB를 순회하면서 생성한 앞서 candidate들 테스트
        // Minimum support 충족하지 못하면 drop하고
        // 충족한다면 L[k]에 추가한다.
        // ==============================================
        map_itemset_sup::iterator it = tmp.begin();
        while (it != tmp.end())
        {
            double cur_sup = (double)it->second / DB.size();
            if (cur_sup < minimumSupport)
                tmp.erase(it++->first);
            else
                it++;
        }
        L.push_back(tmp);
        if (L[k].empty())
        {
            length--;
            break;
        }
    }

    // ==============================================
    // 모든 frequent itemset을 map에 저장
    // ==============================================
    for (auto& itemsets : L)
        for (auto& itemset : itemsets)
                all_frequent_set.insert(itemset);

    writeFile.open(outputFileName);
    writeFile << fixed << setprecision(2);

    for (auto& itemsets : L)
    {
        map_itemset_sup all_L;
        for (auto& itemset : itemsets)
            all_L.insert(itemset);

        // ==============================================
        // Association Rules 생성
        // ==============================================
        vector<information> outputVector = find_Association_Rules(all_L);

        // ==============================================
        // 얻은 정보를 바탕으로 support와 confidence 계산
        // 출력 파일에 저장하기
        // ==============================================
        write_Output(outputVector, writeFile, outputFileName);
    }

    writeFile.close();

    cout << "\n저장을 끝마쳤습니다.\n";

    return 0;
}

// =================================================================================
// *********************************************************************************
// ******************                                             ******************
// ******************      The Apriori Algorithm Pseudo-Code      ******************
// ******************                                             ******************
// *********************************************************************************
// =================================================================================
// Initially, scan DB once to get frequent 1-itemset
// Generate candidate itemsets of length(k + 1) from frequent itemsets of length k
// Test the candidates against DB
// Terminate when no frequent or candidate set can be generated
// 
// ********************************** Pseudo-code **********************************
//
// Ck: Candidate itemset of size k
// Lk: frequent itemset of size k
// L1 = {frequent items}
// for(k=1; Lk != empty; k++) do begin
//      Ck+1 = genCandidates(Lk);
//      for each Xact t in DB do
//          for each candidate c in Ck do
//              if c is contained in t then
//                  count++;
//          end
//      end
//      Lk+1 = Candidates in Ck+1 with min_support
// end
// return UkLk;
// =================================================================================