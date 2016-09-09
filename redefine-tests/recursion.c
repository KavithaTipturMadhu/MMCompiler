int sum(int x){
	if(x>10){
		return sum(x-1);
	}
	return x;
}

int redefine_out_val;
void redefine_start(){
	redefine_out_val = sum(20);
}

