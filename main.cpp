#include <iostream>
#include <fstream>

using namespace std;

const int MAX_SIZE = 20;

struct figurine {
    int x, y;
};

void readInfo(ifstream &file, int &size, int &maxDept) {
    file >> size >> maxDept;
}


bool isQueen(char c) {
    return c == '3';
}

bool isBlack(char c) {
    return c == '1';
}


bool isWhite(char c) {
    return c == '2';
}


void printData(figurine &queen, char desk[][MAX_SIZE], int size) {
    cout << "Queen [" << queen.x << ", " << queen.y << "]" << endl;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << desk[i][j];
        }
        cout << endl;
    }
}


void readData(ifstream &file, figurine &queen, char desk[][MAX_SIZE]) {
    int i = 0;
    for (string line; getline(file, line);) {
        int j = 0;
        for (char &c : line) {
            if (isQueen(c)) {
                queen.x = i;
                queen.y = j;
                desk[i][j] = '0';
            } else {
                desk[i][j] = c;
            }
            j++;
        }
        i++;
    }
}

int main(int argc, char *argv[]) {
    ifstream file("/home/samik/CLionProjects/MI-PDP-semestral/data/kralovna01.txt");

    // velikost hraci plochy, maximalni hloubka (omezeni), pocet bilych a cernych figurek
    int size, maxDept, blackCount, whiteCount;
    figurine queen{};
    char desk[MAX_SIZE][MAX_SIZE];

    readInfo(file, size, maxDept);
    readData(file, queen, desk);

    printData(queen, desk, size);

    return 0;
}