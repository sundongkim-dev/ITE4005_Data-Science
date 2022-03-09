#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

// ========================
// 종료 코드
// -1: 비정상적인 인자 개수
// ========================

typedef map<set<int>, int> map_itemset_sup;
typedef pair<set<int>, int> itemset_sup;
typedef set<set<int>> candidates;
vector< set<int> > DB;

void print123()
{
    cout << "버그다!!!" << "\n";
}

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
            set<int> frequent_1_itemset;  // 1개 아이템을 지닌 아이템 셋 저장을 위한 set
            int item_id = item;
            int sup = 0;
            frequent_1_itemset.insert(item_id);

            if (L[1].find(frequent_1_itemset) != L[1].end())
                continue;

            for (auto& is : DB) // DB scan하면서 sup 저장
            {
                if (is.find(item) != is.end())
                    sup++;
            }

            double cur_sup = (double)sup / DB.size();
            if (cur_sup >= min_support)
                L[1].insert(itemset_sup(frequent_1_itemset, sup));
        }
    }
    return L;
}

// ===========================================
// ********Generate Candidates from Lk********
// Frequent한 길이 k의 itemset으로 부터
// k+1 길이의 Candidates 생성
// ===========================================
set<set<int>> genCandidates(map_itemset_sup Lk)
{
    set<set<int>> candidate;
    for (auto x : Lk)
    {
        for (auto y : x.first)
            cout << y << " ";
    }
    return candidate;
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
    double minimumSupport = stod(argv[1]) / 100; // 백분율 기준으로 입력하기 때문에 100으로 나눠줘야 함
    string inputFileName = argv[2];
    string outputFileName = argv[3];
    cout << minimumSupport << " " << inputFileName << " " << outputFileName << endl;

    ifstream readFile;
    ofstream writeFile;
    
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
    
    /*for (auto x : L)
    {
        for (auto y : x)
        {
            for (auto z : y.first)
            {
                cout << z << " ";
            }
            cout << "\n";
        }
    }*/
    candidates C = genCandidates(L[1]);

    // ==========================================
    // Frequent한 길이 k의 itemset으로 부터
    // 길이 k+1의 candidate itemset 생성
    // ==========================================
    /*for (int k = 2; !L.empty(); k++)
    {
        set<set<int>> C = genCandidates(L[k - 1]);
    }*/
    
    

    // ==========================================
    // DB를 순회하면서 생성한 candidate들 테스트
    // 맞으면 저장하고 틀리면 drop 하며
    // 더이상 candidate 생성되지 않으면 종료
    // ==========================================
    return 0;
}

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