int factorial(int x){
	if(x>1){
		return x*factorial(x-1);
	}
	return 1;
}

int redefine_out_val;
void redefine_start(){
	redefine_out_val = factorial(20);
}

