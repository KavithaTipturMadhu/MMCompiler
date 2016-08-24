// Array reduction sum
int sumRed(int* a, int N) {
	int i;
	int sum = 0;
	//for(i=0;i<N;i++) {
	//	sum += sum + a[i];
	//}
	sum = a[0] + a[N-1];
	return sum;
}

void redefine_start() {
	int x[] = {0,1,2,4};
	int N = 4;
	int sum = sumRed(x,N);
}
