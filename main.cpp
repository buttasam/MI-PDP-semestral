#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int MAX_SIZE = 20;

class Move {
public:
    Move() {}
    Move(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int x, y;
};

bool isQueen(char c) {
    return c == '3';
}

bool isBlack(char c) {
    return c == '1';
}


bool isWhite(char c) {
    return c == '2';
}

void readInfo(ifstream &file, int &size, int &maxDept) {
    file >> size >> maxDept;
}

void printData(Move &queen, char desk[][MAX_SIZE], int size) {
    cout << "Queen [" << queen.x << ", " << queen.y << "]" << endl;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << desk[i][j];
        }
        cout << endl;
    }
}


void readData(ifstream &file, Move &queen, char desk[][MAX_SIZE], int& size) {
    string line;
    getline(file, line); // nacteni prazdne radky

    for (int i = 0; i < size; i++) {
        getline(file, line);

        for (int j = 0; j < size; j++) {
            char c = line.at(j);
            if (isQueen(c)) {
                queen.x = i;
                queen.y = j;
                desk[i][j] = '0';
            } else {
                desk[i][j] = c;
            }
        }
    }
}

void availableMovesForDirection(Move& queen, char desk[][MAX_SIZE], int size, vector<Move>& deadBlackList, int deltaX, int deltaY) {
    int x = queen.x + deltaX;
    int y = queen.y + deltaY;

    while(x >= 0 && y >= 0 && x < size && y < size) {
        cout << x << "," << y << endl;

        x = x + deltaX;
        y = y + deltaY;

    }
    cout << "--------------------" << endl;

}

void availableMoves(Move& queen, char desk[][MAX_SIZE], int size, vector<Move> deadBlackList) {
    availableMovesForDirection(queen, desk, size, deadBlackList, 0, 1);
    availableMovesForDirection(queen, desk, size, deadBlackList, 1, 0);
    availableMovesForDirection(queen, desk, size, deadBlackList, 0, -1);
    availableMovesForDirection(queen, desk, size, deadBlackList, -1, 0);
    availableMovesForDirection(queen, desk, size, deadBlackList, 1, 1);
    availableMovesForDirection(queen, desk, size, deadBlackList, -1, -1);
    availableMovesForDirection(queen, desk, size, deadBlackList, -1, 1);
    availableMovesForDirection(queen, desk, size, deadBlackList, 1, -1);
}

int main(int argc, char *argv[]) {
    ifstream file("/home/samik/CLionProjects/MI-PDP-semestral/data/kralovna01.txt");

    // velikost hraci plochy, maximalni hloubka (omezeni), pocet bilych a cernych figurek
    int size, maxDept, blackCount, whiteCount;
    Move queen;
    char desk[MAX_SIZE][MAX_SIZE];

    readInfo(file, size, maxDept);
    readData(file, queen, desk, size);

    vector<Move> deadBlackList;
    availableMoves(queen, desk, size, deadBlackList);


    return 0;
}