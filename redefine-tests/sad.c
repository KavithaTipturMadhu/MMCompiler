#define N 4
int redefine_out_sad;
int redefine_in_a[4];
int redefine_in_b[4];
void redefine_start()
{
	int i,temp;
	int sad = 0;
	/*
	printf("Enter values in first matrix: \n");
	for(i=0;i<3;i++){
		scanf("%d",&a[i]);}

	printf("Enter values in second matrix: \n");
	for(i=0;i<3;i++){
		scanf("%d",&b[i]);}

	
	for(i=1;i<=3;i++)
	{
		sad+=abs(a[i]-b[i]);
	}
	*/

	for(i=0;i<N;i++)
	{
		if(redefine_in_a[i]>redefine_in_b[i]){
			temp = redefine_in_a[i]-redefine_in_b[i];			
		}else{
			temp = redefine_in_b[i]-redefine_in_a[i];		
		}
		sad+=temp;
	}
	redefine_out_sad = sad;
}
