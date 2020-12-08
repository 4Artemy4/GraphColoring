#include <iostream>
#include <fstream>

using namespace std;

void printGroups(int *const *adjMatrix, int vertex, int color);

void initMatrix(int vertex, int **&adjMatrix, fstream &fin);

bool isNeighbourWithThisColorExists(int *const *adjMatrix, int vertex, int color, int i);

int main() {
    //adjustment matrix
    int **adjMatrix;

    //vertex count
    int vertex;

    //file with a matrix
    fstream fin("input.txt");

    for (int l = 0; l < 5; ++l) {

        fin >> vertex;
        vertex++;

        //init
        initMatrix(vertex, adjMatrix, fin);

        bool isComplete = false;
        int color = 0;
        while (!isComplete) {
            isComplete = true;
            color++;
            for (int i = 1; i < vertex; ++i) {
                if (adjMatrix[i][0] == 0) {
                    //if a neighbour with same color not found - fill current vertex
                    if (!isNeighbourWithThisColorExists(adjMatrix, vertex, color, i)) {
                        adjMatrix[i][0] = color;
                        adjMatrix[0][i] = color;
                        isComplete = false;
                    }
                }
            }
        }

        printGroups(adjMatrix, vertex, color);
    }
    return 0;
}

bool isNeighbourWithThisColorExists(int *const *adjMatrix, int vertex, int color, int i) {
    for (int j = 1; j < vertex; ++j) {
        if (adjMatrix[i][j] != 0 && adjMatrix[0][j] == color) {
            return true;
        }
    }
    return false;
}

void initMatrix(int vertex, int **&adjMatrix, fstream &fin) {
    adjMatrix = new int *[vertex];
    for (int i = 0; i < vertex; ++i) {
        adjMatrix[i] = new int[vertex];
    }

    //reading matrix from the file
    for (int i = 1; i < vertex; ++i) {
        for (int j = 1; j < vertex; ++j) {
            fin >> adjMatrix[i][j];
        }
    }
}

void printGroups(int *const *adjMatrix, int vertex, int color) {
    cout << "groups and vertexes:" << endl;
    for (int k = 1; k < color; ++k) {
        cout << "group " << k << ": ";
        for (int i = 1; i < vertex; ++i) {
            if (adjMatrix[i][0] == k) cout << i << " ";
        }
        cout << endl;
    }
}
