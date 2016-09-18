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
typedef struct array {
	int state[n][n];
	bool valid;
}Board;

Board[4] redefine_out_results;
Board queens(Board board, int newEntryX, int newEntryY) {
	int j, i;
	for (j = 0; j < newEntryX; j++) {
		if (board.state[newEntryX][j] != 0 || board.state[j][newEntryY] != 0||(((newEntryY - j - 1 > -1)
				&& board.state[newEntryX - j - 1][newEntryY - j - 1] != 0)
				|| ((newEntryY + j + 1) < n
						&& board.state[newEntryX - j - 1][newEntryY + j + 1] != 0))) {
			board.valid = false;			
			return;
		}
	}

	board.state[newEntryX][newEntryY] = 1;
	if (newEntryX == n-1) {
		return board;
	} else {
		for (j = 0; j < n; j++) {
			Board result = queens(board, newEntryX + 1, j);
			if(result.valid){
				return result;
			}
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
