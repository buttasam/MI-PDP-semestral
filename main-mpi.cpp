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

using namespace std;

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

int * movesVector2Ints(vector<Move> &moves, int &size) {
    size = 2 * moves.size();
    int* array = new int[size];

    for(int i = 0; i < moves.size(); i += 1) {
        array[2 * i] = moves.at(i).x;
        array[2 * i + 1] = moves.at(i).y;
    }

    return array;
}


void prepareDataToSend(Solution &solution, char * buffer, int& position) {
    // deadBlack list
    int deadBlackIntsSize = 0;
    int * deadBlackInts = movesVector2Ints(solution.deadBlackList, deadBlackIntsSize);

    // x souradnice kralovny
    MPI_Pack(&solution.queenPosition.x, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);
    // y souradnice kralovny
    MPI_Pack(&solution.queenPosition.y, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);

    // deadBlack slist size
    MPI_Pack(&deadBlackIntsSize, 1, MPI_INT, buffer, BUFFER_LENGTH, &position, MPI_COMM_WORLD);

    // deadBlack list
    for(int i = 0 ; i < deadBlackIntsSize; i++) {
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

int main(int argc, char **argv) {
    int my_rank, p;
    int tag = 1;
    MPI_Status status;
    char buffer[BUFFER_LENGTH];
    int position = 0;

    /* start up MPI */
    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        cout << "---Master--- there are " << p << " processes " << endl;

        Solution solution = testSolution();
        prepareDataToSend(solution, buffer, position);

        for (int i = 1; i < p; i++) {
            MPI_Send(buffer, position, MPI_PACKED, i, tag, MPI_COMM_WORLD);
        }

    } else {
        cout << "---Slave--- " << my_rank << endl;
        int queenX, queenY, deadBlackIntsSize;
        int currentMove;

        position = 0;
        MPI_Recv(buffer, BUFFER_LENGTH, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);


        MPI_Unpack(buffer, BUFFER_LENGTH, &position, &queenX, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, BUFFER_LENGTH, &position, &queenY, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, BUFFER_LENGTH, &position, &deadBlackIntsSize, 1, MPI_INT, MPI_COMM_WORLD);

        cout << "Queeen: " << queenX << "," << queenY << endl;
        for(int i = 0 ; i < deadBlackIntsSize; i++) {
            MPI_Unpack(buffer, BUFFER_LENGTH, &position, &currentMove, 1, MPI_INT, MPI_COMM_WORLD);
            cout << "current: " << currentMove << endl;
        }

    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}