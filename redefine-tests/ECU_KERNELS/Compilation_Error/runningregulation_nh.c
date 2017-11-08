//#define FUEL_PUMP_CMD_MAX_PULSE_WIDTH		0.920F
//#define NH_IDLE_GROUND						53.5
#define MODE_STARTREG						0x00000004
#define PLA_IDLE							10
#define PLA_MCR_L							73
#define PLA_MCR_H							45
#define PLA_MCM_L							73
#define PLA_MCM_H							75
#define PLA_TOP								100
//#define NH_REF								50719.0F

float NH_REF = 50719.0F, NH_IDLE_GROUND = 53.5, FUEL_PUMP_CMD_MAX_PULSE_WIDTH = 0.920F;

float redefine_in_a[10];
//float redefine_in_current_nh; 
//float redefine_in_nh_setpoint; 
//float redefine_in_ramp_filter_memory;
//float redefine_in_fuel_pump_integrator;
//float redefine_in_dtisa;
//float redefine_in_wow;
//float redefine_in_pla;
//float redefine_in_p0;
//float redefine_in_p2;
//float redefine_in_mode;

float redefine_out_b[9];
//float redefine_out_fuel_pump_cmd;
//float redefine_out_ramp_filter_memory;
//float redefine_out_fuel_pump_integrator;
//float redefine_out_nh_top;
//float redefine_out_nh_mcm;
//float redefine_out_nh_mcr;
//float redefine_out_nh_idle;
//float redefine_out_nh_setpoint;
//float redefine_out_nl_setpoint;

/*
float div(float num, float den) {
	int int_num = *((int*)&num);
	int int_den = *((int*)&den);
	int bias = 0x7F;
	int zero = 0;
	int one = 1;
	int two = 2;
	int sign_bit = 0; //(num >> 31) ^ (den >> 31); Sometimes doesn't work but works in host system
	int num_exponent = (int_num & 0x7F800000) >> 23;
	int den_exponent = (int_den & 0x7F800000) >> 23;
	int num_mantissa = (int_num & 0x007FFFFF) + 0x00800000;
	int den_mantissa = (int_den & 0x007FFFFF) + 0x00800000;
	int result_exponent = num_exponent - den_exponent + bias;
	int mantissa_quotient = 0x00000000;
	int increment_bit = 0x00800000;
	int result;
	
	if((int_num & 0x80000000) == one) {
		if((int_den & 0x80000000) == zero)
			sign_bit = one;
	} else {
		if((int_den & 0x80000000) == one)
			sign_bit = one;
	}

	if(num_mantissa < den_mantissa) {
		num_mantissa = num_mantissa * two;			//Problem is here, can't use immediate values
		result_exponent = result_exponent - one;
	}

	while(increment_bit > zero) {

		if(num_mantissa >= den_mantissa) {
			num_mantissa = num_mantissa - den_mantissa;
			mantissa_quotient = mantissa_quotient + increment_bit;
		} else if(num_mantissa == zero)
			break;
		
		num_mantissa = num_mantissa << one;
		increment_bit = increment_bit >> one;
	}

	result = (sign_bit << 31) + (result_exponent << 23) + (mantissa_quotient & 0x007FFFFF);

	return *((float*)&result);
}
*/

float setpoint_nh(float input[]) {
	float setpoint;
    float nh_top;
    float nh_mcm;
    float nh_mcr;
    float nh_idle;

	if (input[4] < 0) {
		nh_top = -1.0F + input[4] * (101.7F - 94.5F) / 40.0F + 101.7F;
	}
	else {
		nh_top = -1.0F + input[4] * (100.70F - 101.70F) / 40.0F + 101.7F;
	}
	
	nh_mcm = nh_top - 2.4F;
	nh_mcr = nh_top - 3.7F;

	if (input[5]) {
		nh_idle = NH_IDLE_GROUND;
	}
	else {
		nh_idle = 70 - 2.31F * (input[8]/input[7] - 1);
	}

	if (input[9] == MODE_STARTREG) {
		setpoint = (nh_idle / 100.0F) * NH_REF;
		return setpoint;
	}

	/* PLA setpoint */
	if (input[6] <= PLA_IDLE) {
		setpoint = nh_idle;
	}
	else if (input[6] < PLA_MCR_L) {
		setpoint = nh_idle + (nh_mcr - nh_idle) * (input[6] - PLA_IDLE) / (float) (PLA_MCR_L - PLA_IDLE);
	}
	else if (input[6] <= PLA_MCR_H) {
		setpoint = nh_mcr;
	}
	else if (input[6] < PLA_MCM_L) {
		setpoint = nh_mcr + (nh_mcm - nh_mcr) * (input[6] - PLA_MCR_H) / (float) (PLA_MCM_L - PLA_MCR_H);
	}
	else if (input[6] <= PLA_MCM_H) {
		setpoint = nh_mcm;
	}
	else if (input[6] < PLA_TOP) {
		setpoint = nh_mcm + (nh_top - nh_mcm) * (input[6] - PLA_MCM_H) / (float) (PLA_TOP - PLA_MCM_H);
	}
	else {
		setpoint = nh_top;
	}
/*
#ifndef POWER_MANAGEMENT
		setpoint = nh_idle + (input[6] * (nh_top - nh_idle) / 100.0F);
#endif
*/
	setpoint = (setpoint / 100.0F) * NH_REF;
	redefine_out_b[3] = nh_top;
	redefine_out_b[4]= nh_mcm;
	redefine_out_b[5]= nh_mcr;
	redefine_out_b[6]= nh_idle;
	return setpoint;
}

//float redefine_in_current_nh; 
//float redefine_in_nh_setpoint; 
//float redefine_in_ramp_filter_memory;
//float redefine_in_fuel_pump_integrator;
//float redefine_in_dtisa;
//float redefine_in_wow;
//float redefine_in_pla;
//float redefine_in_p0;
//float redefine_in_p2;
//float redefine_in_mode;
float regulation_nh(float input[]) {//float current_nh, float *nh_setpoint, float *ramp_filter_memory, float *fuel_pump_integrator, float dtisa, unsigned char wow, signed int pla, float p0, float p2, int mode) {
	float fuel_pump_cmd;
	float eps_setpoint;
	float epsilon;
	float pla_setpoint = 0.0;
	float eps_ramp_filter;
	float sat_eps_up;
	float sat_eps_down;
	float nl_setpoint;

	/* Compute the NH setpoint */
	pla_setpoint = setpoint_nh(input);
	nl_setpoint = 0.0F;
/*
#ifdef BRIDAGE
	if (pla_setpoint > NH_BRIDAGE) {
		pla_setpoint = NH_BRIDAGE;
	}
#endif
*/
	sat_eps_up = 4500 + (input[0] - 27000) / (float) (52000 - 27000) * 1500;
	sat_eps_down = -3000 - (input[0] - 27000) / (float) (52000 - 27000) * 1500;

	eps_setpoint = pla_setpoint - input[1];
	if (eps_setpoint > 0) {
		eps_setpoint *= 4.0;
	} else {
		eps_setpoint *= 4.0;
	}
	if (eps_setpoint > sat_eps_up) {
		eps_setpoint = sat_eps_up;
	}
	else if (eps_setpoint < sat_eps_down) {
		eps_setpoint = sat_eps_down;
	}
	else {
		/* Nothing */
	}

	input[1] = input[1] + (eps_setpoint * 0.02);

	eps_ramp_filter = 3 * (input[1] - input[2]);
	input[2] = input[2] + eps_ramp_filter * 0.02;

	epsilon = input[2] - input[0];
	input[3] = input[3] + epsilon * 6e-5 * 0.02;
	fuel_pump_cmd = input[3] + 7e-5 * epsilon;

	if (fuel_pump_cmd > FUEL_PUMP_CMD_MAX_PULSE_WIDTH) {
		fuel_pump_cmd = FUEL_PUMP_CMD_MAX_PULSE_WIDTH;
	}

	redefine_out_b[8] = nl_setpoint;
    return fuel_pump_cmd;
}

void redefine_start()
{
	redefine_out_b[0] = regulation_nh(redefine_in_a);
	redefine_out_b[1] = redefine_in_a[2];
	redefine_out_b[2] = redefine_in_a[3];
	redefine_out_b[7] = redefine_in_a[1];
}
