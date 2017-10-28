#define PROTECTION_OIL_PRESS		0x00000004
#define PROTECTION_FLAME_OUT		0x00000020
#define PROTECTION_EPU_TEMP			0x00000010
#define MODE_STOP					0x00000010
#define MAX_EPU_TEMP				116

int redefine_in_a[7];
float var = 0.7F;
//float redefine_in_current_nh;
//float redefine_in_oil_press, redefine_in_dtisa;
//signed int redefine_in_current_egt, redefine_in_epu_temp, redefine_in_protection_status, redefine_in_operation_mode;

int redefine_out_b[3];
//signed int redefine_out_protection_status, redefine_out_operation_mode;

int *running_protection(int input[]) {
	float oil_press = *((float*)&input[1]);
	float dtisa = *((float*)&input[2]);

	if (oil_press < var) {
		input[6] = MODE_STOP;
		input[5] = PROTECTION_OIL_PRESS;
	}

	if (input[3] < (350 + 2 * dtisa)) {
		input[5] = PROTECTION_FLAME_OUT;
		input[6] = MODE_STOP;
	}

	if (input[4] > MAX_EPU_TEMP){
		input[6] = MODE_STOP;
		input[5] = PROTECTION_EPU_TEMP;
	}

	return input;
}

float regulation_oil(int input[]) {
	float current_nh = *((float*)&input[0]);
	float oil_pump_cmd = 0;	
	float var1 = 1.82e-14;
	float var2 = 1.46e-09;
	float var3 = 4.24e-05;
	float var4 = 0.0462;
	float var5 = 1.08;

	if (current_nh > 3500) {
		//oil_pump_cmd = (1.82e-14 * current_nh * current_nh * current_nh
		//				- 1.46e-09 * current_nh * current_nh + 4.24e-05 * current_nh
		//				+ 0.0462)*1.08;
		
		oil_pump_cmd = (var1 * current_nh * current_nh
					  - var2 * current_nh * current_nh + var3 * current_nh
                      + var4) * var5;

		if (oil_pump_cmd < 3) {
			oil_pump_cmd = 3;
		}
		else if (oil_pump_cmd > OIL_PUMP_CMD_MAX_PULSE_WIDTH) {
			oil_pump_cmd = OIL_PUMP_CMD_MAX_PULSE_WIDTH;
		}
	}

	return oil_pump_cmd;
}

void redefine_start()
{
	int *output = running_protection(redefine_in_a);
	
	redefine_out_b[0] = output[4];
	redefine_out_b[1] = output[5];
	redefine_out_b[2] = regulation_oil(redefine_in_a);
}
