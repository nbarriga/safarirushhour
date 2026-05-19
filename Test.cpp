#include "Common.h"
#include "Puzzle.h"
#include "State.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

int timeArg = 60;
int ttSizeArg = 1000000;
int maxSolLength = 1000;
std::string algorithmArg;
std::string heuristicArg;
std::string ttTypeArg = "normal";
int generateRelaxedArg = 0;
HEURISTIC heuristic;

// Helper to load puzzles from file
std::vector<std::pair<std::string, std::string>>
loadPuzzles(const std::string &filepath) {
  std::vector<std::pair<std::string, std::string>> puzzles;
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open puzzle file: " << filepath << std::endl;
    exit(1);
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line.back() == '\r')
      line.pop_back();
    if (line.empty() || std::isspace(line[0]))
      continue;
    if (line[0] == 'P') {
      std::string name = line.substr(2);
      while (!name.empty() && std::isspace(name.back()))
        name.pop_back();
      std::string state;
      for (int i = 0; i < 7; i++) {
        if (!std::getline(file, line))
          break;
        if (line.back() == '\r')
          line.pop_back();
        state.append(line.substr(0, 7));
      }
      puzzles.push_back({name, state});
    }
  }
  return puzzles;
}

// Helper to load solutions from file
void loadSolutions(
    const std::string &filepath,
    std::map<std::string, std::pair<int, std::vector<std::string>>>
        &expected_solutions) {
  std::ifstream file(filepath);
  if (!file.is_open())
    return;
  std::string line;
  while (std::getline(file, line)) {
    if (line.back() == '\r')
      line.pop_back();
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string tag, name;
    if (!(ss >> tag >> name))
      continue;
    if (tag != "solution")
      continue;

    std::string time_or_timeout;
    if (!(ss >> time_or_timeout))
      continue;
    if (time_or_timeout == "timeout") {
      expected_solutions[name] = {-1, {}};
      continue;
    }

    int moves_count;
    if (!(ss >> moves_count))
      continue;

    std::vector<std::string> moves;
    std::string mv;
    while (ss >> mv) {
      moves.push_back(mv);
    }
    expected_solutions[name] = {moves_count, moves};
  }
}

// Helper to verify that the provided moves solve the puzzle and contain no
// overlaps
bool verifyMoves(const std::string &state_str,
                 const std::vector<std::string> &moves) {
  State *current = new State(state_str);
  for (const auto &mv : moves) {
    if (mv.length() < 3) {
      delete current;
      return false;
    }
    State *next = current->applyMove(mv);

    // Check for out-of-bounds/illegal move
    const RowCol *new_pcs = next->getPiecesCopy();
    bool illegal = false;
    for (int p = 0; p < numPieces; p++) {
      if (new_pcs[p].packed == illegal_pos) {
        illegal = true;
        break;
      }
    }

    // Verify no overlaps in next state
    uint64_t occupied = 0;
    bool overlap = false;
    for (int p = 0; p < numPieces && !illegal; p++) {
      uint64_t p_board =
          piece_board[new_pcs[p].packed][index_orientation[p]][index_type[p]];
      if ((occupied & p_board) != 0) {
        overlap = true;
        break;
      }
      occupied |= p_board;
    }
    delete[] new_pcs;

    if (illegal || overlap) {
      delete current;
      delete next;
      return false;
    }

    delete current;
    current = next;
  }

  bool solved = current->isGoal();
  delete current;
  return solved;
}

int main(int argc, char *argv[]) {
  // Determine the test set, quiet flag, and optional BFS flag to run
  bool run_bfs = false;
  bool quiet = false;
  std::string test_set = "J1";
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--BFS") {
      run_bfs = true;
    } else if (arg == "--quiet") {
      quiet = true;
    } else {
      test_set = arg;
    }
  }

  if (!quiet) {
    std::cout << "Loading test set: " << test_set << std::endl;
    if (run_bfs) {
      std::cout << "BFS testing is ENABLED." << std::endl;
    }
  }

  std::string puzzle_file;
  bool is_individual = false;
  if (test_set == "J1") {
    puzzle_file = "J1";
  } else if (test_set == "J") {
    puzzle_file = "Jpuzzles";
  } else if (test_set == "B") {
    puzzle_file = "Bpuzzles";
  } else if (test_set == "I") {
    puzzle_file = "Ipuzzles";
  } else if (test_set == "A") {
    puzzle_file = "Apuzzles";
  } else if (test_set == "E") {
    puzzle_file = "Epuzzles";
  } else if (test_set == "all") {
    puzzle_file = "allPuzzles";
  } else {
    is_individual = true;
    if (!test_set.empty()) {
      char c = test_set[0];
      if (c == 'J')
        puzzle_file = "Jpuzzles";
      else if (c == 'B')
        puzzle_file = "Bpuzzles";
      else if (c == 'I')
        puzzle_file = "Ipuzzles";
      else if (c == 'A')
        puzzle_file = "Apuzzles";
      else if (c == 'E')
        puzzle_file = "Epuzzles";
      else
        puzzle_file = "allPuzzles";
    } else {
      puzzle_file = "allPuzzles";
    }
  }

  // Load standard data structures
  heuristicArg = "relaxed2";
  _dummy_static_initializer _dummy;

  // Load puzzles
  auto puzzles = loadPuzzles(puzzle_file);
  if (is_individual) {
    std::vector<std::pair<std::string, std::string>> filtered;
    for (const auto &p : puzzles) {
      if (p.first == test_set) {
        filtered.push_back(p);
      }
    }
    puzzles = filtered;
    if (puzzles.empty()) {
      std::cerr << "Error: Puzzle " << test_set << " not found in "
                << puzzle_file << std::endl;
      return 1;
    }
  }
  if (!quiet) {
    std::cout << "Loaded " << puzzles.size() << " puzzles." << std::endl;
  }

  // Load expected solutions
  std::map<std::string, std::pair<int, std::vector<std::string>>>
      expected_solutions;
  loadSolutions("allSolutions", expected_solutions);
  loadSolutions("ASolutions", expected_solutions);
  loadSolutions("BSolutions", expected_solutions);
  loadSolutions("ESolutions", expected_solutions);
  loadSolutions("ISolutions", expected_solutions);
  loadSolutions("JSolutions", expected_solutions);

  std::vector<std::string> algorithms = {"Astar", "IDAstar"};
  if (run_bfs) {
    algorithms.push_back("BFS");
  }
  std::vector<std::pair<std::string, HEURISTIC>> heuristics = {
      {"0", zero},
      {"210", twoonezero},
      {"obstacles0", obstacles0},
      {"obstacles1", obstacles1},
      {"obstacles2", obstacles2},
      {"obstacles3", obstacles3},
      {"relaxed1", relaxed1},
      {"relaxed2", relaxed2}};

  bool all_passed = true;

  for (const auto &puzzle_pair : puzzles) {
    const std::string &name = puzzle_pair.first;
    const std::string &state = puzzle_pair.second;

    if (!quiet) {
      std::cout << "----------------------------------------" << std::endl;
      std::cout << "Testing Puzzle: " << name << std::endl;
    } else {
      std::cout << "Testing Puzzle: " << name << "... " << std::flush;
    }

    bool puzzle_failed = false;

    // Find expected solution
    auto it = expected_solutions.find(name);
    if (it == expected_solutions.end()) {
      std::cout << "  [WARNING] No expected solution found in solution files! "
                   "Skipping validation."
                << std::endl;
      continue;
    }

    int expected_length = it->second.first;

    for (const auto &alg : algorithms) {
      for (const auto &heur : heuristics) {
        // BFS does not use heuristics for search sorting; test only once with
        // "0"
        if (alg == "BFS" && heur.first != "0")
          continue;

        if (!quiet) {
          std::cout << "  Algorithm: " << alg << ", Heuristic: " << heur.first
                    << "... " << std::flush;
        }

        algorithmArg = alg;
        heuristicArg = heur.first;
        heuristic = heur.second;

        if (alg == "BFS") {
          clusterCache = true;
          connectedComponents = true;
        } else {
          clusterCache = false;
          connectedComponents = false;
        }

        Puzzle pz(name, state);
        pz.init();

        auto startTime = std::chrono::high_resolution_clock::now();
        int solved_length = -1;
        if (alg == "Astar") {
          solved_length = pz.aStar();
        } else if (alg == "IDAstar") {
          solved_length = pz.idaStar(ttSizeArg, maxSolLength);
        } else if (alg == "BFS") {
          pz.BFS(ttSizeArg);
          solved_length = expected_length; // BFS doesn't return path length
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double durationMs =
            std::chrono::duration<double, std::milli>(endTime - startTime)
                .count();

        if (alg == "BFS") {
          if (!quiet) {
            std::cout << "PASSED (BFS completed without crash in " << durationMs
                      << "ms)" << std::endl;
          }
          continue;
        }

        if (solved_length == -1) {
          if (expected_length < 0) {
            if (!quiet) {
              std::cout << "PASSED (Expected TIMEOUT in " << durationMs << "ms)"
                        << std::endl;
            }
          } else {
            if (!quiet) {
              std::cout << "TIMEOUT (after " << durationMs << "ms)"
                        << std::endl;
            }
          }
          continue;
        }

        if (solved_length != expected_length) {
          if (quiet) {
            std::cout << "\n  [FAILED] " << alg << ", Heuristic: " << heur.first
                      << " - Expected length " << expected_length << ", got "
                      << solved_length << " (took " << durationMs << "ms)"
                      << std::endl;
          } else {
            std::cout << "FAILED! Expected length " << expected_length
                      << ", got " << solved_length << " (took " << durationMs
                      << "ms)" << std::endl;
          }
          all_passed = false;
          puzzle_failed = true;
          continue;
        }

        // Verify the moves the solver actually returned solve the puzzle
        std::vector<std::string> solver_moves;
        std::stringstream ss(pz.getSolution());
        std::string mv;
        while (ss >> mv) {
          solver_moves.push_back(mv);
        }

        if ((int)solver_moves.size() != solved_length) {
          if (quiet) {
            std::cout << "\n  [FAILED] " << alg << ", Heuristic: " << heur.first
                      << " - Solver moves length " << solver_moves.size()
                      << " does not match solved length " << solved_length
                      << " (took " << durationMs << "ms)" << std::endl;
          } else {
            std::cout << "FAILED! Solver moves length " << solver_moves.size()
                      << " does not match solved length " << solved_length
                      << " (took " << durationMs << "ms)" << std::endl;
          }
          all_passed = false;
          puzzle_failed = true;
          continue;
        }

        bool moves_ok = verifyMoves(state, solver_moves);
        if (moves_ok) {
          if (!quiet) {
            std::cout << "PASSED! (took " << durationMs << "ms)" << std::endl;
          }
        } else {
          if (quiet) {
            std::cout << "\n  [FAILED] " << alg << ", Heuristic: " << heur.first
                      << " - Solver moves did not solve the puzzle or caused "
                         "collision!"
                      << std::endl;
          } else {
            std::cout << "FAILED! Solver moves did not solve the puzzle or "
                         "caused collision (took "
                      << durationMs << "ms)." << std::endl;
          }
          all_passed = false;
          puzzle_failed = true;
        }
      }
    }

    if (quiet) {
      if (puzzle_failed) {
        std::cout << "FAILED" << std::endl;
      } else {
        std::cout << "PASSED" << std::endl;
      }
    }
  }

  if (!quiet) {
    std::cout << "----------------------------------------" << std::endl;
  }
  if (all_passed) {
    std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
  } else {
    std::cout << "SOME TESTS FAILED!" << std::endl;
    return 1;
  }
}
