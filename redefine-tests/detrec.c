#define N 2
struct arr{
	int A[N];
};
typedef struct arr Sub;

int redefine_out_sum;

int detrec(Sub a, Sub b, int n){
	if(n<=4){
		int i;
		int sum=0;
		for(i=0;i<n;i++){
			sum+=a.A[i]*b.A[i];
		}
		return sum;
		
	}
	else{
		Sub tempA1, tempB1, tempA2, tempB2;
		int i;
		for(i=0;i<n/2;i++){
			tempA1.A[i]=a.A[i];
			tempB1.A[i]=b.A[i];		
			tempA2.A[i]=a.A[i+(n/2)];
			tempB2.A[i]=b.A[i+(n/2)];		
		}
		return detrec(tempA1, tempB1, n/2)+detrec(tempA2, tempB2, n/2);
	}
}

void redefine_start(){
	Sub a={1,2};
//,3,4,5,6,7,8};
	Sub b={1,1};
//,1,1,1,1,1,1};
	/*
	for(i=0;i<N;i++){
		a.A[i]=i;
		b.A[i]=1;
	}
	*/

	redefine_out_sum = detrec(a,b,N);
}
