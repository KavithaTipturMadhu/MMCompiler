int a[4][4], b[4][4], c[4][4];
void main(){
	int i, j, k;
	for(k=0;k<4;k++){
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				c[i][j]+=a[i][k]*b[k][j];
			}
		}
	}
}
