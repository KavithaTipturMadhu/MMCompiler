// Summation over the elements of the array
int sumReduce(int a[], int N, int i){
	int sum = a[i] + a[N-1-i];
	return sum;
}


void redefine_start(){
	int b[] = {0,1,2,4};
	int c = sumReduce(b,4,0);
}

