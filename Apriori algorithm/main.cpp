#include <iostream>

using namespace std;

// ========================
// 종료 코드
// -1: 비정상적인 인자 개수
// ========================

int main(int argc, char** argv)
{
    // ==========================================================================================
    // 실행은 3 개의 arguments가 주어진다. 3개보다 많으면 에러 문구 출력하고 종료한다.
    // Minimum support, Input file name, Output file name
    // ex) Minimum support = 5%, Input file name = 'input.txt', Output file name = 'output.txt'
    // ==========================================================================================
    if(argc != 4)
    {
        cout << "다음 3 개의 arguments를 순서대로 입력해주세요. Minimum support, Input file name, Output file name" << "\n";
        return -1;
    }
    double minimumSupport = stod(argv[1]);
    string inputFileName = argv[2];
    string outputFileName = argv[3];

    return 0;
}