int fib(int x){
	if(x>1){
		return fib(x-1)+fib(x-2);
	}else{
		return x;
	}
}

int redefine_out_val;
void redefine_start(){
	redefine_out_val = fib(4);
}

