int redefine_out_sum[5];

void redefine_start(){
	int a[5], b[5];
	int i;
	for(i = 0; i<=4 ; i++){
		redefine_out_sum[i]=a[i]*b[i];
	}
}
