#define PROTECTION_OIL_PRESS		0x00000004
#define PROTECTION_FLAME_OUT		0x00000020
#define PROTECTION_EPU_TEMP			0x00000010
#define MODE_STOP					0x00000010
#define MAX_EPU_TEMP				116

int redefine_in_a[6];
float var = 0.7F;
//float redefine_in_oil_press, redefine_in_dtisa;
//signed int redefine_in_current_egt, redefine_in_epu_temp, redefine_in_protection_status, redefine_in_operation_mode;

int redefine_out_b[2];
//signed int redefine_out_protection_status, redefine_out_operation_mode;

int *running_protection(int input[]) {
	float oil_press = *((float*)&input[0]);
	float dtisa = *((float*)&input[1]);

	if (oil_press < var) {
	//if (input[0] < 0x3F333333) {
		input[5] = MODE_STOP;
		input[4] = PROTECTION_OIL_PRESS;
	}

	if (input[2] < (350 + 2 * dtisa)) {
		input[4] = PROTECTION_FLAME_OUT;
		input[5] = MODE_STOP;
	}

	if (input[3] > MAX_EPU_TEMP){
		input[5] = MODE_STOP;
		input[4] = PROTECTION_EPU_TEMP;
	}

	return input;
}

void redefine_start()
{
	int *output = running_protection(redefine_in_a);
	redefine_out_b[0] = output[4];
	redefine_out_b[1] = output[5];
}
