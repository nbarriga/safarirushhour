/*
 * Common.h
 *
 *  Created on: Jan 23, 2013
 *      Author: nbarriga
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <string>

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <cmath>
#include <sstream>
#include <bitset>
#include <utility>
#include <climits>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cinttypes>
#include <boost/random.hpp>
#include <queue>
#include <unordered_set>
#include <functional>
#include <boost/functional/hash/hash.hpp>
using namespace std;


static uint8_t piece_type[256];
static char piece_letter[19]={'u','v','x','o', 'q', 's','p', 'r','a', 'f','d',
		'g', 'j','b', 'h', 'i', 'k','c', 'e'};


struct _dummy_static_initializer{
	_dummy_static_initializer(){
		for(auto i=0;i<19;i++){
			switch(piece_letter[i]){
					case 'x':
						piece_type[piece_letter[i]]=0;
						break;
					case 'u':
					case 'v':
						piece_type[piece_letter[i]]=1;
						break;
					case 'o':
					case 'q':
					case 's':
					case 'p':
					case 'r':
						piece_type[piece_letter[i]]=2;
						break;
					case 'a':
					case 'f':
					case 'd':
					case 'g':
					case 'j':
					case 'b':
					case 'h':
					case 'i':
					case 'k':
					case 'c':
					case 'e':
						piece_type[piece_letter[i]]=3;
						break;
					}
		}
	}
};
#endif /* COMMON_H_ */
