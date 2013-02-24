/*
 * Safari.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Common.h"
#include "Puzzle.h"


int main(){
	auto start =boost::posix_time::microsec_clock::local_time();
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
		//cout<<puzzle.toString();
		//cout<<"Result: "<<endl;
//		cout<<puzzle.aStar()<<endl;
		puzzle.aStar();
		cout<<"Solved "<<name<<" in "<<(boost::posix_time::microsec_clock::local_time() -start).total_milliseconds()<<"[ms]"<<endl;

		//cout<<"END"<<endl;
	}
}
