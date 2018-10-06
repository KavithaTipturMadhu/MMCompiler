int redefine_out_sum[4];

void redefine_start(){
	int a[4], b[4];
	int i;
	for(i = 0; i<4 ; i++){
		redefine_out_sum[i]=a[i]*b[i];
	}
}
