struct mytest{
	int x,y;
};
struct mytest a[10], b[10];
void main(){
	int i;
	for(i=0;i<10;i++){
		a[i].x=i;
		a[i].y =i+10;
		b[i].x=i+1;
		b[i].y=i*18;
	}
}

