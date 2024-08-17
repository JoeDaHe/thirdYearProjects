/* vim: :se ai :se sw=4 :se ts=4 :se sts :se et */

/*H**********************************************************************
 *  AUTHOR: 			Johan Neethling
 *  STUDENT NUMBER: 	24739286
 * 
 *  PSEUDOCODE:
 *  	Root processor:
 * 			run_master:
 * 				Bcast colour ->
 *	 			Bcast running ->
 * 				Bcast board ->
 *		Other processors:
 *			run_worker:
 * 				Bcast colour <-
 *	 			Bcast running <-
 * 				Bcast temp_board <-
 * 			my_strategy:
 * 				claim moves per process
 *	 		MinMaxV:
 * 				find "goodness" per move using evaluateState function
 * 				choose best move per process using alpha-beta pruning and a mini-max strategy
 * 				MPI_Send move and corresponding value per process ->
 *	 	Root processor:
 * 			gen_move_master:
 * 				MPI_Recv moves and corresponding values <-
 * 				compare and choose best move
 *	 			loc = best move
 * 				make move
 * 			run_master:
 * 				print board
 * 		Other processors:
 * 			run_worker:
 * 				Bcast running <-
 * 
 *H***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <mpi.h>
#include <time.h>
#include <assert.h>
#include "comms.h"
#include <stdbool.h>

const int EMPTY = 0;
const int BLACK = 1;
const int WHITE = 2;

const int OUTER = 3;
const int ALLDIRECTIONS[8] = {-11, -10, -9, -1, 1, 9, 10, 11};
const int BOARDSIZE = 100;

const int LEGALMOVSBUFSIZE = 65;
const char piecenames[4] = {'.','b','w','?'};

void run_master(int argc, char *argv[]);
int initialise_master(int argc, char *argv[], int *time_limit, int *my_colour, FILE **fp);
void gen_move_master(char *move, int my_colour, FILE *fp);
void apply_opp_move(char *move, int my_colour, FILE *fp);
void game_over();
void run_worker();
void initialise_board();
void free_board();

void legal_moves(int player, int *moves, FILE *fp, int* local_board);
int legalp(int move, int player, FILE *fp, int* local_board);
int validp(int move);
int would_flip(int move, int dir, int player, FILE *fp, int* local_board);
int opponent(int player, FILE *fp);
int find_bracket_piece(int square, int dir, int player, FILE *fp, int* local_board);
int random_strategy(int my_colour, FILE *fp);
void make_move(int move, int player, FILE *fp, int* local_board);
void make_flips(int move, int dir, int player, FILE *fp, int* local_board);
int get_loc(char* movestring);
void get_move_string(int loc, char *ms);
void print_board(FILE *fp);
char nameof(int piece);
int count(int player, int* board);
void my_strategy(int my_colour, int* current_board, int depth, FILE* w_fp);
int minMaxV(int* temp_board, int depth, int alpha, int beta, bool maximizingPlayer, int my_colour, FILE* w_fp);
int min(int a, int b);
int max(int a, int b);
int evaluateState(int my_colour, int* board, FILE* w_fp);
void print_curr_board(int *my_board);

int *board;

int main(int argc, char *argv[]) {
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
	initialise_board(); //one for each process

	if (rank == 0) {
	    run_master(argc, argv);
	} else {
	    run_worker(rank);
	}
	game_over();
	MPI_Finalize();
}

void run_master(int argc, char *argv[]) {
	char cmd[CMDBUFSIZE];
	char my_move[MOVEBUFSIZE];
	char opponent_move[MOVEBUFSIZE];
	int time_limit;
	int my_colour;
	int running = 0;
	FILE *fp = NULL;

	if (initialise_master(argc, argv, &time_limit, &my_colour, &fp) != FAILURE) {
		running = 1;
	}
	if (my_colour == EMPTY) my_colour = BLACK;
	
	// Broadcast my_colour
	MPI_Bcast(&my_colour, 1, MPI_INT, 0, MPI_COMM_WORLD);

	while (running == 1) {

		/* Receive next command from referee */
		if (comms_get_cmd(cmd, opponent_move) == FAILURE) {
			fprintf(fp, "Error getting cmd\n");
			fflush(fp);
			running = 0;
			break;
		}

		/* Received game_over message */
		if (strcmp(cmd, "game_over") == 0) {
			running = 0;
			fprintf(fp, "Game over\n");
			fflush(fp);
			break;

		/* Received gen_move message */
		} else if (strcmp(cmd, "gen_move") == 0) {
			// Broadcast running
			MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
			
			// Broadcast board 
			MPI_Bcast(board, BOARDSIZE, MPI_INT, 0, MPI_COMM_WORLD);

			gen_move_master(my_move, my_colour, fp);
			print_board(fp);

			if (comms_send_move(my_move) == FAILURE) { 
				running = 0;
				fprintf(fp, "Move send failed\n");
				fflush(fp);
				break;
			}

		/* Received opponent's move (play_move mesage) */
		} else if (strcmp(cmd, "play_move") == 0) {
			apply_opp_move(opponent_move, my_colour, fp);
			print_board(fp);

		/* Received unknown message */
		} else {
			fprintf(fp, "Received unknown command from referee\n");
		}
	}
	// Broadcast running
	MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Finalize();
	fclose(fp);
}

int initialise_master(int argc, char *argv[], int *time_limit, int *my_colour, FILE **fp) {
	int result = FAILURE;

	if (argc == 5) { 
		unsigned long ip = inet_addr(argv[1]);
		int port = atoi(argv[2]);
		*time_limit = atoi(argv[3]);

		*fp = fopen(argv[4], "w");
		if (*fp != NULL) {
			fprintf(*fp, "Initialise communication and get player colour \n");
			if (comms_init_network(my_colour, ip, port) != FAILURE) {
				result = SUCCESS;
			}
			fflush(*fp);
		} else {
			fprintf(stderr, "File %s could not be opened", argv[4]);
		}
	} else {
		fprintf(*fp, "Arguments: <ip> <port> <time_limit> <filename> \n");
	}
	
	return result;
}

void initialise_board() {
	int i;
	board = (int *) malloc(BOARDSIZE * sizeof(int));
	for (i = 0; i <= 9; i++) board[i] = OUTER;
	for (i = 10; i <= 89; i++) {
		if (i%10 >= 1 && i%10 <= 8) board[i] = EMPTY; else board[i] = OUTER;
	}
	for (i = 90; i <= 99; i++) board[i] = OUTER;
	board[44] = WHITE; board[45] = BLACK; board[54] = BLACK; board[55] = WHITE;
}

void free_board() {
	free(board);
}

/**
 *  Initialize variables:
 * 		running, my_colour, temp_board recieved from the broadcast
 * 		depth represents the (number of splits in the mini-max function)-1
 *  Call my_strategy:
 * 		void function, therefor no value returned
 * 			
*/
void run_worker() {
	int* temp_board = (int*)malloc(BOARDSIZE * sizeof(int)); 
	int running     = 0;
	int depth       = 6;
	int my_colour, rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Bcast(&my_colour, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	char *filename= malloc(sizeof(char)*10);
	sprintf(filename,"%d_log.txt",rank);
	FILE* w_fp = fopen(filename,"w");

	if (w_fp == NULL) {
			printf("Failed to open file\n");
			return;
		}

	while (running == 1) {
		MPI_Bcast(temp_board, BOARDSIZE, MPI_INT, 0, MPI_COMM_WORLD);
		my_strategy(my_colour, temp_board, depth, w_fp);

		MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}

/**
 *  Initialize variables:
 * 		loc is final move location
 * 		hold represents current processes move and value being compared to best
 * 	Choose best move:
 * 		processors returned moves and their values in for loop
 * 		if the value of hold (current processor) id larger than that of best, update best with new value and move
 * 	Finalize and make move:
 * 		set loc as move of best
 * 		make_move for loc will apply the chosen best move
 *  
 */
void gen_move_master(char *move, int my_colour, FILE *fp) {
	int loc;
	int hold[2]   = {0, 0};				//tilePos - minimaxV
	int best[2]   = {0, -1000000};		//tilePos - minimaxV
	int num_ranks = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

	//RECEIVE MOVES AND MINIMAX VALUES
	for (int i = 0; i < (num_ranks-1); i++) {
		MPI_Recv(hold, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (hold[1] > best[1]) {
			best[0] = hold[0];
			best[1] = hold[1]; 
		}
	}
	loc = best[0];
	
	if (loc == -1) {
		strncpy(move, "pass\n", MOVEBUFSIZE);
	} else {
		get_move_string(loc, move);
		make_move(loc, my_colour, fp, board);
	}
}

void apply_opp_move(char *move, int my_colour, FILE *fp) {
	int loc;
	if (strcmp(move, "pass\n") == 0) {
		return;
	}
	loc = get_loc(move);
	make_move(loc, opponent(my_colour, fp), fp, board);
}

void game_over() {
	free_board();
	MPI_Finalize();
}

void get_move_string(int loc, char *ms) {
	int row, col, new_loc;
	new_loc = loc - (9 + 2 * (loc / 10));
	row = new_loc / 8;
	col = new_loc % 8;
	ms[0] = row + '0';
	ms[1] = col + '0';
	ms[2] = '\n';
	ms[3] = 0;
}

int get_loc(char* movestring) {
	int row, col;
	/* movestring of form "xy", x = row and y = column */ 
	row = movestring[0] - '0'; 
	col = movestring[1] - '0'; 
	return (10 * (row + 1)) + col + 1;
}

void legal_moves(int player, int *moves, FILE *fp, int* local_board) {
	int move, i;
	moves[0] = 0;
	i = 0;
	for (move = 11; move <= 88; move++) {
		if (legalp(move, player, fp, local_board)) {
      i++;
      moves[i] = move;
    }
	}
	moves[0] = i;
}

int legalp(int move, int player, FILE *fp,int* local_board) {
	int i;
	if (!validp(move)) return 0;
	if (local_board[move] == EMPTY) {
		i = 0;
		while (i <= 7 && !would_flip(move, ALLDIRECTIONS[i], player, fp, local_board)) i++;
		if (i == 8) return 0; else return 1;
	}
	else return 0;
}

int validp(int move) {
	if ((move >= 11) && (move <= 88) && (move%10 >= 1) && (move%10 <= 8))
		return 1;
	else return 0;
}

int would_flip(int move, int dir, int player, FILE *fp, int* local_board) {
	int c;
	c = move + dir;
	if (local_board[c] == opponent(player, fp))
		return find_bracket_piece(c+dir, dir, player, fp, local_board);
	else return 0;
}

int find_bracket_piece(int square, int dir, int player, FILE *fp, int* local_board) {
	while (validp(square) && local_board[square] == opponent(player, fp)) square = square + dir;
	if (validp(square) && local_board[square] == player) return square;
	else return 0;
}

int opponent(int player, FILE *fp) {
	if (player == BLACK) return WHITE;
	if (player == WHITE) return BLACK;
	fprintf(fp, "illegal player\n"); return EMPTY;
}

void make_move(int move, int player, FILE *fp, int* local_board) {
	int i;
	local_board[move] = player;
	for (i = 0; i <= 7; i++) make_flips(move, ALLDIRECTIONS[i], player, fp, local_board);
}

void make_flips(int move, int dir, int player, FILE *fp, int* local_board) {
	int bracketer, c;
	bracketer = would_flip(move, dir, player, fp, local_board);
	if (bracketer) {
		c = move + dir;
		do {
			local_board[c] = player;
			c = c + dir;
		} while (c != bracketer);
	}
}

void print_board(FILE *fp) {
	int row, col;
	fprintf(fp, "   1 2 3 4 5 6 7 8 [%c=%d %c=%d]\n",
		nameof(BLACK), count(BLACK, board), nameof(WHITE), count(WHITE, board));
	for (row = 1; row <= 8; row++) {
		fprintf(fp, "%d  ", row);
		for (col = 1; col <= 8; col++)
			fprintf(fp, "%c ", nameof(board[col + (10 * row)]));
		fprintf(fp, "\n");
	}
	fflush(fp);
}

char nameof(int piece) {
	assert(0 <= piece && piece < 5);
	return(piecenames[piece]);
}

int count(int player, int * board) {
	int i, cnt;
	cnt = 0;
	for (i = 1; i <= 88; i++)
		if (board[i] == player) cnt++;
	return cnt;
}

/**
 *  Initialize variables:
 * 		alpha represents negative infinity, while beta represents positive infinity
 * 		test is the value returned from each minMaxV call
 * 	Find best move:
 * 		the for loop and if statement work together to claim certain moves
 * 		per move we create a temp_board and apply the move
 * 		then we find the minMaxV for that move
 * 		compare and choose best minMaxV from the moves, set move that correlates with value
 * 	Return values:
 * 		best move and it's value is sent to gen_move_master function
 * 
*/
void my_strategy(int my_colour, int* current_board, int depth, FILE* w_fp) {
	int rank, num_ranks;
	int* temp_board = (int*)malloc(BOARDSIZE*sizeof(int));
	int* moveNdVal  = (int*)malloc(2*sizeof(int)); 
	int* moves      = (int*)malloc(LEGALMOVSBUFSIZE*sizeof(int));
	int alpha       = -1000;
	int beta        = 1000;
	int test        = -1;
	int best        = -10000;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

	moveNdVal[0] = 0;											//position of tile
	moveNdVal[1] = 0;											//minimaxV of move

	memset(moves, 0, LEGALMOVSBUFSIZE*sizeof(int));
	memcpy(temp_board, current_board, BOARDSIZE*sizeof(int));
	legal_moves(my_colour, moves, w_fp, temp_board);

	if (moves[0] == 0) {
		moveNdVal[0] = -1;
		moveNdVal[1] = 0;
	}
	else {
		for (int i = 1; i <= moves[0]; i++) {
			if ((i%(num_ranks-1)) == (rank-1)) {
				memcpy(temp_board, current_board, BOARDSIZE*sizeof(int));
				make_move(moves[i], my_colour, w_fp, temp_board);
				test = minMaxV(temp_board, depth, alpha, beta, true, my_colour, w_fp);
				if (test > best) {
					best = test;
					moveNdVal[0] = moves[i];
					moveNdVal[1] = test;
					
				}
			}
		}
	}
	free(temp_board);
	free(moves);
    MPI_Send(moveNdVal, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
	return;
}

/**
 * 	Initialize variables:
 * 		we bring in maximizingPlayer to see if we are looking to work from our perspective or the opponent's
 * 	Strategy:
 * 		look at future positions and evaluate how "good" they are
 * 		we find the leave's values and choose the best move (maximizes player and minimizes opponent)
 * 		ALPHA-BETA PRUNING removes positions that won't affect the outcome
 * 		alpha beta values keep track of best outcomes and improve efficiency
 * 
*/
int minMaxV(int* temp_board, int depth, int alpha, int beta, bool maximizingPlayer, int my_colour, FILE* w_fp) {
	int eval, lim, i;
	
	if (depth == 0) {
		eval = evaluateState(my_colour, temp_board, w_fp);
		return eval;
	}
	
	int *moves      = (int*)malloc(LEGALMOVSBUFSIZE*sizeof(int));
	int *mmv_board  = (int*)malloc(BOARDSIZE*sizeof(int)); 
	memset(moves, 0, LEGALMOVSBUFSIZE*sizeof(int));
	memcpy(mmv_board, temp_board, BOARDSIZE*sizeof(int));
	
	if (moves[0] == 0) 
		return evaluateState(my_colour, mmv_board, w_fp);
		
	if (maximizingPlayer) {
		lim = -10000;
		legal_moves(my_colour, moves, w_fp, mmv_board);

		for (i = 1; i <= moves[0]; i++) {
			make_move(moves[i], my_colour, w_fp, mmv_board);
			eval = minMaxV(mmv_board, depth-1, alpha, beta, false, my_colour, w_fp);
			lim = max(lim, eval);
			alpha = max(alpha, eval);
			if (beta <= alpha)
				break;
		}
		free(moves);
		free(mmv_board);
		free(w_fp);
		return lim;
	} else {
		lim = 10000;
		legal_moves(my_colour, moves, w_fp, mmv_board);

		for (i = 1; i <= moves[0]; i++) {
			make_move(moves[i], opponent(my_colour,w_fp), w_fp, mmv_board);
			eval = minMaxV(mmv_board, depth-1, alpha, beta, true, my_colour, w_fp);
			lim = min(lim, eval);
			beta = min(beta, eval);
			if (beta <= alpha)
				break;
		}
		free(moves);
		free(mmv_board);
		free(w_fp);
		return lim;
	}
	return 0;
}

int max(int a, int b) {
	if (a > b)
		return a;
	else		
		return b;
}

int min(int a, int b) {
	if (a > b) 
		return b;
	else	
		return a;
}

/**
 *  Simple:
 * 		we just compare the weight of our positions with those of the opposition
 * 	Less simple:
 * 		corners are prime locations
 * 		locations that make corners available are bad
 * 		mobilityWeight will add a mobility weight to the score returned, mobility is a positive factor
*/
int evaluateState(int my_colour, int* board, FILE* w_fp) {
	int playerMobility = 0;
	int opponentMobility = 0;
	int mobilityWeight = 15;
    int weights[100] = {
         0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    	 0,  50, -10,  10,   6,   6,  10, -10,  50, 0,
		 0, -10, -20,   1,   2,   2,   1, -20, -10, 0,
		 0,  10,   1,   5,   4,   4,   5,   1,  10, 0,
		 0,   6,   2,   4,   0,   0,   4,   2,   6, 0,
		 0,   6,   2,   4,   0,   0,   4,   2,   6, 0,
		 0,  10,   1,   5,   4,   4,   5,   1,  10, 0,	     
		 0, -10, -20,   1,   2,   2,   1, -20, -10, 0,
    	 0,  50, -10,  10,   6,   6,  10, -10,  50, 0,
		 0,   0,   0,   0,   0,   0,   0,   0,   0, 0
	};

    int score = 0;

    for (int i = 0; i < 100; i++) {
		if (board[i] == my_colour) {
    		score += weights[i];
    		playerMobility++;
		} else if (board[i] == opponent(my_colour, w_fp)) {
    		score -= weights[i];
    		opponentMobility++;
		}
	}
	score += (playerMobility - opponentMobility) * mobilityWeight;
    return score;
}