#define OIL_VALVE_OPEN_CMD				1
//#define OIL_PUMP_CMD_MAX_PULSE_WIDTH	0.94F

float OIL_PUMP_CMD_MAX_PULSE_WIDTH = 0.94F;

float redefine_in_f[2];
//float redefine_in_current_nh;
//float redefine_in_oil_valve_cmd;

int redefine_in_i;
//signed int redefine_in_startup_time;

float redefine_out_b[2];
//float redefine_out_oil_valve_cmd;
//float redefine_out_oil_pump_cmd;
/*
int asl_open_oil_valve(void) {
	return OIL_VALVE_OPEN_CMD;
}
*/

float regulation_oil(float current_nh) {
	//float current_nh = input_f[0];
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
    //return oil_pump_cmd;
}

void startup_oil(float input_f[], int input_i) {
	//static float output[2];

	if (input_i == 0) {
		/* Open OSO */
		//output[0] = OIL_VALVE_OPEN_CMD;
		//output[1] = 0;
		redefine_out_b[0] = OIL_VALVE_OPEN_CMD;
		redefine_out_b[1] = 0;
	}
	else {
		/* oil pump regulation */
		//output[0] = input_f[1];
		//output[1] = regulation_oil(input_f[0]);
		redefine_out_b[0] = input_f[1];
		redefine_out_b[1] = regulation_oil(input_f[0]);
	}
	//return output;
}

void redefine_start()
{
	startup_oil(redefine_in_f, redefine_in_i);
	//redefine_out_b[0] = output[0];
	//redefine_out_b[1] = output[1];
}
