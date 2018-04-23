#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <queue>
#include "mpi.h"
#include <vector>
#include <stdio.h>

#define BUFFER_LENGTH 100
#define QUEUE_SIZE 20
#define TAG_NEW_WORK 1 // nova prace
#define TAG_RESULT 2 // odeslane reseni
#define TAG_END 3 // ukonceni Slavu

const int MAX_SIZE = 20;

using namespace std;

class Move {
public:
    Move() {
    }

    Move(int x, int y) {
        this->x = x;
        this->y = y;
        isBlack = false;
    }

    int x, y;
    bool isBlack;
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
            if (move.isBlack) cout << "*";
        }
        cout << endl;
    }


    void reset() {
        minMoves = maxDept;
        minMovesPath.clear();
    }


    void findSolutionBFS(deque<Solution> &queueSolutions) {
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
            lastSolution.queenPosition.isBlack = true;
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

private:
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
            queen.isBlack = true;
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
    void findSolutionSeq(Solution solution, bool critical) {

        // nalezeno optimalni reseni
        if (minMoves == blackCount) return;

        // prekroceni hloubky
        if (solution.moves.size() > maxDept) return;

        // uz neni mozne nalezt lepsi tah
        if (solution.moves.size() + (blackCount - solution.deadBlackList.size()) > minMoves) return;

        // vyhod cernou
        if (isBlack(desk[solution.queenPosition.x][solution.queenPosition.y]) &&
            !isDead(solution.queenPosition, solution.deadBlackList)) {
            solution.queenPosition.isBlack = true;
            solution.deadBlackList.push_back(solution.queenPosition);
        }

        // pridej tah
        solution.moves.push_back(solution.queenPosition);

        // printMoves(moves, deadBlackList);

        // nalezene reseni? a muze byt lepsi?
        if (solution.deadBlackList.size() == blackCount) {
            if (critical) {
                // muze byt lepsi?
                if ((solution.moves.size() - 1 < minMoves)) {
                    // nastav kritickou
#pragma omp critical
                    {
                        // znovu zkontroluj
                        if ((solution.moves.size() - 1 < minMoves)) {
                            // nastav znovu kritickou sekci
                            minMoves = (int) solution.moves.size() - 1;
                            minMovesPath = solution.moves;
                        }
                    }
                    return;
                }

            } else {
                if (solution.moves.size() - 1 < minMoves) {
                    minMoves = (int) solution.moves.size() - 1;
                    minMovesPath = solution.moves;
                    return;
                }
            }
        }

        // najdi vsechny mozne tahy
        vector<Move> availableMovesList;
        availableMoves(availableMovesList, solution.queenPosition, solution.deadBlackList);

        // aplikuj rekurzi na vsechny mozne tahy
        for (auto move : availableMovesList) {
            solution.queenPosition = move;

            findSolutionSeqCritical(solution);
        }
    }


    void findSolutionSeqCritical(Solution &solution) {
        findSolutionSeq(solution, true);
    }

    void findSolutionSeqNormal(Solution &solution) {
        findSolutionSeq(solution, false);
    }

};


int *movesVector2Ints(vector<Move> &moves, int &size) {
    size = 2 * moves.size();
    int *array = new int[size];

    for (int i = 0; i < moves.size(); i += 1) {
        array[2 * i] = moves.at(i).x;
        array[2 * i + 1] = moves.at(i).y;
    }

    return array;
}


void prepareDataToSend(Solution &solution, int *buffer, int &position) {
    // deadBlack list
    int deadBlackIntsSize = 0;
    int *deadBlackInts = movesVector2Ints(solution.deadBlackList, deadBlackIntsSize);

    position = 0;
    // x souradnice kralovny
    MPI_Pack(&solution.queenPosition.x, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);
    // y souradnice kralovny
    MPI_Pack(&solution.queenPosition.y, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);

    // deadBlack slist size
    MPI_Pack(&deadBlackIntsSize, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);

    // deadBlack list
    for (int i = 0; i < deadBlackIntsSize; i++) {
        MPI_Pack(&deadBlackInts[i], 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);
    }
}

Solution testSolution() {
    Solution solution;

    vector<Move> deadBlack;
    Move d1(4, 2);
    Move d2(8, 3);
    deadBlack.push_back(d1);
    deadBlack.push_back(d2);

    vector<Move> moves;
    Move m1(1, 2);
    Move m2(3, 4);
    moves.push_back(m1);
    moves.push_back(m2);

    solution.moves = moves;
    solution.deadBlackList = deadBlack;
    Move move1(7, 8);
    solution.queenPosition = move1;

    return solution;
}

void sendEnding(int& processCount, int *buffer, int &position) {
    for (int i = 1; i < processCount; i++) {
        MPI_Send(buffer, position, MPI_PACKED, i, TAG_END, MPI_COMM_WORLD);
    }
}

void sendInitDataToSlaves(deque<Solution>& queueSolutions, int& processCount, Game& game, int *buffer, int &position) {
    cout << "---Master--- there are " << processCount << " processes " << endl;
    // pocatecni reseni
    Solution solution;
    solution.queenPosition = game.queen;

    // fronta reseni
    queueSolutions.push_back(solution);

    // napln frontu
    while (queueSolutions.size() <= QUEUE_SIZE) {
        game.findSolutionBFS(queueSolutions);
    }

    // odesli prvni praci vsem slavum
    for (int i = 1; i < processCount; i++) {
        solution = queueSolutions.front(); // vezmi prvek z fronty
        queueSolutions.pop_front(); // odeber prvek z fronty

        prepareDataToSend(solution, buffer, position);
        MPI_Send(buffer, position, MPI_PACKED, i, TAG_NEW_WORK, MPI_COMM_WORLD);

        cout << "sending solution: " << solution.queenPosition.x << ", " << solution.queenPosition.y << endl;
    }
}


void processResults(deque<Solution>& queueSolutions, int& processCount, int *buffer, int &position) {
    MPI_Status status;
    int result;
    int working = processCount - 1;
    // prijeti reseni
    while (!queueSolutions.empty() || (working > 0)) {
        MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);
        cout << "result: " << result << " queue size: " << queueSolutions.size() << endl;


        if(!queueSolutions.empty()) {
            Solution solution = queueSolutions.front(); // vezmi prvek z fronty
            queueSolutions.pop_front(); // odeber prvek z fronty

            prepareDataToSend(solution, buffer, position);
            MPI_Send(buffer, position, MPI_PACKED, status.MPI_SOURCE, TAG_NEW_WORK, MPI_COMM_WORLD);
            position = 0;
            cout << "sending solution: " << solution.queenPosition.x << ", " << solution.queenPosition.y << endl;

        } else {
            working--;
            // cout << "working put down: " << working << endl;
        }
    }
}

int main(int argc, char **argv) {
    int my_rank, p;
    MPI_Status status;
    int buffer[BUFFER_LENGTH];
    int position = 0;

    /* start up MPI */
    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    ifstream file("/home/samik/CLionProjects/MI-PDP-semestral/data/kralovna01.txt");

    Game game;
    game.readInfo(file);
    game.readData(file);

    if (my_rank == 0) {
        // fronta reseni
        deque<Solution> queueSolutions;

        // odeslani slavum
        sendInitDataToSlaves(queueSolutions, p, game, buffer, position);

        // cekej na prijeti a posilani nove prace slavum
        processResults(queueSolutions, p, buffer, position);

        // posli vsem vlaknum info o ukonceni
        sendEnding(p, buffer, position);
    } else {
        cout << "---Slave--- " << my_rank << endl;
        int currentMove, queenX, queenY, deadBlackIntsSize;
        bool isAlive = true;

        while(isAlive) {
            MPI_Recv(buffer, BUFFER_LENGTH, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if(status.MPI_TAG == TAG_NEW_WORK) {
                position = 0;

                MPI_Unpack(buffer, BUFFER_LENGTH, &position, &queenX, 1, MPI_INT, MPI_COMM_WORLD);
                MPI_Unpack(buffer, BUFFER_LENGTH, &position, &queenY, 1, MPI_INT, MPI_COMM_WORLD);
                MPI_Unpack(buffer, BUFFER_LENGTH, &position, &deadBlackIntsSize, 1, MPI_INT, MPI_COMM_WORLD);


                // nastaveni hodnot
                game.queen.x = queenX;
                game.queen.y = queenY;

                cout << "receaving solution: " << game.queen.x << ", " << game.queen.y << endl;

                for (int i = 0; i < deadBlackIntsSize; i++) {
                    MPI_Unpack(buffer, BUFFER_LENGTH, &position, &currentMove, 1, MPI_INT, MPI_COMM_WORLD);
                }

                // vypocet reseni
                game.findBestSolutionTaskParallel();

                // odeslani reseni;
                MPI_Send(&game.minMoves, 1, MPI_INT, 0, TAG_RESULT, MPI_COMM_WORLD);
            } else if (status.MPI_TAG == TAG_END){
                isAlive = false;
            }
        }
    }

    /* shut down MPI */
    MPI_Finalize();
    cout << "---Ended--- " << my_rank << endl;

    return 0;
}