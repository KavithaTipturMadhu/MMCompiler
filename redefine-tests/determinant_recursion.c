#define N 4
int redefine_out_d;

struct submatrix{
	int A[N][N];
};

typedef struct submatrix Sub;


void CoFactor(Sub M, Sub* b, int c, int n){
	int i=0,j=0;
	int r,col;

	for(int r=1;r<n;r++){
		for(int col=0;col<n;col++){
			if(col!=c){
				b->A[i][j++] = M.A[r][col];
				if(j == n-1) //row is filled
				{
					j = 0;
					i++;
				}
			}
		}
	}
}

int computeSign(int n){
	if(n%2==0){
		return 1;
	}
	return -1;
}

int det(Sub M,int n){
	int s[N];
	int i;
	int sum=0;
	if(n == 1){
		return M.A[0][0];
	}

	for(i=0;i<n;i++){
		Sub temp;
		CoFactor(M,&temp,i,n);
		s[i] = computeSign(i)*M.A[0][i]*det(temp,n-1);
	}

	for(i=0;i<n;i++){
		sum+=s[i];
	}
	return sum;
}

int main(){
	Sub array;
	int i;
	int j;
	for(i=0; i<N; i++)
		for(j=0; j<N; j++)
			{
				array.A[i][j]=(i==j);
			}

	redefine_out_d = det(array,N);

	return redefine_out_d;
}


