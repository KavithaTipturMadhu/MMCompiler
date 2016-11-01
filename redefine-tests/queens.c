/*
 ============================================================================
 Name        : queens.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define n 4

struct array{
	int state[n][n];
	int valid;
};

typedef struct array Board;

Board redefine_out_results[4];

Board queens(Board board, int newEntryX, int newEntryY) {
	int j, i;
	if(newEntryX!=0){
		j=0;
		if (board.state[newEntryX][j] != 0 || board.state[j][newEntryY] != 0||(((newEntryY - j - 1 > -1)
					&& board.state[newEntryX - j - 1][newEntryY - j - 1] != 0)
					|| ((newEntryY + j + 1) < n
							&& board.state[newEntryX - j - 1][newEntryY + j + 1] != 0))) {
				board.valid = 0;			
				return board;
			}
	}

	if(newEntryX!=1){
			j=1;
			if (board.state[newEntryX][1] != 0 || board.state[j][newEntryY] != 0||(((newEntryY - j - 1 > -1)
						&& board.state[newEntryX - j - 1][newEntryY - j - 1] != 0)
						|| ((newEntryY + j + 1) < n
								&& board.state[newEntryX - j - 1][newEntryY + j + 1] != 0))) {
					board.valid = 0;			
					return board;
				}
		}

	if(newEntryX!=2){
		j=2;
		if (board.state[newEntryX][1] != 0 || board.state[j][newEntryY] != 0||(((newEntryY - j - 1 > -1)
					&& board.state[newEntryX - j - 1][newEntryY - j - 1] != 0)
					|| ((newEntryY + j + 1) < n
							&& board.state[newEntryX - j - 1][newEntryY + j + 1] != 0))) {
				board.valid = 0;			
				return board;
			}
	}
	if(newEntryX!=3){
		j=3;
		if (board.state[newEntryX][1] != 0 || board.state[j][newEntryY] != 0||(((newEntryY - j - 1 > -1)
					&& board.state[newEntryX - j - 1][newEntryY - j - 1] != 0)
					|| ((newEntryY + j + 1) < n
							&& board.state[newEntryX - j - 1][newEntryY + j + 1] != 0))) {
				board.valid = 0;			
				return board;
			}
	}

	board.state[newEntryX][newEntryY] = 1;
	if (newEntryX == n-1) {
		return board;
	} else {
		Board result = queens(board, newEntryX + 1, 0);
		if(result.valid){
			return result;
		}
		result = queens(board, newEntryX + 1, 1);
		if(result.valid){
			return result;
		}
		result = queens(board, newEntryX + 1, 2);
		if(result.valid){
			return result;
		}
		result = queens(board, newEntryX + 1, 3);
		if(result.valid){
			return result;
		}

		
	}
}

void redefine_start() {
	struct array board;
	unsigned i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			board.state[i][j]=0;
		}
	}
	Board result1 = queens(board, 0, 0);
	Board result2 = queens(board, 0, 1);
	Board result3 = queens(board, 0, 2);
	Board result4 = queens(board, 0, 3);
	redefine_out_results[0]=result1;
	redefine_out_results[1]=result2;
	redefine_out_results[2]=result3;
	redefine_out_results[3]=result4;
}
