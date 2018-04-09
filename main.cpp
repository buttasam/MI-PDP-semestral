#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <queue>

using namespace std;

const int MAX_SIZE = 20;

class Move {
public:
    Move() {
    }

    Move(int x, int y) {
        this->x = x;
        this->y = y;
        printStar = false;
    }

    int x, y;
    bool printStar;
};

class Solution {
public:
    Solution() {
    }

    Move queenPosition;
    vector<Move> deadBlackList;
    vector<Move> moves;
};


class Game {
public:
    int size, maxDept, blackCount = 0;
    Move queen;
    char desk[MAX_SIZE][MAX_SIZE];

// reseni
    int minMoves;
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
        minMoves = maxDept;
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
                    if (isBlack(c)) {
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


    bool isDead(Move &move, vector<Move> &deadBlackList) {
        for (auto &deadFigure : deadBlackList) {
            if (deadFigure.x == move.x && deadFigure.y == move.y) return true;
        }
        return false;
    }

    void addAvailableMovesForDirection(vector<Move> &blackMoves, vector<Move> &otherMoves, Move &queen,
                                       vector<Move> &deadBlackList, int deltaX, int deltaY) {
        int x = queen.x + deltaX;
        int y = queen.y + deltaY;

        while (x >= 0 && y >= 0 && x < size && y < size) {
            if (isWhite(desk[x][y])) {
                return;
            }

            Move move(x, y);
            if (isBlack(desk[x][y])) {
                if (!isDead(move, deadBlackList)) {
                    // pridat na zacatek
                    blackMoves.push_back(move);
                    return;
                }
            } else {
                otherMoves.push_back(move);
            }

            x = x + deltaX;
            y = y + deltaY;
        }
    }


    void availableMoves(vector<Move> &availableMoves, Move &queen, vector<Move> &deadBlackList) {
        vector<Move> otherMoves;
        int blackCounter = 0;
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, 1, 1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, -1, -1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, -1, 1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, 1, -1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, 0, 1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, 1, 0);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, 0, -1);
        addAvailableMovesForDirection(availableMoves, otherMoves, queen, deadBlackList, -1, 0);

        // reverse(otherMoves.begin(), otherMoves.end());
        availableMoves.insert(availableMoves.end(), otherMoves.begin(), otherMoves.end());
    }

    void printMoves(vector<Move> &moves, vector<Move> &deadBlackList) {
        for (auto &move : moves) {
            cout << "(" << move.x << "," << move.y << ")";
        }
        cout << deadBlackList.size() << endl;
    }


    void findBestSolutionTaskParallel() {
        auto start = chrono::system_clock::now();

#pragma omp parallel
        {
#pragma omp single
            {
                vector<Move> deadBlackList;
                vector<Move> moves;
                findSolutionTaskParallel(queen, deadBlackList, moves);
            }
        }

        auto end = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << elapsed / 1000.0 << " seconds" << endl;

        cout << minMoves << endl;
        for (auto &move : minMovesPath) {
            cout << "(" << move.x << "," << move.y << ")";
            if (move.printStar) cout << "*";
        }
        cout << endl;
    }

    void findBestSolutionSeq() {
        auto start = chrono::system_clock::now();

        Solution initSolution;
        initSolution.queenPosition = queen;

        findSolutionSeq(initSolution);

        auto end = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << elapsed / 1000.0 << " seconds" << endl;

        cout << minMoves << endl;
        for (auto &move : minMovesPath) {
            cout << "(" << move.x << "," << move.y << ")";
            if (move.printStar) cout << "*";
        }
        cout << endl;
    }

    void findBestSolutionDataParallel() {
        auto start = chrono::system_clock::now();

        // pocatecni reseni
        Solution initSolution;
        initSolution.queenPosition = queen;

        // fronta reseni
        deque<Solution> queueSolutions;
        queueSolutions.push_back(initSolution);

        // pokud je velikost queue > n
        while(queueSolutions.size() < 50) {
            findSolutionBFS(queueSolutions);
        }


        for(int i = 0; i < queueSolutions.size(); i++) {
            findSolutionSeq(queueSolutions.at(i));
        }

        auto end = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << elapsed / 1000.0 << " seconds" << endl;

        cout << minMoves << endl;
        for (auto &move : minMovesPath) {
            cout << "(" << move.x << "," << move.y << ")";
            if (move.printStar) cout << "*";
        }
        cout << endl;
    }

    void reset() {
        minMoves = maxDept;
        minMovesPath.clear();
    }

private:

    void findSolutionBFS(deque<Solution>& queueSolutions) {
        // ziskej element z fronty
        Solution lastSolution = queueSolutions.front();
        // odeber element z fronty
        queueSolutions.pop_front();

        // nalezeno optimalni reseni
        if (minMoves == blackCount) return;

        // prekroceni hloubky
        if (lastSolution.moves.size() > maxDept) return;

        // uz neni mozne nalezt lepsi tah
        if (lastSolution.moves.size() + (blackCount - lastSolution.deadBlackList.size()) > minMoves) return;

        // vyhod cernou
        if (isBlack(desk[lastSolution.queenPosition.x][lastSolution.queenPosition.y]) &&
            !isDead(lastSolution.queenPosition, lastSolution.deadBlackList)) {
            lastSolution.queenPosition.printStar = true;
            lastSolution.deadBlackList.push_back(lastSolution.queenPosition);
        }

        // pridej tah
        lastSolution.moves.push_back(lastSolution.queenPosition);

        // printMoves(moves, deadBlackList);

        // nalezene reseni? a muze byt lepsi?
        if ((lastSolution.deadBlackList.size() == blackCount) && ((lastSolution.moves.size() - 1 < minMoves))) {
            minMoves = (int) lastSolution.moves.size() - 1;
            minMovesPath = lastSolution.moves;
            return;
        }

        // najdi vsechny mozne tahy
        vector<Move> availableMovesList;
        availableMoves(availableMovesList, lastSolution.queenPosition, lastSolution.deadBlackList);

        // aplikuj rekurzi na vsechny mozne tahy
        for (auto move : availableMovesList) {
            Solution foundSolution;
            foundSolution.queenPosition = move;
            foundSolution.moves = lastSolution.moves;
            foundSolution.deadBlackList = lastSolution.deadBlackList;

            queueSolutions.push_back(foundSolution);
        }
    }

    // rekurzivni funkce
    void findSolutionTaskParallel(Move &queen, vector<Move> deadBlackList, vector<Move> moves) {
        // nalezeno optimalni reseni
        if (minMoves == blackCount) return;

        // prekroceni hloubky
        if (moves.size() > maxDept) return;

        // uz neni mozne nalezt lepsi tah
        if (moves.size() + (blackCount - deadBlackList.size()) > minMoves) return;

        // vyhod cernou
        if (isBlack(desk[queen.x][queen.y]) && !isDead(queen, deadBlackList)) {
            queen.printStar = true;
            deadBlackList.push_back(queen);
        }

        // pridej tah
        moves.push_back(queen);

        // printMoves(moves, deadBlackList);

        // nalezene reseni?
        if ((deadBlackList.size() == blackCount)) {

            // muze byt lepsi?
            if ((moves.size() - 1 < minMoves)) {
                // nastav kritickou
#pragma omp critical
                {
                    // znovu zkontroluj
                    if ((moves.size() - 1 < minMoves)) {
                        // nastav znovu kritickou sekci
                        minMoves = (int) moves.size() - 1;
                        minMovesPath = moves;
                    }
                }
                return;
            }
        }

        // najdi vsechny mozne tahy
        vector<Move> availableMovesList;
        availableMoves(availableMovesList, queen, deadBlackList);

        // aplikuj rekurzi na vsechny mozne tahy
        for (auto move : availableMovesList) {
            if (moves.size() < 2) {
#pragma omp task
                {
                    findSolutionTaskParallel(move, deadBlackList, moves);
                }
            } else {
                findSolutionTaskParallel(move, deadBlackList, moves);
            }
        }
    }

    // rekurzivni funkce
    void findSolutionSeq(Solution solution) {

        // nalezeno optimalni reseni
        if (minMoves == blackCount) return;

        // prekroceni hloubky
        if (solution.moves.size() > maxDept) return;

        // uz neni mozne nalezt lepsi tah
        if (solution.moves.size() + (blackCount - solution.deadBlackList.size()) > minMoves) return;

        // vyhod cernou
        if (isBlack(desk[solution.queenPosition.x][solution.queenPosition.y]) &&
            !isDead(solution.queenPosition, solution.deadBlackList)) {
            solution.queenPosition.printStar = true;
            solution.deadBlackList.push_back(solution.queenPosition);
        }

        // pridej tah
        solution.moves.push_back(solution.queenPosition);

        // printMoves(moves, deadBlackList);

        // nalezene reseni? a muze byt lepsi?
        if ((solution.deadBlackList.size() == blackCount) && ((solution.moves.size() - 1 < minMoves))) {
            minMoves = (int) solution.moves.size() - 1;
            minMovesPath = solution.moves;
            return;
        }

        // najdi vsechny mozne tahy
        vector<Move> availableMovesList;
        availableMoves(availableMovesList, solution.queenPosition, solution.deadBlackList);

        // aplikuj rekurzi na vsechny mozne tahy
        for (auto move : availableMovesList) {
            solution.queenPosition = move;

            findSolutionSeq(solution);
        }
    }


};

int main(int argc, char *argv[]) {
    ifstream file("/home/samik/CLionProjects/MI-PDP-semestral/data/kralovna07.txt");

    // velikost hraci plochy, maximalni hloubcinka (omezeni), cernych figurek
    Game game;
    game.readInfo(file);
    game.readData(file);

    // game.printData();
    game.findBestSolutionSeq();
    game.reset();
    game.findBestSolutionDataParallel();

    return 0;
}
