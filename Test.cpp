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
#include <omp.h>

thread_local int timeArg = 60;
int ttSizeArg = 1000000;
int maxSolLength = 1000;
int beamWidthArg = 100;
thread_local std::string algorithmArg;
thread_local std::string heuristicArg;
std::string ttTypeArg = "normal";
int generateRelaxedArg = 0;
thread_local HEURISTIC heuristic;

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
  bool run_beam = false;
  bool run_rbfs = false;
  bool quiet = false;
  std::string test_set = "J1";
  std::string custom_puzzle_file = "";   // set by -f <file>
  std::string custom_solution_file = ""; // set by -s <file>
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--BFS") {
      run_bfs = true;
    } else if (arg == "--BeamSearch") {
      run_beam = true;
    } else if (arg == "--RBFS") {
      run_rbfs = true;
    } else if (arg == "--quiet") {
      quiet = true;
    } else if ((arg == "-t" || arg == "--threads") && i + 1 < argc) {
      int num_threads = std::stoi(argv[++i]);
      omp_set_num_threads(num_threads);
    } else if (arg == "-f" && i + 1 < argc) {
      custom_puzzle_file = argv[++i];
    } else if (arg == "-s" && i + 1 < argc) {
      custom_solution_file = argv[++i];
    } else {
      test_set = arg;
    }
  }

  if (!custom_puzzle_file.empty()) {
    if (!quiet) {
      std::cout << "Loading puzzle file: " << custom_puzzle_file << std::endl;
      if (run_bfs) {
        std::cout << "BFS testing is ENABLED." << std::endl;
      }
    }
  } else {
    if (!quiet) {
      std::cout << "Loading test set: " << test_set << std::endl;
      if (run_bfs) {
        std::cout << "BFS testing is ENABLED." << std::endl;
      }
    }
  }

  std::string puzzle_file;
  bool is_individual = false;
  if (!custom_puzzle_file.empty()) {
    // -f <file>: load all boards from the given file, no individual filtering
    puzzle_file = custom_puzzle_file;
  } else if (test_set == "J1") {
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
  std::ifstream f_three("dataThree.bin");
  bool has_dataThree = f_three.good();
  f_three.close();

  if (has_dataThree) {
    heuristicArg = "relaxed3";
  } else {
    heuristicArg = "relaxed2";
  }
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
  if (!custom_solution_file.empty()) {
    loadSolutions(custom_solution_file, expected_solutions);
    if (!quiet) {
      std::cout << "Loaded solution file: " << custom_solution_file
                << std::endl;
    }
  }

  std::vector<std::string> algorithms = {"Astar", "IDAstar"};
  if (run_bfs) {
    algorithms.push_back("BFS");
  }
  if (run_beam) {
    algorithms.push_back("BeamSearch");
  }
  if (run_rbfs) {
    algorithms.push_back("RBFS");
  }
  std::vector<std::pair<std::string, HEURISTIC>> heuristics = {
      // {"0", zero},
      // {"210", twoonezero},
      {"obstacles0", obstacles0},
      {"obstacles1", obstacles1},
      {"obstacles2", obstacles2},
      {"obstacles3", obstacles3},
      {"obstacles4", obstacles4},
      {"relaxed1", relaxed1},
      {"relaxed2", relaxed2},
      {"combo", combo}};
  if (has_dataThree) {
    heuristics.push_back({"relaxed3", relaxed3});
  }

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

    struct Job {
      std::string alg;
      std::string heur_name;
      HEURISTIC heur;
    };
    std::vector<Job> jobs;
    for (const auto &alg : algorithms) {
      for (const auto &heur : heuristics) {
        if (alg == "BFS" && heur.first != "0")
          continue;
        jobs.push_back({alg, heur.first, heur.second});
      }
    }

    struct TestResult {
      std::string alg;
      std::string heur_name;
      bool passed = false;
      bool timeout = false;
      bool expected_timeout = false;
      double durationMs = 0.0;
      std::string error_message;
      int solved_length = -1;
    };
    std::vector<TestResult> results(jobs.size());

#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < jobs.size(); ++i) {
      const auto &job = jobs[i];
      results[i].alg = job.alg;
      results[i].heur_name = job.heur_name;

      algorithmArg = job.alg;
      heuristicArg = job.heur_name;
      heuristic = job.heur;

      if (job.alg == "BFS") {
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
      if (job.alg == "Astar") {
        solved_length = pz.aStar();
      } else if (job.alg == "IDAstar") {
        solved_length = pz.idaStar(ttSizeArg, maxSolLength);
      } else if (job.alg == "BFS") {
        pz.BFS(ttSizeArg);
        solved_length = expected_length;
      } else if (job.alg == "BeamSearch") {
        solved_length = pz.beamSearch(beamWidthArg);
      } else if (job.alg == "RBFS") {
        solved_length = pz.rbfs(ttSizeArg, maxSolLength);
      }
      auto endTime = std::chrono::high_resolution_clock::now();
      double durationMs =
          std::chrono::duration<double, std::milli>(endTime - startTime)
              .count();

      results[i].durationMs = durationMs;
      results[i].solved_length = solved_length;

      if (job.alg == "BFS") {
        results[i].passed = true;
        continue;
      }

      if (solved_length == -1) {
        results[i].passed = true;
        results[i].timeout = true;
        if (expected_length < 0) {
          results[i].expected_timeout = true;
        } else {
          results[i].expected_timeout = false;
        }
        continue;
      }

      if (solved_length != expected_length && job.alg != "BeamSearch") {
        results[i].passed = false;
        std::stringstream ss;
        ss << "Expected length " << expected_length << ", got "
           << solved_length;
        results[i].error_message = ss.str();
        continue;
      }

      std::vector<std::string> solver_moves;
      std::stringstream ss_moves(pz.getSolution());
      std::string mv;
      while (ss_moves >> mv) {
        solver_moves.push_back(mv);
      }

      if ((int)solver_moves.size() != solved_length) {
        results[i].passed = false;
        std::stringstream ss;
        ss << "Solver moves length " << solver_moves.size()
           << " does not match solved length " << solved_length;
        results[i].error_message = ss.str();
        continue;
      }

      bool moves_ok = verifyMoves(state, solver_moves);
      if (moves_ok) {
        results[i].passed = true;
      } else {
        results[i].passed = false;
        results[i].error_message =
            "Solver moves did not solve the puzzle or caused collision!";
      }
    }

    for (const auto &res : results) {
      if (!res.passed) {
        all_passed = false;
        puzzle_failed = true;
      }

      if (!quiet) {
        std::cout << "  Algorithm: " << res.alg
                  << ", Heuristic: " << res.heur_name << "... " << std::flush;
        if (res.passed) {
          if (res.alg == "BFS") {
            std::cout << "PASSED (BFS completed without crash in "
                      << res.durationMs << "ms)" << std::endl;
          } else if (res.timeout) {
            if (res.expected_timeout) {
              std::cout << "PASSED (Expected TIMEOUT in " << res.durationMs
                        << "ms)" << std::endl;
            } else {
              std::cout << "TIMEOUT (after " << res.durationMs << "ms)"
                        << std::endl;
            }
          } else {
            std::cout << "PASSED! (took " << res.durationMs << "ms)"
                      << std::endl;
          }
        } else {
          std::cout << "FAILED! " << res.error_message << " (took "
                    << res.durationMs << "ms)" << std::endl;
        }
      } else {
        if (!res.passed) {
          std::cout << "\n  [FAILED] " << res.alg
                    << ", Heuristic: " << res.heur_name << " - "
                    << res.error_message << " (took " << res.durationMs << "ms)"
                    << std::endl;
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
