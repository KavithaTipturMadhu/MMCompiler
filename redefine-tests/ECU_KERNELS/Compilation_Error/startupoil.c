#define OIL_VALVE_OPEN_CMD				1
//#define OIL_PUMP_CMD_MAX_PULSE_WIDTH	0.94F
//#define OIL_PUMP_CMD_MAX_PULSE_WIDTH	0x3F70A3D7

float OIL_PUMP_CMD_MAX_PULSE_WIDTH = 0.94F;

int redefine_in_a[3];
//float redefine_in_current_nh;
//signed int redefine_in_startup_time;
//float redefine_in_oil_valve_cmd;

//float redefine_out_oil_valve_cmd;
//float redefine_out_oil_pump_cmd;
//int redefine_out_oil_valve_cmd;
float redefine_out_b[2];

int asl_open_oil_valve(void) {
	return OIL_VALVE_OPEN_CMD;
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
    //return oil_pump_cmd;
}

float *startup_oil(int input[]) {
	static float output[2];

	if (input[1] == 0) {
		/* Open OSO */
		output[0] = asl_open_oil_valve();//OIL_VALVE_OPEN_CMD; //asl_open_oil_valve();
		output[1] = 0;
		return output;
	}
	else {
		/* oil pump regulation */
		output[0] = *((float*)&input[2]);
		output[1] = regulation_oil(input);
		return output;
	}
}

void redefine_start()
{
	float *output = startup_oil(redefine_in_a);
	redefine_out_b[0] = output[0];
	redefine_out_b[1] = output[1];
}
