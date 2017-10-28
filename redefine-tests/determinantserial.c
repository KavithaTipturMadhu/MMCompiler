//#include<stdio.h>

int redefine_out_det;

int mod(int dividend, int divisor){
	while(dividend>=divisor){
		dividend = dividend-divisor;
	}	
	return dividend;
}

void redefine_start(){
	int a[3][3]; //assume 3x3 matrix
	int i,j;
	int d = 0;
	
	/*printf("Enter elements of matrix: \n");*/
	
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(i==j){
				a[i][j]=i+j+1;
			}
			else{
				a[i][j]=0;
			}
		}	
	}
	for(i=0;i<3;i++){
		d+= a[0][i]*(a[1][mod(i+1,3)]*a[2][mod(i+2,3)]) - a[1][mod(i+2,3)]*a[2][mod(i+1,3)];
	}

	redefine_out_det = d;
}
