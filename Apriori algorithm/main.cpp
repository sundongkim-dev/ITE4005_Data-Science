#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

// ========================
// 종료 코드
// -1: 비정상적인 인자 개수
// ========================

vector< set<int> > DB;

void print123()
{
    cout << "버그다!!!" << "\n";
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
    // ==========================================
    // 얻은 정보를 바탕으로 candidates 생성한다.
    // ==========================================
    for (auto x : DB)
    {
        for (auto s : x)
            cout << s << " ";
        cout << "\n";
    }


    return 0;
}