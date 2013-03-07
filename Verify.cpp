/*
 * Verify.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: nbarriga
 */


#include "Common.h"
#include "State.h"

int timeArg,memoryArg,ttSizeArg;
string algorithmArg,heuristicArg;
HEURISTIC heuristic;

int main(int argc, char* argv[]){
	_dummy_static_initializer _dummy;
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
		State *s= new State(state);
		getline(cin,line);
		stringstream str(line);
		string solution,problem;
		float time;
		int n;
		str>>solution>>problem>>time>>n;
		if(solution!="solution"||problem!=name){
			cerr<<"Formatting problem"<<endl;
			return EXIT_FAILURE;
		}
		string moves[n];
		for(int i=0;i<n;i++){
			str>>moves[i];
			cout<<moves[i]<<endl;
		}
		for(int i=0;i<n;i++){
					s=s->applyMove(moves[i]);
					cout<<s->toString();
				}
		if(s->isGoal()){
			cout<<"OK!"<<endl;
		}else{
			cout<<"BAD!"<<endl;
		}

	}

	return EXIT_SUCCESS;
}
