#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include "Puzzle.h"

int timeArg = 60*10,memoryArg,ttSizeArg;
string algorithmArg,heuristicArg;
HEURISTIC heuristic;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Not enough arguments." << std::endl;
        return 1;
    }
    REMUSE = false;
    museMode = ALL_CHILDREN;
    if (argc >= 3) {
        if (std::stoi(std::string(argv[2])) == 1) REMUSE = true;
        if (argc >= 4) {
            if (std::stoi(std::string(argv[3])) == 1) museMode = ONLY_SOLUTION_CHILDREN;
        }
    }
	_dummy_static_initializer _dummy;

    std::string filename(argv[1]);
    std::ifstream file(filename);
    std::string line;

    std::getline(file, line);
    if(line.back()=='\r')line.pop_back();
    std::string name = line.substr(2, std::string::npos);
    std::string state;
    for (int i = 0; i < 7; ++i) {
        std::getline(file, line);
        if(line.back()=='\r')line.pop_back();
        state.append(line.substr(0, 7));
    }
    file.close();
    std::cout << state << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    Puzzle puzzle(name, state);
    int moves = -1;

    std::cout << "Puzzle:\n" << puzzle.toString() << std::endl;
    puzzle.init();

    moves = puzzle.Muse();

    std::stringstream timeString;
    timeString << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()/1000.0;
    // std::cout << "Solution: " << name << " " << timeString.str() << " " << moves << " " << puzzle.getSolution() << std::endl;
    std::cout << timeString.str() << std::endl;

    return 0;
}