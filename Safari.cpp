/*
 * Safari.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Common.h"
#include "Puzzle.h"

namespace po = boost::program_options;
int timeArg;
string algorithmArg,heuristicArg;
HEURISTIC heuristic;

int main(int argc, char* argv[]){
	auto start =boost::posix_time::microsec_clock::local_time();

	// Declare the supported options.
	po::options_description desc("Allowed options");

	desc.add_options()
	    ("help", "prints this help message")
	    ("time,t", po::value<int>(&timeArg)->default_value(INT_MAX), "set timeout")
	    ("heuristic,h", po::value<string>(&heuristicArg)->default_value("210"), "set heuristic")
	    ("algorithm,a", po::value<string>(&algorithmArg)->default_value("Astar"), "set algorithm")
	;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
	po::notify(vm);

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
		return EXIT_FAILURE;
	}
	_dummy_static_initializer _dummy;
	cout<<"Initialized in "<<(boost::posix_time::microsec_clock::local_time() -start).total_milliseconds()<<"[ms]"<<endl;


	string line;
	while(getline(cin,line)){
		if(line==""){
			continue;
		}
		if(line[0]!='P'){
			cerr<<"Formatting problem"<<endl;
			return EXIT_FAILURE;
		}
		string name= line.substr(2,string::npos);
		string state;
		for(int i=0;i<7;i++){
			getline(cin,line);
			state.append(line.substr(0,7));
		}
		start =boost::posix_time::microsec_clock::local_time();
		Puzzle puzzle(name,state);

		int moves;
		if(algorithmArg=="Astar"){
			moves=puzzle.aStar();

		}else if(algorithmArg=="IDAstar"){
			moves=puzzle.idaStar();
		}else if(algorithmArg=="fallback"){
			//todo: implement a* with IDA* fallback
		}
		stringstream timeString;
		timeString<<(boost::posix_time::microsec_clock::local_time() -start).total_milliseconds()/1000.0;
		cout<<"solution "<<name<<" "<<
				timeString.str()<<
				" "<<moves<<" "<<puzzle.getSolution()<<endl;

		times.append(timeString.str()).append(" ");
		stringstream ss;
		ss<<puzzle.getInitialH();
		initialH.append(ss.str()).append(" ");
		//cout<<"END"<<endl;
	}

	cout<<"P"<<heuristicArg<<"={"<<Problems.substr(0,Problems.size()-1)<<"};"<<endl;
	cout<<"T"<<heuristicArg<<"=["<<times<<"];"<<endl;

	cout<<"N"<<heuristicArg<<"=["<<nodesSearched<<"];"<<endl;
	cout<<"H"<<heuristicArg<<"=["<<initialH<<"];"<<endl;
	cout<<"G"<<heuristicArg<<"=["<<actualSolution<<"];"<<endl;
	if(algorithmArg=="A*"){
		cout<<"M"<<heuristicArg<<"=["<<memories<<"];"<<endl;
	}
}
