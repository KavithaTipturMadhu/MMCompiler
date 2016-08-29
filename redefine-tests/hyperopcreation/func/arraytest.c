int sum(int a[1]){
	return a[0];
}

void redefine_start(){
	int a[1]={100};
	sum(a);
}
