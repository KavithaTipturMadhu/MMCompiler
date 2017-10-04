#define ARRAY_SIZE 25
//int redefine_in_array[ARRAY_SIZE];
//int redefine_in_array_size;
int redefine_out_array[ARRAY_SIZE];


int data_init()
{
	return 0;
}

void redefine_start()
{
	int i;
	for(i = 0; i < ARRAY_SIZE; i++)
	{
		redefine_out_array[i] = data_init();
	}

}


