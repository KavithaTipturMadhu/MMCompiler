unsigned int fib(unsigned int n){
	if(n>1){
		return fib(n-1)+fib(n-2);
	}
	return n;
}

void main(){
	int n = 20, m;
	m=fib(n);
}
