#include "Common.h"
#include "Puzzle.h"
#include <iostream>
#include <vector>
#include <string>

int timeArg = 60;
int ttSizeArg = 1000000;
int maxSolLength = 1000;
std::string algorithmArg;
std::string heuristicArg;
std::string ttTypeArg = "normal";
HEURISTIC heuristic;

int main() {
    // Initialize standard structures (piece boards, neighbor tables, etc.)
    _dummy_static_initializer _dummy;
    
    // J1 board parsed as 49 characters
    std::string state = 
        "......."
        ".rrrf.."
        "hh..fq."
        "o.xx.q."
        "o.xx.q."
        "ob..ee."
        ".bppp..";

    std::vector<std::string> algorithms = {"Astar", "IDAstar", "BFS"};
    std::vector<std::pair<std::string, HEURISTIC>> heuristics = {
        {"0", zero},
        {"210", twoonezero},
        {"obstacles0", obstacles0},
        {"obstacles1", obstacles1},
        {"obstacles2", obstacles2},
        {"obstacles3", obstacles3},
        {"relaxed1", relaxed1},
        {"relaxed2", relaxed2}
    };

    bool all_passed = true;

    for (const auto& alg : algorithms) {
        for (const auto& heur : heuristics) {
            // BFS doesn't use heuristics to sort search, so we only need to test it once
            if (alg == "BFS" && heur.first != "0") continue;

            algorithmArg = alg;
            heuristicArg = heur.first;
            heuristic = heur.second;

            // BFS specifically asserts that clusterCache and connectedComponents are true
            if (alg == "BFS") {
                clusterCache = true;
                connectedComponents = true;
            } else {
                clusterCache = false;
                connectedComponents = false;
            }

            std::cout << "Testing Algorithm: " << alg << ", Heuristic: " << heur.first << "... " << std::flush;

            Puzzle puzzle("J1", state);
            puzzle.init();
            
            int moves = -1;
            if (alg == "Astar") {
                moves = puzzle.aStar();
            } else if (alg == "IDAstar") {
                moves = puzzle.idaStar(ttSizeArg, maxSolLength);
            } else if (alg == "BFS") {
                puzzle.BFS(ttSizeArg);
                moves = 5; // BFS populates bfsTT but does not return a move count. If it didn't crash, it succeeded.
            }

            // Expected moves for J1 is 5
            if (alg != "BFS") {
                if (moves != 5) {
                    std::cout << "FAILED! Expected 5 moves, got " << moves << std::endl;
                    all_passed = false;
                } else {
                    std::cout << "PASSED!" << std::endl;
                }
            } else {
                std::cout << "PASSED (BFS completed without crash)" << std::endl;
            }
        }
    }

    if (all_passed) {
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
        return 0;
    } else {
        std::cout << "SOME TESTS FAILED!" << std::endl;
        return 1;
    }
}
