#define	ARRAY_SIZE	25
float redefine_in_array[ARRAY_SIZE];
float redefine_out_parameter;
	
float po_calculation(float* parray)
{
	int i,p0;
	for(i=0, p0=0; i<ARRAY_SIZE; i++){
		p0 += parray[i];
	}
	p0 = p0*ARRAY_SIZE;
	return p0;
}

void redefine_start()
{
	redefine_out_parameter = po_calculation(redefine_in_array);
}



	
