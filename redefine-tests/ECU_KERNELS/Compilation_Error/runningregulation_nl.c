#define MODE_STARTREG						0x00000004
#define PLA_IDLE							10
#define PLA_MCR_L							73
#define PLA_MCR_H							45
#define PLA_MCM_L							73
#define PLA_MCM_H							75
#define PLA_TOP								100

float NL_REF = 43636.0F, NL_IDLE_GROUND = 30.0F, NL_IDLE_AIR = 47.0, FUEL_PUMP_CMD_MAX_PULSE_WIDTH = 0.920F;
float redefine_in_a[9];
//float redefine_in_current_nl; 
//float redefine_in_nl_setpoint; 
//float redefine_in_ramp_filter_memory;
//float redefine_in_fuel_pump_integrator;
//float redefine_in_dtisa;
//float redefine_in_wow;
//float redefine_in_pla;
//float redefine_in_p0;
//float redefine_in_mode;

float redefine_out_b[11];
//float redefine_out_fuel_pump_cmd;
//float redefine_out_ramp_filter_memory;
//float redefine_out_fuel_pump_integrator;
//float redefine_out_nl_max_top;
//float redefine_out_nl_max_flat;
//float redefine_out_nl_max;
//float redefine_out_nl_mcm;
//float redefine_out_nl_mcr;
//float redefine_out_nl_idle;
//float redefine_out_nl_setpoint;
//float redefine_out_nh_setpoint;
float custom_pow(float base, float exponent)
{
	int i;
	float result = 1;
	if (exponent == 0)
		return result;
	for (i = 0; i < exponent; i++) {
		result = result * base;
	}
	return result;
}

float setpoint_nl(float input []) {
	float setpoint;
	float pam = input[7] * 100.0F;

	float nl_max_top;
	float nl_max_flat;
	float nl_max;
	float nl_mcm;
	float nl_mcr;
	float nl_idle;

	nl_max_top = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * custom_pow(101.325F - pam, 2)  - 0.305F * input[4];
	nl_max_flat = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * custom_pow(101.325F - pam, 2) + 0.175F * input[4];

	if (nl_max_top > nl_max_flat) {
		nl_max = nl_max_flat;
		nl_mcm = nl_max_flat - 5.0F;
		nl_mcr = nl_max_flat - 7.5F;
	}
	else {
		nl_max = nl_max_top;
		nl_mcm = nl_max_top - 5.0F;
		nl_mcr = nl_max_top - 7.5F;
	}
	if (nl_max > 105.0F) {
		nl_max = 105.0F;
	}
	if (nl_mcm > 105.0F) {
		nl_mcm = 105.0F;
	}

	if (input[5]) {
		nl_idle = NL_IDLE_GROUND;
	}
	else {
		nl_idle = NL_IDLE_AIR;
	}

	if (input[6] <= PLA_IDLE) {
		setpoint = nl_idle;
	}
	else if (input[6] < PLA_MCR_L) {
		setpoint = nl_idle + (nl_mcr - nl_idle) * (input[6] - PLA_IDLE) / (float) (PLA_MCR_L - PLA_IDLE);
	}
	else if (input[6] <= PLA_MCR_H) {
		setpoint = nl_mcr;
	}
	else if (input[6] < PLA_MCM_L) {
		setpoint = nl_mcr + (nl_mcm - nl_mcr) * (input[6] - PLA_MCR_H) / (float) (PLA_MCM_L - PLA_MCR_H);
	}
	else if (input[6] <= PLA_MCM_H) {
		setpoint = nl_mcm;
	}
	else if (input[6] < PLA_TOP) {
		setpoint = nl_mcm + (nl_max - nl_mcm) * (input[6] - PLA_MCM_H) / (float) (PLA_TOP - PLA_MCM_H);
	}
	else {
		setpoint = nl_max;
	}
/*
#ifndef POWER_MANAGEMENT
	setpoint = nl_idle + (input[6] * (nl_max - nl_idle) / 100.0F);
#endif
*/
	setpoint = setpoint / 100.0F * NL_REF;
	
	redefine_out_b[3] = nl_max_top;
	redefine_out_b[4] = nl_max_flat;
	redefine_out_b[5] = nl_max;
	redefine_out_b[6] = nl_mcm;
	redefine_out_b[7] = nl_mcr;
	redefine_out_b[8] = nl_idle;
	return setpoint;
}

//float redefine_in_current_nl; 
//float redefine_in_nl_setpoint; 
//float redefine_in_ramp_filter_memory;
//float redefine_in_fuel_pump_integrator;
//float redefine_in_dtisa;
//float redefine_in_wow;
//float redefine_in_pla;
//float redefine_in_p0;
//float redefine_in_p2;
//float redefine_in_mode;

//float redefine_out_fuel_pump_cmd;
//float redefine_out_ramp_filter_memory;
//float redefine_out_fuel_pump_integrator;
//float redefine_out_nl_max_top;
//float redefine_out_nl_max_flat;
//float redefine_out_nl_max;
//float redefine_out_nl_mcm;
//float redefine_out_nl_mcr;
//float redefine_out_nl_idle;
//float redefine_out_nl_setpoint;
//float redefine_out_nh_setpoint;
float regulation_nl(float input[]) {
	float fuel_pump_cmd;
	float eps_setpoint;
	float epsilon;
	float pla_setpoint = 0.0;
	float EpsilonFiltreRampe;
	float sat_eps_up;
	float sat_eps_down;
	float nh_setpoint;

	/* Compute the NL setpoint */
	nh_setpoint = 0.0;
	pla_setpoint = setpoint_nl(input);
/*
#ifdef BRIDAGE
	if (pla_setpoint > NL_BRIDAGE) {
		pla_setpoint = NL_BRIDAGE;
	}
#endif
*/
	sat_eps_up = 4000 + (input[0] - 14000) / (float) (44000 - 14000) * 2600;
	sat_eps_down = -3200 - (input[0] - 14000) / (float) (44000 - 14000) * 2000;

	eps_setpoint = pla_setpoint - input[1];
	if (eps_setpoint > 0) {
		eps_setpoint *= 4.0;
	} else {
		eps_setpoint *= 0.5;
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

	EpsilonFiltreRampe = 6 * (input[1] - input[2]);
	input[2] = input[2] + EpsilonFiltreRampe * 0.02;

	epsilon = input[2] - input[0];
	input[3] = input[3] + epsilon * 9e-5 * 0.02;
	fuel_pump_cmd = input[3] + 1e-4 * epsilon;

	if (fuel_pump_cmd > FUEL_PUMP_CMD_MAX_PULSE_WIDTH) {
		fuel_pump_cmd = FUEL_PUMP_CMD_MAX_PULSE_WIDTH;
	}

	redefine_out_b[10] = nh_setpoint;
	return fuel_pump_cmd;
}

void redefine_start()
{
	redefine_out_b[0] = regulation_nl(redefine_in_a);
	redefine_out_b[1] = redefine_in_a[2];
	redefine_out_b[2] = redefine_in_a[3];
	redefine_out_b[9] = redefine_in_a[1];
}
