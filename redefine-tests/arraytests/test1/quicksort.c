void test(){
	int a[6] = {25,10};
	int i,j;
	for(i=0;i<1;i++){
		int min = -1, temp, index;
		for(j=i+1;j<2;j++){
			if(a[j]<min||min==-1){
				min=a[j];
				index = j;
			}
		}
		if(a[i]>min){
			//swap 
			temp=a[i];
			a[i]=min;
			a[index]=temp;
		}
	}
}

		

