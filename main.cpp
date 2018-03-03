#include <iostream>

using namespace std;

const int MAX_SIZE = 20;

struct figurine {
    int x, y;
};

void readInfo(int &size, int &maxDept) {
    cin >> size >> maxDept;
}


bool isQueen(char c) {
    return c == '3';
}


void printData(figurine &queen, char desk[][MAX_SIZE], int size) {
    cout << "Queen [" << queen.x << ", " << queen.y << "]" << endl;

    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            cout << desk[i][j];
        }
        cout << endl;
    }
}


void readData(figurine &queen, char desk[][MAX_SIZE]) {

    int i = 1;
    for (string line; getline(cin, line);) {
        int j = 1;
        for (char &c : line) {
            if (isQueen(c)) {
                queen.x = i;
                queen.y = j;
            } else {
                desk[i][j] = c;
            }
            j++;
        }
        i++;
    }
}

int main(int argc, char *argv[]) {

    // promenne
    int size, maxDept; // velikost hraci plochy a maximalni hloubka (omezeni)
    int blackCount, whiteCount;
    readInfo(size, maxDept);

    figurine queen;
    char desk[MAX_SIZE][MAX_SIZE];
    readData(queen, desk);

    printData(queen, desk, size);

    return 0;
}