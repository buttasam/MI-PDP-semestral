#include <iostream>
#include <fstream>
#include <vector>
#include <climits>

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

class Game {
    public:
    int size, maxDept, blackCount = 0;
    Move queen;
    char desk[MAX_SIZE][MAX_SIZE];

    // reseni
    int minMoves = INT_MAX;
    vector<Move> minMovesPath;

    bool isQueen(char c) {
        return c == '3';
    }

    bool isBlack(char c) {
        return c == '1';
    }


    bool isWhite(char c) {
        return c == '2';
    }



    void readInfo(ifstream &file) {
        file >> size >> maxDept;
    }


    void readData(ifstream &file) {
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
                    if(isBlack(c)) {
                        blackCount++;
                    }
                    desk[i][j] = c;
                }
            }
        }
    }

    void printData() {
        cout << "Queen [" << queen.x << ", " << queen.y << "]" << endl;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << desk[i][j];
            }
            cout << endl;
        }
    }


    bool isDead(Move& move, vector<Move>& deadBlackList) {
        for (auto &deadFigure : deadBlackList) {
            if (deadFigure.x == move.x && deadFigure.y == move.y) {
                return true;
            }
        }
        return false;
    }

    void addAvailableMovesForDirection(vector<Move>& availableMoves, Move& queen, vector<Move>& deadBlackList, int deltaX, int deltaY) {
        int x = queen.x + deltaX;
        int y = queen.y + deltaY;

        while(x >= 0 && y >= 0 && x < size && y < size) {
            Move move(x,y);
            if(isBlack(desk[x][y])) {
                if(!isDead(move, deadBlackList)) {
                    // pridat na zacatek
                    availableMoves.insert(availableMoves.begin(), move);
                    break;
                }
            }
            if(isWhite(desk[x][y])) {
                break;
            }

            // vloz na konec
            availableMoves.push_back(move);

            x = x + deltaX;
            y = y + deltaY;
        }

    }


    vector<Move> availableMoves(Move& queen, vector<Move> deadBlackList) {
        vector<Move> availableMoves;
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, 0, 1);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, 1, 0);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, 0, -1);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, -1, 0);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, 1, 1);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, -1, -1);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, -1, 1);
        addAvailableMovesForDirection(availableMoves, queen, deadBlackList, 1, -1);

        return availableMoves;
    }


    void findBestSolution() {

        vector<Move> deadBlackList;
        vector<Move> moves;
        findSolution(queen, deadBlackList, moves);

        cout << minMoves << endl;
        for (auto &move : minMovesPath) {
            cout << "(" << move.x << "," << move.y << ")";
        }
        cout << endl;
    }


private:

// rekurzivní funkce se aplikuje na kazdy volny tah
    void findSolution(Move queen, vector<Move> deadBlackList, vector<Move> moves) {

        // vyhod cernou
        if(isBlack(desk[queen.x][queen.y])) {
            deadBlackList.push_back(queen);
        }

        // neexistuje lepsi reseni pak ukoncit
        if(!existsBetterSolution((int) moves.size(), deadBlackList)) {
            return;
        }

        // pridej tah
        moves.push_back(queen);

        // nalezene reseni?
        if(deadBlackList.size() == blackCount) {
            if(moves.size() < minMoves) {
                minMoves = (int) moves.size() - 1;
                minMovesPath = moves;
            }
        } else {
            // nalezneme mozne tahy pro pozici kralovny
            vector<Move> availableMovesList = availableMoves(queen, deadBlackList);

            for (auto &move : availableMovesList) {
                findSolution(move,deadBlackList, moves);
            }
        }

    }

    bool existsBetterSolution(int movesCount, vector<Move>& deadBlackList) {
        if(movesCount > maxDept) {
            return false;
        }

        if(movesCount + (blackCount - deadBlackList.size()) >= minMoves) {
            return false;
        }

        return true;
    }

};

int main(int argc, char *argv[]) {
    ifstream file("/home/samik/CLionProjects/MI-PDP-semestral/data/kralovna01.txt");

    // velikost hraci plochy, maximalni hloubka (omezeni), cernych figurek
    Game game;
    game.readInfo(file);
    game.readData(file);

    // na zacatku je vektor sebranych figur prazdny
    game.printData();
    game.findBestSolution();


    return 0;
}