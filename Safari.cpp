/*
 * Safari.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Common.h"
#include "Puzzle.h"

namespace po = boost::program_options;
int timeArg,memoryArg,ttSizeArg,maxSolLength;
string algorithmArg,heuristicArg;
HEURISTIC heuristic;

int main(int argc, char* argv[]){	
	auto start =chrono::high_resolution_clock::now();

	// Declare the supported options.
	po::options_description desc("Allowed options");

	desc.add_options()
	    ("help", "prints this help message")
	    ("time,t", po::value<int>(&timeArg)->default_value(60), "set timeout (in seconds)")
	    ("heuristic,h", po::value<string>(&heuristicArg)->default_value("obstacles0"), "set heuristic")
	    ("algorithm,a", po::value<string>(&algorithmArg)->default_value("IDAstar"), "set algorithm")
	    ("memory,m", po::value<int>(&memoryArg)->default_value(200), "set memory limit (in MB)")
	    ("size,s", po::value<int>(&ttSizeArg)->default_value(10000000), "set TT size (in #entries)")
	    ("length,l", po::value<int>(&maxSolLength)->default_value(1000), "set maximum solution length to look for in IDA*")
	;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
	po::notify(vm);
//	cout<<timeArg<<" "<<memoryArg<<endl;
	if (vm.count("help")) {
	    cout << desc << "\n";
	    return 1;
	}
	if(heuristicArg=="0"){
		heuristic=zero;
	}else if(heuristicArg=="210"){
		heuristic=twoonezero;
	}else if(heuristicArg=="obstacles0"){
		heuristic=obstacles0;
	}else if(heuristicArg=="obstacles1"){
		heuristic=obstacles1;
	}else if(heuristicArg=="obstacles2"){
		heuristic=obstacles2;
	}else if(heuristicArg=="obstacles3"){
		heuristic=obstacles3;
	}else if(heuristicArg=="relaxed1"){
		heuristic=relaxed1;
	}else if(heuristicArg=="relaxed2"){
		heuristic=relaxed2;
	}else{
		cout << desc << "\n";
		return 1;
	}
	_dummy_static_initializer _dummy;
//	cout<<"Initialized in "<<(boost::posix_time::microsec_clock::local_time() -start).total_milliseconds()<<"[ms]"<<endl;

	struct rusage usage;
	getrusage(RUSAGE_SELF , &usage);


	int baseMemory=usage.ru_maxrss/1024;
	string line;
	while(getline(cin,line)){
		if(line.back()=='\r')line.pop_back();
		if(line.length()==0||isspace(line[0])||isblank(line[0])){
			continue;
		}
		if(line[0]!='P'){
			cerr<<"Formatting problem"<<endl;
			return 4;
		}
		string name= line.substr(2,string::npos);
		string state;
		for(int i=0;i<7;i++){
			getline(cin,line);
			if(line.back()=='\r')line.pop_back();
			state.append(line.substr(0,7));
		}
		start =chrono::high_resolution_clock::now();
		Puzzle puzzle(name,state);

		int moves=-1;
		puzzle.init(baseMemory);
		if(algorithmArg=="Astar"){
			moves=puzzle.aStar(memoryArg);
		}else if(algorithmArg=="IDAstar"){
			moves=puzzle.idaStar(ttSizeArg,maxSolLength);
			//cout<<"statistics "<<name<<" "<<puzzle.statistics()<<endl;
		}else if(algorithmArg=="fallback"){
			moves=puzzle.aStar(memoryArg);
			if(puzzle.isOverMemBudget()){
				//cout<<name<<" exceeded memory budget of "<<memoryArg<<"MB, reverting to IDA*"<<endl;
				moves=puzzle.idaStar(ttSizeArg,maxSolLength);
			}
		}

		stringstream timeString;
		timeString<<chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() -start).count()/1000.0;
		times.append(timeString.str()).append(" ");
		if(moves>=0){
			cout<<"solution "<<name<<" "<<timeString.str()<<
					" "<<moves<<" "<<puzzle.getSolution()<<endl;
			//cout<<puzzle.getSolution()<<endl;

		}else if(puzzle.isTimeOut()){
			cout<<"solution "<<name<<" "<<"timeout"<<endl;
		}else if(puzzle.isOverMemBudget()){
			cout<<"solution "<<name<<" "<<"timeout -> exceeded memory budget"<<endl;
		}else{
			if(algorithmArg=="IDAstar"){
				cout<<"solution "<<name<<" "<<"No solution exists with length < "<<maxSolLength<<endl;
			}else{
				cout<<"solution "<<name<<" "<<"No solution exists"<<endl;
			}
		}
		


		stringstream ss;
		ss<<puzzle.getInitialH();
		initialH.append(ss.str()).append(" ");

//		getrusage(RUSAGE_SELF , &usage);
//
//
//		cout<<"Usage: "<<usage.ru_maxrss/1024<<"MB"<<endl;
	}
//
//	cout<<"P"<<algorithmArg<<heuristicArg<<"={"<<Problems.substr(0,Problems.size()-1)<<"};"<<endl;
//	cout<<"T"<<algorithmArg<<heuristicArg<<"=["<<times<<"];"<<endl;
//
//	cout<<"N"<<algorithmArg<<heuristicArg<<"=["<<nodesSearched<<"];"<<endl;
//	cout<<"H"<<algorithmArg<<heuristicArg<<"=["<<initialH<<"];"<<endl;
//	cout<<"G"<<algorithmArg<<heuristicArg<<"=["<<actualSolution<<"];"<<endl;
//	cout<<"M"<<algorithmArg<<heuristicArg<<"=["<<memories<<"];"<<endl;

}
