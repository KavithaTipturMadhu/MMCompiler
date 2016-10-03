int factorial(int x){
	int retVal = 0;
	if(x>1){
		retVal =  x*factorial(x-1);
	}else{
		retVal= 1;
	}
	return retVal;
}

int redefine_out_val;
void redefine_start(){
	redefine_out_val = factorial(1);
}

