/*int innerprod(int a[4], int b[4]){
	int i;
	int sum=0;
	for(i=0;i<4;i++){
		sum+=a[i]*b[i];
	}
	return sum;
}*/

int test(int *a){
	return (*a)*10;
}

//int redefine_in_test;
int redefine_out_sum;
void redefine_start(){
	//int a[4]={1,2,3,4};
	//int b[4]={1,1,1,1};
	//redefine_out_sum = innerprod(a,b);
	//int a[2]={10,10};
	int i=10;
	int *a=&i;
	redefine_out_sum = test(a);
}
