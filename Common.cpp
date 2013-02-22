/*
 * Common.cpp
 *
 *  Created on: Feb 21, 2013
 *      Author: nbarriga
 */

#include "Common.h"
uint8_t piece_type[256];
uint8_t piece_letter[20]={'_','u','v','x','o', 'q', 's','p', 'r','a', 'f','d',
		'g', 'j','b', 'h', 'i', 'k','c', 'e'};
uint16_t letter_piece[256];

uint64_t piece_board[1024];

//[direction][distance][starting piece]
uint64_t piece_board_paths[1024][1024];
uint16_t piece_neighbours[4][7][1024];

static _dummy_static_initializer _dummy;
uint16_t encode_piece(uint16_t letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	return (letter<<10) | (row<<7) | (col<<3) | (dir<<2) | (type);
}
uint16_t shorten(uint16_t piece){
	return piece&((1<<10)-1);
}
uint16_t encode_piece(uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	return (row<<7) | (col<<3) | (dir<<2) | (type);
}
piece_t decode_piece(uint16_t piece){
	return piece_t(piece_letter[(piece>>10)&31], (piece>>7)&7, (piece>>3)&15, (piece>>2)&1, piece&3);
}
uint64_t get_board(uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	uint64_t board=0;
	switch(type){
	case rover:
	case two_by_two:
		board=(3ull<<61) | (3ull<<52);
		break;
	case two_by_one:
		if(dir==HOR){
			board=(3ull<<61);
		}else{
			board=(1ull<<62) | (1ull<<53);
		}
		break;
	case three_by_one:
		if(dir==HOR){
			board=(7ull<<60);
		}else{
			board=(1ull<<62) | (1ull<<53) | (1ull<<44);
		}
		break;
	}
	board>>=col;
	board>>=row*9;
	return board;
}
uint16_t move(uint16_t piece, uint8_t direction, uint8_t distance){
	auto p=decode_piece(piece);
	switch(direction){
	case UP:
		if(p.dir==HOR && (p.type==two_by_one || p.type==three_by_one)){
			return illegal_piece;
		}
		if(p.row<distance){
			return illegal_piece;
		}
		p.row-=distance;
		break;
	case DOWN:
		if(p.dir==HOR && (p.type==two_by_one || p.type==three_by_one)){
			return illegal_piece;
		}
		if(p.row+p.Ysize+distance>7){
			return illegal_piece;
		}
		p.row+=distance;
		break;
	case LEFT:
		if(p.dir==VERT && (p.type==two_by_one || p.type==three_by_one)){
			return illegal_piece;
		}
		if(p.col<distance){
			return illegal_piece;
		}
		p.col-=distance;
		break;
	case RIGHT:
		if(p.dir==VERT && (p.type==two_by_one || p.type==three_by_one)){
			return illegal_piece;
		}
		if(p.type==rover && p.row==2){
			if(p.col+p.Xsize+distance>9 || (p.col+distance==6)){
				return illegal_piece;
			}
		}else{
			if(p.col+p.Xsize+distance>7){
				return illegal_piece;
			}
		}
		p.col+=distance;
		break;
	}
	return encode_piece(letter_piece[(uint8_t)p.letter],p.row,p.col,p.dir,p.type);
}
//uint64_t move(uint64_t board, uint8_t direction, uint8_t distance, uint8_t type, bool vertical){
//	uint16_t bits=countSetBits(board);
//	switch(direction){
//	case UP:
//		if(!vertical && (type==two_by_one || type==three_by_one)){
//			board=illegal;
//		}else{
//			board<<=(9*distance);
//			if(countSetBits(board)!=bits){
//				board=illegal;
//			}
//		}
//		break;
//	case DOWN:
//		if(!vertical && (type==two_by_one || type==three_by_one)){
//			board=illegal;
//		}else{
//			board>>=(9*distance);
//			if(countSetBits(board)!=bits){
//				board=illegal;
//			}
//		}
//		break;
//	case LEFT:
//		if(vertical && (type==two_by_one || type==three_by_one)){
//			board=illegal;
//		}else{
//			for(int i=1;i<=distance;i++){
//				board<<=1;
//				board&=inside;
//				if(countSetBits(board)!=bits){
//					board=illegal;
//					break;
//				}
//			}
//		}
//		break;
//	case RIGHT:
//		if(vertical && (type==two_by_one || type==three_by_one)){
//			board=illegal;
//		}else{
//			for(int i=1;i<=distance;i++){
//				board>>=1;
//				if(type!=rover){
//					board&=inside;
//				}else{
//					board&=inside_exit;
//				}
//				if(countSetBits(board)!=bits){
//					board=illegal;
//					break;
//				}
//			}
//		}
//		break;
//	}
//	return board;
//}
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
	for(auto i=0;i<20;i++){
		letter_piece[piece_letter[i]]=i;
		//cout<<"letter_piece["<<piece_letter[i]<<"]="<<i<<endl;;
		switch(piece_letter[i]){
		case '_':
			break;
		case 'x':
			piece_type[piece_letter[i]]=rover;
			for(int row=0;row<6;row++){
				for(int col=0;col<6;col++){
					uint64_t board=get_board(row,col,HOR,rover);
					uint16_t piece=encode_piece(row,col,HOR,rover);
					piece_board[piece]=board;
					for(int dir=0;dir<4;dir++){
						for(int distance=1;distance<=7;distance++){
							uint16_t new_piece=move(piece,dir,distance);
							piece_neighbours[dir][distance-1][piece]=new_piece;
						}
					}
				}
			}
			for(int row=0;row<6;row++){
				for(int col=0;col<6;col++){
					uint16_t piece=encode_piece(row,col,HOR,rover);
					for(int dir=0;dir<4;dir++){
						uint64_t path=piece_board[piece];
						for(int distance=1;distance<=7;distance++){
							uint16_t new_piece=move(piece,dir,distance);
							path|=piece_board[new_piece];
							piece_board_paths[piece][new_piece]=path;
						}
					}
				}
			}
			{
				uint64_t board=get_board(2,7,HOR,rover);
				uint16_t goal_piece=encode_piece(2,7,HOR,rover);
				piece_board[goal_piece]=board;

				int row=2;
				for(int col=0;col<6;col++){
					uint16_t piece=encode_piece(row,col,HOR,rover);
					uint64_t path=piece_board[piece];
					for(int distance=1;distance<=7-col;distance++){
						uint16_t new_piece=move(piece,RIGHT,distance);
						path|=piece_board[new_piece];
					}
					piece_board_paths[piece][goal_piece]=path;
				}
			}
			break;
		case 'u':
		case 'v':
			piece_type[piece_letter[i]]=two_by_two;
			for(int row=0;row<6;row++){
				for(int col=0;col<6;col++){
					uint64_t board=get_board(row,col,HOR,two_by_two);
					uint16_t piece=encode_piece(row,col,HOR,two_by_two);
					piece_board[piece]=board;
					for(int dir=0;dir<4;dir++){
						for(int distance=1;distance<=7;distance++){
							uint16_t new_piece=move(piece,dir,distance);
							piece_neighbours[dir][distance-1][piece]=new_piece;
						}
					}
				}
			}
			for(int row=0;row<6;row++){
				for(int col=0;col<6;col++){
					uint16_t piece=encode_piece(row,col,HOR,two_by_two);
					for(int dir=0;dir<4;dir++){
						uint64_t path=piece_board[piece];
						for(int distance=1;distance<=7;distance++){
							uint16_t new_piece=move(piece,dir,distance);
							path|=piece_board[new_piece];
							piece_board_paths[piece][new_piece]=path;
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
			piece_type[piece_letter[i]]=three_by_one;
			for(int row=0;row<7;row++){
				for(int col=0;col<7;col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && col>=5)continue;
						if(orientation==VERT && row>=5)continue;
						uint64_t board=get_board(row,col,orientation,three_by_one);
						uint16_t piece=encode_piece(row,col,orientation,three_by_one);
						piece_board[piece]=board;
						for(int dir=0;dir<4;dir++){
							for(int distance=1;distance<=7;distance++){
								uint16_t new_piece=move(piece,dir,distance);
								piece_neighbours[dir][distance-1][piece]=new_piece;
							}
						}
					}
				}
			}
			for(int row=0;row<7;row++){
				for(int col=0;col<7;col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && col>=5)continue;
						if(orientation==VERT && row>=5)continue;
						uint16_t piece=encode_piece(row,col,orientation,three_by_one);
						for(int dir=0;dir<4;dir++){
							uint64_t path=piece_board[piece];
							for(int distance=1;distance<=7;distance++){
								uint16_t new_piece=move(piece,dir,distance);
								path|=piece_board[new_piece];
								piece_board_paths[piece][new_piece]=path;
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
			piece_type[piece_letter[i]]=two_by_one;
			for(int row=0;row<7;row++){
				for(int col=0;col<7;col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && col>=6)continue;
						if(orientation==VERT && row>=6)continue;
						uint64_t board=get_board(row,col,orientation,two_by_one);
						uint16_t piece=encode_piece(row,col,orientation,two_by_one);
						piece_board[piece]=board;
						for(int dir=0;dir<4;dir++){
							for(int distance=1;distance<=7;distance++){
								uint16_t new_piece=move(piece,dir,distance);
								piece_neighbours[dir][distance-1][piece]=new_piece;
							}
						}
					}
				}
			}
			for(int row=0;row<7;row++){
				for(int col=0;col<7;col++){
					for(int orientation=0;orientation<2;orientation++){
						if(orientation==HOR && col>=6)continue;
						if(orientation==VERT && row>=6)continue;
						uint16_t piece=encode_piece(row,col,orientation,two_by_one);
						for(int dir=0;dir<4;dir++){
							uint64_t path=piece_board[piece];
							for(int distance=1;distance<=7;distance++){
								uint16_t new_piece=move(piece,dir,distance);
								path|=piece_board[new_piece];
								piece_board_paths[piece][new_piece]=path;
							}
						}
					}
				}
			}
			break;
		}
	}
}
string piece_t::toString() const{
	std::ostringstream oss;
	oss<<letter<<row<<col<<dir<<type;

	return oss.str();
}
piece_t::piece_t(char letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type):
		letter(letter),
		row(row),
		col(col),
		dir(dir),
		type(type){
		if(type==rover || type==two_by_two){
			Xsize=Ysize=2;
		}else if(type==two_by_one){
			if(dir==HOR){
				Xsize=2;
				Ysize=1;
			}else{
				Xsize=1;
				Ysize=2;
			}
		}else{//if(type==three_by_one){
			if(dir==HOR){
				Xsize=3;
				Ysize=1;
			}else{
				Xsize=1;
				Ysize=3;
			}
		}
	}
