#define ARRAY_SIZE 25
#define ARRAY_TEST_SIZE 4
float redefine_out_array[ARRAY_SIZE];
//float redefine_out_array;
int redefine_in_placeholder;

float data_init(int x)
{
	return *((float*)&x);
}

void redefine_start()
{
	int i;
	//int Array_test[4];
	int Array_test;
	Array_test = redefine_in_placeholder;
	/*Array_test[1] = 0x00000000;
	Array_test[2] = 0x3F8CCCCD;
	Array_test[3] = 0x400CCCCD;
	*/
	
	for(i = 0; i < ARRAY_SIZE; i ++)
	{	
		redefine_out_array[i] = data_init(Array_test);//data_init(Array_test[i]);
	}
	
	//redefine_out_array[0] =  *((float*)&Array_test[0]);

}
