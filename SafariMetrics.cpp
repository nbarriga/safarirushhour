// ===== INSTRUMENTATION =====
// Stand-alone driver (no boost needed) that solves Safari Rush Hour boards with A* and computes,
// for every board, the search, solution-path and structural metrics.
// Usage:
//   SafariMetrics <puzzle_file> [heuristic] [output.csv]
//   - puzzle_file: boards in the "P <name>" + 7 lines format (e.g. allPuzzles)
//   - heuristic:   0 | 210 | obstacles0 | obstacles1 | obstacles2 | obstacles3   (default: obstacles2)
//   - output.csv:  if given, one row per board is written; otherwise a report is printed per board
#include "Common.h"
#include "Puzzle.h"
#include <fstream>

int timeArg = 100000, ttSizeArg = 10000000, maxSolLength = 1000;
string algorithmArg = "Astar", heuristicArg, ttTypeArg = "normal";
HEURISTIC heuristic;

int main(int argc, char* argv[]){
	if(argc < 2){ cerr << "Usage: SafariMetrics <puzzle_file> [heuristic] [output.csv]" << endl; return 1; }
	heuristicArg = argc > 2 ? argv[2] : "obstacles2";
	if(heuristicArg == "0") heuristic = zero;
	else if(heuristicArg == "210") heuristic = twoonezero;
	else if(heuristicArg == "obstacles0") heuristic = obstacles0;
	else if(heuristicArg == "obstacles1") heuristic = obstacles1;
	else if(heuristicArg == "obstacles2") heuristic = obstacles2;
	else if(heuristicArg == "obstacles3") heuristic = obstacles3;
	else { cerr << "Unknown heuristic " << heuristicArg << endl; return 1; }
	useMapTT = false;
	_dummy_static_initializer _dummy;

	ifstream in(argv[1]);
	if(!in){ cerr << "Cannot open " << argv[1] << endl; return 1; }
	ofstream out;
	bool csv = argc > 3;
	if(csv){ out.open(argv[3]); out << Puzzle::metricsCsvHeader() << "\n"; }

	string line;
	while(getline(in, line)){
		if(!line.empty() && line.back() == '\r') line.pop_back();
		if(line.empty() || isspace(line[0])) continue;
		if(line[0] != 'P'){ cerr << "Formatting problem: " << line << endl; return 4; }
		string name = line.substr(2), state;
		for(int i = 0; i < 7; i++){
			getline(in, line);
			if(!line.empty() && line.back() == '\r') line.pop_back();
			state.append(line.substr(0, 7));
		}
		// letter_index is global and is only overwritten for the letters present in a board. The
		// equality test of State swaps pieces 'u' and 'v'; on a board with 'u' but no 'v' it would
		// use the 'v' index left by a previous board. Reset it so that every board is solved
		// exactly as in a fresh process, independently of the boards solved before it.
		memset(letter_index, 0, sizeof(letter_index));
		Puzzle puzzle(name, state);
		puzzle.init();
		int moves = puzzle.aStar();
		if(moves < 0){ cerr << "No solution: " << name << endl; continue; }
		if(csv){ out << puzzle.metricsCsvRow() << "\n"; out.flush(); cerr << "done " << name << endl; }
		else cout << "Board " << name << "\n" << puzzle.metricsReport() << endl;
	}
	return 0;
}
