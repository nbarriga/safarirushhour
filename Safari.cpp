#include "Common.h"
#include "Puzzle.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;
thread_local int timeArg;
int ttSizeArg, maxSolLength;
int generateRelaxedArg = 0;

thread_local string algorithmArg;
thread_local string heuristicArg;
string ttTypeArg;
thread_local HEURISTIC heuristic;

int main(int argc, char *argv[]) {

  // Declare the supported options.
  po::options_description desc("Allowed options");

  desc.add_options()("help", "prints this help message")(
      "time,t", po::value<int>(&timeArg)->default_value(60),
      "set timeout (in seconds)")(
      "heuristic,h",
      po::value<string>(&heuristicArg)->default_value("obstacles0"),
      "set heuristic")(
      "algorithm,a", po::value<string>(&algorithmArg)->default_value("IDAstar"),
      "set algorithm")(
      "size,s", po::value<int>(&ttSizeArg)->default_value(10000000),
      "set TT size (in #entries, approximately 16 bytes per node)")(
      "length,l", po::value<int>(&maxSolLength)->default_value(1000),
      "set maximum solution length to look for in IDA*")(
      "generate-relaxed,g",
      po::value<int>(&generateRelaxedArg)->default_value(0),
      "generate relaxed heuristics (1: dataOne, 2: dataOne & dataTwo)")(
      "cluster,c", "Enable caching of solutions between boards, for reuse in "
                   "the same cluster")(
      "typett",
      po::value<string>(&ttTypeArg)
          ->default_value("normal")
          ->implicit_value("stdin"),
      "set transposition table type (map = std::unordered_map, normal = array")(
      "ccomp", "Enable connected components");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  TTInterface *TT = nullptr;
  if (ttTypeArg == "map") {
    useMapTT = true;
  } else if (ttTypeArg == "normal")
    useMapTT = false;
  else {
    cerr << "No correct transposition table type specified." << endl;
    return 1;
  }

  if (vm.count("ccomp")) {
    connectedComponents = true;
  }

  if (vm.count("cluster")) {
    clusterCache = true;
    if (!connectedComponents) {
      if (useMapTT) {
        TT = new TranspositionTableMap(ttSizeArg);
      } else {
        TT = new TranspositionTable(ttSizeArg);
      }
    }
  }
  if (heuristicArg == "0") {
    heuristic = zero;
  } else if (heuristicArg == "210") {
    heuristic = twoonezero;
  } else if (heuristicArg == "obstacles0") {
    heuristic = obstacles0;
  } else if (heuristicArg == "obstacles1") {
    heuristic = obstacles1;
  } else if (heuristicArg == "obstacles2") {
    heuristic = obstacles2;
  } else if (heuristicArg == "obstacles3") {
    heuristic = obstacles3;
  } else if (heuristicArg == "obstacles4") {
    heuristic = obstacles4;
  } else if (heuristicArg == "relaxed1") {
    heuristic = relaxed1;
  } else if (heuristicArg == "relaxed2") {
    heuristic = relaxed2;
  } else if (heuristicArg == "relaxed3") {
    heuristic = relaxed3;
  } else {
    cout << desc << "\n";
    return 1;
  }
  _dummy_static_initializer _dummy;

  string line;
  while (getline(cin, line)) {
    if (line.back() == '\r')
      line.pop_back();
    if (line.length() == 0 || isspace(line[0]) || isblank(line[0])) {
      continue;
    }
    if (line[0] != 'P') {
      cerr << "Formatting problem" << endl;
      return 4;
    }
    string name = line.substr(2, string::npos);
    string state;
    for (int i = 0; i < 7; i++) {
      getline(cin, line);
      if (line.back() == '\r')
        line.pop_back();
      state.append(line.substr(0, 7));
    }

    Puzzle puzzle(name, state);
    if (clusterCache && !connectedComponents) {
      puzzle.setTTForReuse(TT);
    }
    int moves = -1;
    puzzle.init();
    auto startTime = chrono::high_resolution_clock::now();
    if (algorithmArg == "Astar") {
      moves = puzzle.aStar();
    } else if (algorithmArg == "IDAstar") {
      moves = puzzle.idaStar(ttSizeArg, maxSolLength);
    } else if (algorithmArg == "BFS") {
      puzzle.BFS(ttSizeArg);
      /*for(auto tt : TTs)
      {
              cout << tt->statistics() << endl;
      }*/
    }
    auto endTime = chrono::high_resolution_clock::now();
    double durationMs = chrono::duration<double, std::milli>(endTime - startTime).count();

    if (algorithmArg == "IDAstar" && moves >= 0) {
      cout << "statistics " << name << " " << puzzle.statistics() << endl;
    }

    stringstream timeString;
    timeString << durationMs;
    times.append(timeString.str()).append(" ");
    if (moves >= 0) {
      cout << "solution " << name << " " << timeString.str() << " " << moves
           << " " << puzzle.getSolution() << endl;
    } else if (puzzle.isTimeOut()) {
      cout << "solution " << name << " " << "timeout" << endl;
    } else {
      if (algorithmArg == "IDAstar") {
        cout << "solution " << name << " "
             << "No solution exists with length < " << maxSolLength << endl;
      } else if (algorithmArg == "Astar") {
        cout << "solution " << name << " " << "No solution exists" << endl;
      }
    }
  }
  if (clusterCache && !connectedComponents) {
    delete TT;
  } else if (connectedComponents) {
    for (auto tt : TTs) {
      cout << tt->statistics() << endl;
      delete tt;
    }
  }
}
