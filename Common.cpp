/*
 * Common.cpp
 *
 *  Created on: Feb 21, 2013
 *      Author: nbarriga
 */

#include "Common.h"
ORIENTATION ORIENTATIONS[]={HOR,VERT};
DIRECTION DIRECTIONS[]={UP,DOWN,LEFT,RIGHT};
PIECE_TYPE PIECE_TYPES[]={rover,two_by_two,three_by_one,two_by_one};
PIECE_TYPE piece_type[256];
uint8_t piece_letter[20]={'_','u','v','x','o', 'q', 's','p', 'r','a', 'f','d',
		'g', 'j','b', 'h', 'i', 'k','c', 'e'};
uint8_t letter_piece[256];

uint64_t piece_board[128][2][4];

uint64_t piece_board_paths[4][128][128];
RowCol piece_neighbours[4][7][128][4][2];

uint8_t type_sizeX[4][2];
uint8_t type_sizeY[4][2];

uint8_t index_letter[19];
uint8_t letter_index[256];
uint8_t numPieces;
PIECE_TYPE index_type[19];
ORIENTATION index_orientation[19];

uint64_t zobrist[256][2][4][2];

bool exit_obstacles[4][2][128];
bool exit_obstacles2[3][4][2][128];
bool exit_obstacles3[7][4][2][128];
bool rover_on_exit[3][128];

uint64_t get_board(RowCol rowCol,ORIENTATION orientation,PIECE_TYPE type){
	uint64_t board=0;
	switch(type){
	case rover:
	case two_by_two:
		board=(3ull<<61) | (3ull<<52);
		break;
	case two_by_one:
		if(orientation==HOR){
			board=(3ull<<61);
		}else{
			board=(1ull<<62) | (1ull<<53);
		}
		break;
	case three_by_one:
		if(orientation==HOR){
			board=(7ull<<60);
		}else{
			board=(1ull<<62) | (1ull<<53) | (1ull<<44);
		}
		break;
	}
	board>>=(int)rowCol.split.col;
	board>>=((int)rowCol.split.row)*9;
	return board;
}
RowCol move(RowCol rowCol,PIECE_TYPE type,ORIENTATION orientation, DIRECTION direction, uint8_t distance){
	RowCol illegal;
	illegal.packed=illegal_pos;
	int row=rowCol.split.row, col=rowCol.split.col;
	switch(direction){
	case UP:
		if(orientation==HOR && (type==two_by_one || type==three_by_one)){
			return illegal;
		}
		if(row<distance){
			return illegal;
		}
		row-=distance;
		break;
	case DOWN:
		if(orientation==HOR && (type==two_by_one || type==three_by_one)){
			return illegal;
		}
		if(row+type_sizeY[type][orientation]+distance>7){
			return illegal;
		}
		row+=distance;
		break;
	case LEFT:
		if(orientation==VERT && (type==two_by_one || type==three_by_one)){
			return illegal;
		}
		if(col<distance){
			return illegal;
		}
		col-=distance;
		break;
	case RIGHT:
		if(orientation==VERT && (type==two_by_one || type==three_by_one)){
			return illegal;
		}
		if(type==rover && row==2){
			if(col+type_sizeX[type][orientation]+distance>9 || (col+distance==6)){
				return illegal;
			}
		}else{
			if(col+type_sizeX[type][orientation]+distance>7){
				return illegal;
			}
		}
		col+=distance;
		break;
	}
	rowCol.split.row=row;
	rowCol.split.col=col;
	return rowCol;
}

void printBoard(uint64_t board){
	auto b=bitset<64>(board);
	for(int i=0;i<63;i++){
		if((i%9)==7){
			continue;
		}
		if((i%9)==8){
			cout<<endl;
			continue;
		}
		cout<<b[63-i];
	}
}
uint16_t countSetBits(uint64_t i) {
	i = i - ((i >> 1) & 0x5555555555555555UL);
	i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
	return (uint16_t)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}
_dummy_static_initializer::_dummy_static_initializer(){
	type_sizeX[rover][HOR]=2;
	type_sizeY[rover][HOR]=2;
	type_sizeX[rover][VERT]=2;
	type_sizeY[rover][VERT]=2;
	type_sizeX[two_by_two][HOR]=2;
	type_sizeY[two_by_two][HOR]=2;
	type_sizeX[two_by_two][VERT]=2;
	type_sizeY[two_by_two][VERT]=2;
	type_sizeX[three_by_one][HOR]=3;
	type_sizeY[three_by_one][HOR]=1;
	type_sizeX[three_by_one][VERT]=1;
	type_sizeY[three_by_one][VERT]=3;
	type_sizeX[two_by_one][HOR]=2;
	type_sizeY[two_by_one][HOR]=1;
	type_sizeX[two_by_one][VERT]=1;
	type_sizeY[two_by_one][VERT]=2;

	boost::random::mt19937_64 rng;//use a seed?
	for(auto i=0;i<256;i++){
		for(auto orientation=0;orientation<2;orientation++){
			for(auto type=0;type<4;type++){
				for(auto z=0;z<2;z++){
					zobrist[i][orientation][type][z]=rng();
				}
			}
		}
	}



	for(auto i=0;i<20;i++){

		letter_piece[piece_letter[i]]=i;

		switch(piece_letter[i]){
		case '_':
			break;
		case 'x':
		{
			piece_type[piece_letter[i]]=rover;
			RowCol rowCol;
			for(rowCol.split.row=0;rowCol.split.row<6;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<6;rowCol.split.col++){
					uint64_t board=get_board(rowCol,HOR,rover);
					piece_board[rowCol.packed][HOR][rover]=board;
					for(int dir=0;dir<4;dir++){
						for(int distance=1;distance<=7;distance++){
							RowCol new_pos=move(rowCol,rover,HOR,DIRECTIONS[dir],distance);
							piece_neighbours[dir][distance-1][rowCol.packed][rover][HOR]=new_pos;
						}
					}
				}
			}
			for(rowCol.split.row=0;rowCol.split.row<6;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<6;rowCol.split.col++){
					for(int dir=0;dir<4;dir++){
						uint64_t path=piece_board[rowCol.packed][HOR][rover];
						for(int distance=1;distance<=7;distance++){
							RowCol new_pos=piece_neighbours[dir][distance-1][rowCol.packed][rover][HOR];
							if(new_pos.packed==illegal_pos){
								continue;
							}
							path|=piece_board[new_pos.packed][HOR][rover];
							piece_board_paths[rover][rowCol.packed][new_pos.packed]=path;
						}
					}
				}
			}

			RowCol goalPos;
			goalPos.split.row=2;
			goalPos.split.col=7;
			uint64_t board=get_board(goalPos,HOR,rover);
			piece_board[goalPos.packed][HOR][rover]=board;


			rowCol.split.row=2;
			for(rowCol.split.col=0;rowCol.split.col<6;rowCol.split.col++){
				uint64_t path=piece_board[rowCol.packed][HOR][rover];
				for(int distance=1;distance<=7-rowCol.split.col;distance++){
					RowCol new_pos=move(rowCol,rover,HOR,RIGHT,distance);
					if(new_pos.packed==illegal_pos){
						continue;
					}
					path|=piece_board[new_pos.packed][HOR][rover];
				}
				piece_board_paths[rover][rowCol.packed][goalPos.packed]=path;
			}

		}
		break;
		case 'u':
		case 'v':
		{
			RowCol rowCol;
			piece_type[piece_letter[i]]=two_by_two;
			for(rowCol.split.row=0;rowCol.split.row<6;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<6;rowCol.split.col++){
					uint64_t board=get_board(rowCol,HOR,two_by_two);
					piece_board[rowCol.packed][HOR][two_by_two]=board;
					for(int dir=0;dir<4;dir++){
						for(int distance=1;distance<=7;distance++){
							RowCol new_pos=move(rowCol,two_by_two,HOR,DIRECTIONS[dir],distance);
							piece_neighbours[dir][distance-1][rowCol.packed][two_by_two][HOR]=new_pos;
						}
					}
				}
			}
			for(rowCol.split.row=0;rowCol.split.row<6;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<6;rowCol.split.col++){
					for(int dir=0;dir<4;dir++){
						uint64_t path=piece_board[rowCol.packed][HOR][two_by_two];
						for(int distance=1;distance<=7;distance++){
							RowCol new_pos=piece_neighbours[dir][distance-1][rowCol.packed][two_by_two][HOR];
							if(new_pos.packed==illegal_pos){
								continue;
							}
							path|=piece_board[new_pos.packed][HOR][two_by_two];
							piece_board_paths[two_by_two][rowCol.packed][new_pos.packed]=path;
						}
					}
				}
			}
		}
			break;
		case 'o':
		case 'q':
		case 's':
		case 'p':
		case 'r':
		{
			piece_type[piece_letter[i]]=three_by_one;
			RowCol rowCol;
			for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<7;rowCol.split.col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && rowCol.split.col>=5)continue;
						if(orientation==VERT && rowCol.split.row>=5)continue;
						uint64_t board=get_board(rowCol,ORIENTATIONS[orientation],three_by_one);
						piece_board[rowCol.packed][orientation][three_by_one]=board;
						for(int dir=0;dir<4;dir++){
							for(int distance=1;distance<=7;distance++){
								RowCol new_pos=move(rowCol,three_by_one,ORIENTATIONS[orientation],DIRECTIONS[dir],distance);
								piece_neighbours[dir][distance-1][rowCol.packed][three_by_one][orientation]=new_pos;
							}
						}
					}
				}
			}
			for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<7;rowCol.split.col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && rowCol.split.col>=5)continue;
						if(orientation==VERT && rowCol.split.row>=5)continue;
						for(int dir=0;dir<4;dir++){
							uint64_t path=piece_board[rowCol.packed][orientation][three_by_one];
							for(int distance=1;distance<=7;distance++){
								RowCol new_pos=piece_neighbours[dir][distance-1][rowCol.packed][three_by_one][orientation];
								if(new_pos.packed==illegal_pos){
									continue;
								}
								path|=piece_board[new_pos.packed][orientation][three_by_one];
								piece_board_paths[three_by_one][rowCol.packed][new_pos.packed]=path;
							}
						}
					}
				}
			}
		}
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
		{
			piece_type[piece_letter[i]]=two_by_one;
			RowCol rowCol;
			for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<7;rowCol.split.col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && rowCol.split.col>=6)continue;
						if(orientation==VERT && rowCol.split.row>=6)continue;
						uint64_t board=get_board(rowCol,ORIENTATIONS[orientation],two_by_one);
						piece_board[rowCol.packed][orientation][two_by_one]=board;
						for(int dir=0;dir<4;dir++){
							for(int distance=1;distance<=7;distance++){
								RowCol new_pos=move(rowCol,two_by_one,ORIENTATIONS[orientation],DIRECTIONS[dir],distance);
								piece_neighbours[dir][distance-1][rowCol.packed][two_by_one][orientation]=new_pos;
							}
						}
					}
				}
			}


			for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<7;rowCol.split.col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && rowCol.split.col>=6)continue;
						if(orientation==VERT && rowCol.split.row>=6)continue;
						for(int dir=0;dir<4;dir++){
							uint64_t path=piece_board[rowCol.packed][orientation][two_by_one];
							for(int distance=1;distance<=7;distance++){
								RowCol new_pos=piece_neighbours[dir][distance-1][rowCol.packed][two_by_one][orientation];
								if(new_pos.packed==illegal_pos){
									continue;
								}
								path|=piece_board[new_pos.packed][orientation][two_by_one];
								piece_board_paths[two_by_one][rowCol.packed][new_pos.packed]=path;
							}
						}
					}
				}
			}
		}
			break;
		}
	}

	for(auto type=0;type<4;type++){
		for(auto orientation=0;orientation<2;orientation++){
			RowCol rowCol;
			for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
				for(rowCol.split.col=0;rowCol.split.col<7;rowCol.split.col++){
					exit_obstacles[type][orientation][rowCol.packed]=
							(piece_board[rowCol.packed][orientation][type]&pre_exit)!=0;
					for(int i=0;i<3;i++){
						exit_obstacles2[i][type][orientation][rowCol.packed]=
								(piece_board[rowCol.packed][orientation][type]&pre_pre_exit[i])!=0;
					}
					for(int i=0;i<7;i++){
						exit_obstacles3[i][type][orientation][rowCol.packed]=
								(piece_board[rowCol.packed][orientation][type]&pre_pre_pre_exit[i])!=0;
					}
				}
			}
		}
	}

	RowCol rowCol;
	for(rowCol.split.row=0;rowCol.split.row<7;rowCol.split.row++){
		for(rowCol.split.col=0;rowCol.split.col<9;rowCol.split.col++){
			rover_on_exit[0][rowCol.packed]=
					(piece_board[rowCol.packed][HOR][rover]&pre_exit)!=0;
			bool on_exit=false;
			for(int i=0;i<3;i++){
				on_exit=on_exit||((piece_board[rowCol.packed][HOR][rover]&pre_pre_exit[i])!=0);
			}
			rover_on_exit[1][rowCol.packed]=on_exit;
			on_exit=false;
			for(int i=0;i<7;i++){
				on_exit=on_exit||((piece_board[rowCol.packed][HOR][rover]&pre_pre_pre_exit[i])!=0);
			}
			rover_on_exit[2][rowCol.packed]=on_exit;
		}
	}
}
