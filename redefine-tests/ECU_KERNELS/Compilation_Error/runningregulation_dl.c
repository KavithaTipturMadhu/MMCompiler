#define MODE_STARTREG						0x00000004
#define PLA_IDLE							10
#define PLA_MCR_L							73
#define PLA_MCR_H							45
#define PLA_MCM_L							73
#define PLA_MCM_H							75
#define PLA_TOP								100
#define DNHQDT_MAX_25000					10
#define DNHQDT_MIN_25000					-10
#define DNHQDT_MAX_38000					20
#define DNHQDT_MIN_38000					-20
#define DNHQDT_MAX_52000					50
#define DNHQDT_MIN_52000					-30

float NH_REF = 50719.0F, NH_IDLE_GROUND = 53.5, FUEL_PUMP_CMD_MAX_PULSE_WIDTH = 0.920F, FUEL_PUMP_CMD_MIN_PULSE_WIDTH = 0.110F;
float NL_REF = 43636.0F, NL_IDLE_GROUND = 30.0F, NL_IDLE_AIR = 47.0;
float kP_NH_25000 = 2.29e-5;
float kI_NH_25000 = 2.5e-3;
float kP_NH_35000 = 2.9e-5;
float kI_NH_35000 = 12e-3;
float EPS_NH_MIN_25000 = -1000.0;
float EPS_NH_MAX_25000 = 4000.0;
float EPS_NH_MIN_38000 = -2700.0;
float EPS_NH_MAX_38000 = 5500.0;
float EPS_NH_MIN_52000 = -5500.0;
float EPS_NH_MAX_52000 = 5500.0;

float redefine_in_a[20];
//float redefine_in_current_nh;
//float redefine_in_dtisa;
//float redefine_in_p0;
//float redefine_in_p2;
//float redefine_in_mode;
//float redefine_in_pla;
//float redefine_in_current_nl;
//float redefine_in_kp_nl;
//float redefine_in_ki_nl;
//float redefine_in_imax_nl;
//float redefine_in_previous_nh;
//float redefine_in_kdnhqdt_max;
//float redefine_in_kdnhqdt_min;
//float redefine_in_nhmax;
//float redefine_in_nlmax;
//float redefine_in_knhmax;
//float redefine_in_knlmax;
//float redefine_in_pid_nh_iTerm;
//float redefine_in_pid_nl_iTerm;
//float redefine_in_wow;

float redefine_out_b[38];
//


struct pid {
	float epsilon;
	float epsilon_modified;
	float iTerm;
	float dTerm_modified;
};

float setpoint_nh(float input[]) {
	float setpoint;

	//required to be assigned to the host code
	float nh_top;
    float nh_mcm;
    float nh_mcr;
    float nh_idle;

	if (input[1] < 0) {
		nh_top = -1.0F + input[1] * (101.7F - 94.5F) / 40.0F + 101.7F;
	}
	else {
		nh_top = -1.0F + input[1] * (100.70F - 101.70F) / 40.0F + 101.7F;
	}
	
	nh_mcm = nh_top - 2.4F;
	nh_mcr = nh_top - 3.7F;

	if (input[19]) {
		nh_idle = NH_IDLE_GROUND;
	}
	else {
		nh_idle = 70 - 2.31F * ((input[3] / input[2]) - 1);
	}

	if (input[4] == MODE_STARTREG) {
		setpoint = nh_idle / 100.0F * NH_REF;
		return setpoint;
	}

	/* PLA setpoint */
	if (input[5] <= PLA_IDLE) {
		setpoint = nh_idle;
	}
	else if (input[5] < PLA_MCR_L) {
		setpoint = nh_idle + (nh_mcr - nh_idle) * (input[5] - PLA_IDLE) / (float) (PLA_MCR_L - PLA_IDLE);
	}
	else if (input[5] <= PLA_MCR_H) {
		setpoint = nh_mcr;
	}
	else if (input[5] < PLA_MCM_L) {
		setpoint = nh_mcr + (nh_mcm - nh_mcr) * (input[5] - PLA_MCR_H) / (float) (PLA_MCM_L - PLA_MCR_H);
	}
	else if (input[5] <= PLA_MCM_H) {
		setpoint = nh_mcm;
	}
	else if (input[5] < PLA_TOP) {
		setpoint = nh_mcm + (nh_top - nh_mcm) * (input[5] - PLA_MCM_H) / (float) (PLA_TOP - PLA_MCM_H);
	}
	else {
		setpoint = nh_top;
	}
/*
#ifndef POWER_MANAGEMENT
		setpoint = nh_idle + (input[5] * (nh_top - nh_idle) / 100.0F);
#endif
*/
	setpoint = setpoint / 100.0F * NH_REF;

	redefine_out_b[0] = nh_top;
	redefine_out_b[1]= nh_mcm;
	redefine_out_b[2]= nh_mcr;
	redefine_out_b[3]= nh_idle;

	return setpoint;
}

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

float setpoint_nl(float input[]) {
	float setpoint;
	float pam = input[2] * 100.0F;

	//required to be assigned in the host code
	float nl_max_top;
	float nl_max_flat;
	float nl_max;
	float nl_mcm;
	float nl_mcr;
	float nl_idle;

	nl_max_top = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * custom_pow(101.325F - pam, 2)  - 0.305F * input[1];
	nl_max_flat = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * custom_pow(101.325F - pam, 2) + 0.175F * input[1];

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

	if (input[19]) {
		nl_idle = NL_IDLE_GROUND;
	}
	else {
		nl_idle = NL_IDLE_AIR;
	}

	if (input[5] <= PLA_IDLE) {
		setpoint = nl_idle;
	}
	else if (input[5] < PLA_MCR_L) {
		setpoint = nl_idle + (nl_mcr - nl_idle) * (input[5] - PLA_IDLE) / (float) (PLA_MCR_L - PLA_IDLE);
	}
	else if (input[5] <= PLA_MCR_H) {
		setpoint = nl_mcr;
	}
	else if (input[5] < PLA_MCM_L) {
		setpoint = nl_mcr + (nl_mcm - nl_mcr) * (input[5] - PLA_MCR_H) / (float) (PLA_MCM_L - PLA_MCR_H);
	}
	else if (input[5] <= PLA_MCM_H) {
		setpoint = nl_mcm;
	}
	else if (input[5] < PLA_TOP) {
		setpoint = nl_mcm + (nl_max - nl_mcm) * (input[5] - PLA_MCM_H) / (float) (PLA_TOP - PLA_MCM_H);
	}
	else {
		setpoint = nl_max;
	}
/*
#ifndef POWER_MANAGEMENT
	setpoint = nl_idle + (input[5] * (nl_max - nl_idle) / 100.0F);
#endif
*/
	setpoint = setpoint / 100.0F * NL_REF;

	redefine_out_b[4] = nl_max_top;
	redefine_out_b[5] = nl_max_flat;
	redefine_out_b[6] = nl_max;
	redefine_out_b[7] = nl_mcm;
	redefine_out_b[8] = nl_mcr;
	redefine_out_b[9] = nl_idle;
	return setpoint;
}

void pid_dl_update(float nh_setpoint, float nl_setpoint, float input[]) {
	//pid_nl_l = pid_nl;

	//required to be assigned in the host code
	float active_limits;
	float eps_nl;
	float eps_nh;
	float pTerm_nl;
	float iTerm_nl;
	float dTerm_nl;
	float output_nl;
	float kp_nh;
	float ki_nh;
	float epsilon_nh_min_variable;
	float epsilon_nh_max_variable;
	float eps_nh_sat;
	float dnhqdt_max;
	float dnhqdt_min;
	float dnhqdt;
	float ednhqdt;
	float eps_nh_but;
	float enhmax;
	float enlmax;
	float pTerm_nh;
	float iTerm_nh;
	float dTerm_nh;
	float output_nh;
	float output_nh_sat;
	float epsilon_nh_modified;
	float fuel_pump_cmd;

	active_limits = 0;
	/* --------------------- NL ------------------------ */
	eps_nl = nl_setpoint - input[6];

	pTerm_nl = input[7] * eps_nl;
	iTerm_nl = input[18]  + input[8] * eps_nl;
	if (iTerm_nl > input[9]) {
		iTerm_nl = input[9];
	}
	if (iTerm_nl < -input[9]) {
		iTerm_nl = -input[9];
	}
	dTerm_nl = 0; //kp_nl * kd_nl * (eps_nl - pid_nl->epsilon) - pid_nl->dTerm_modified * td_nl / (td_nl + 1);

	output_nl = pTerm_nl + iTerm_nl; // + dTerm_nl;

	/* ---------------------- NH ----------------------- */
	eps_nh = output_nl + nh_setpoint - input[0];

	/* Recalcul ki et kp de la regul NH */
	if (input[0] < 25000.0F) {
		kp_nh = kP_NH_25000;
		ki_nh = kI_NH_25000;
	} else {
		kp_nh = kP_NH_25000
				+ (kP_NH_35000 - kP_NH_25000) * (input[0] - 25000.0F)
						/ (35000.0F - 25000.0F);
		ki_nh = kI_NH_25000
				+ (kI_NH_35000 - kI_NH_25000) * (input[0] - 25000.0F)
						/ (35000.0F - 25000.0F);
	}
	/* FIN recalcul ki et kp de la regul NH */

	/* Test avec butee epsilon de la regul NH */
	if (input[0] < 25000.0F) {
		epsilon_nh_min_variable = EPS_NH_MIN_25000;
		epsilon_nh_max_variable = EPS_NH_MAX_25000;
	} else if (input[0] < 38000.0F) {
		epsilon_nh_min_variable = EPS_NH_MIN_25000
				+ (EPS_NH_MIN_38000 - EPS_NH_MIN_25000)
						* ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
		epsilon_nh_max_variable = EPS_NH_MAX_25000
				+ (EPS_NH_MAX_38000 - EPS_NH_MAX_25000)
						* ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
	} else if (input[0] < 52000.0F) {
		epsilon_nh_min_variable = EPS_NH_MIN_38000
				+ (EPS_NH_MIN_52000 - EPS_NH_MIN_38000)
						* ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
		epsilon_nh_max_variable = EPS_NH_MAX_38000
				+ (EPS_NH_MAX_52000 - EPS_NH_MAX_38000)
						* ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
	} else {
		epsilon_nh_min_variable = EPS_NH_MIN_52000;
		epsilon_nh_max_variable = EPS_NH_MAX_52000;
	}

	if (eps_nh < epsilon_nh_min_variable) {
		eps_nh_sat = epsilon_nh_min_variable;
	} else if (eps_nh > epsilon_nh_max_variable) {
		eps_nh_sat = epsilon_nh_max_variable;
	} else {
		eps_nh_sat = eps_nh;
	}
	/* FIN Test avec butee epsilon de la regul NH */

	/* Test avec recalcul dnhqdt et kdnhqdt */
	if (input[0] < 25000.0F) {
		dnhqdt_max = DNHQDT_MAX_25000;
		dnhqdt_min = DNHQDT_MIN_25000;
//		kdnhqdt_max = KDNHQDT_MAX_25000;
//		kdnhqdt_min = KDNHQDT_MIN_25000;
	}
	else if (input[0] < 38000.0F) {
		dnhqdt_max = DNHQDT_MAX_25000 + (DNHQDT_MAX_38000 - DNHQDT_MAX_25000) * ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
		dnhqdt_min = DNHQDT_MIN_25000 + (DNHQDT_MIN_38000 - DNHQDT_MIN_25000) * ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
//		kdnhqdt_max = DNHQDT_MAX_25000 + (KDNHQDT_MAX_38000 - KDNHQDT_MAX_25000) * ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
//		kdnhqdt_min = DNHQDT_MIN_25000 + (KDNHQDT_MIN_38000 - KDNHQDT_MIN_25000) * ((input[0] - 25000.0F) / (38000.0F - 25000.0F));
	}
	else if (input[0] < 52000.0F) {
		dnhqdt_max = DNHQDT_MAX_38000 + (DNHQDT_MAX_52000 - DNHQDT_MAX_38000) * ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
		dnhqdt_min = DNHQDT_MIN_38000 + (DNHQDT_MIN_52000 - DNHQDT_MIN_38000) * ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
//		kdnhqdt_max = DNHQDT_MAX_38000 + (KDNHQDT_MAX_52000 - KDNHQDT_MAX_38000) * ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
//		kdnhqdt_min = DNHQDT_MIN_38000 + (KDNHQDT_MIN_52000 - KDNHQDT_MIN_38000) * ((input[0] - 38000.0F) / (52000.0F - 38000.0F));
	}
	else {
		dnhqdt_max = DNHQDT_MAX_52000;
		dnhqdt_min = DNHQDT_MIN_52000;
//		kdnhqdt_max = DNHQDT_MAX_52000;
//		kdnhqdt_min = DNHQDT_MIN_52000;
	}
	/* FIN test avec reclacul ki kp */

	/* Butee dNH/dt */
	dnhqdt = input[0] - input[10];
	if ((dnhqdt > dnhqdt_max) && (eps_nh_sat > 0)) {
		ednhqdt = (dnhqdt - dnhqdt_max) * input[11];
		eps_nh_but = eps_nh_sat - ednhqdt;
	} else if ((dnhqdt < dnhqdt_min) && (eps_nh_sat < 0)) {
		ednhqdt = (dnhqdt - dnhqdt_min) * input[12];
		eps_nh_but = eps_nh_sat - ednhqdt;
	} else {
		ednhqdt = 0;
		eps_nh_but = eps_nh_sat;
	}

	/* Butee NHmax */
	if (input[0] > input[13]) {
		enhmax = (input[0] - input[13]) * input[15];
		eps_nh_but = eps_nh_but - enhmax;
	} else {
		enhmax = 0;
	}

	/* Butee NLmax */
	if (input[6] > input[14]) {
		enlmax = (input[6] - input[14]) * input[16];
		eps_nh_but = eps_nh_but - enlmax;
	} else {
		enlmax = 0;
	}

	/* Correction */
	pTerm_nh = kp_nh * eps_nh_but;
	iTerm_nh = input[17]  + (ki_nh * kp_nh * eps_nh_but);
	dTerm_nh = 0; //kp_nh * kd_nh * (eps_nh_but - pid_nh->epsilon) - pid_nh->dTerm_modified * td_nh / (td_nh + 1);

	output_nh = pTerm_nh + iTerm_nh + dTerm_nh;

//	output_nh = limit_fps_dnhqdt(output_nh);

	/* Output limitation */
	if (output_nh < FUEL_PUMP_CMD_MIN_PULSE_WIDTH) {
		output_nh_sat = FUEL_PUMP_CMD_MIN_PULSE_WIDTH;
	} else if (output_nh > FUEL_PUMP_CMD_MAX_PULSE_WIDTH) {
		output_nh_sat = FUEL_PUMP_CMD_MAX_PULSE_WIDTH;
	} else {
		output_nh_sat = output_nh;
	}

	/* --------------- Anti Wind-Up NH ----------------- */
	epsilon_nh_modified = output_nh_sat
//			+ pid_nh->epsilon_modified * kp_nh * kd_nh / (td_nh+ 1.0F)
//			+ pid_nh->dTerm_modified * td_nh / (td_nh + 1.0F)
			- input[17] ;
//	epsilon_nh_modified /= (kp_nh * (1.0F + ki_nh + kd_nh / (td_nh + 1.0F)));
	epsilon_nh_modified /= (kp_nh * (1.0F + ki_nh)); //

	iTerm_nh = input[17]  + (kp_nh * ki_nh * epsilon_nh_modified);

	/* --------------- Anti Wind-Up NL ----------------- */
//	output_nl = epsilon_nh_modified - nh_setpoint + input[0];
//	if (output_nl < -20000) {
//		asm ("nop");
//	}
//	eps_nl_modified = output_nl	- pid_nl->iTerm;
//	eps_nl_modified /= (kp_nl + ki_nl);
//	iTerm_nl = pid_nl->iTerm + (ki_nl * eps_nl_modified);
//	output_nl = epsilon_nh_modified - nh_setpoint + input[0];
//
//	eps_nl_modified = output_nl
////			+ pid_nl->epsilon_modified * kp_nl * kd_nl / (td_nl + 1.0F)
////			+ pid_nl->dTerm_modified * td_nl / (td_nl + 1.0F)
//			- pid_nl->iTerm;
////	eps_nl_modified /= (kp_nl * (1.0F + ki_nl + kd_nl / (td_nl + 1.0F)));
//	eps_nl_modified /= (kp_nl + ki_nl);
//
//	iTerm_nl = pid_nl->iTerm + (kp_nl * ki_nl * eps_nl_modified);
	/* --------------- PID Structs update -------------- */
	/*
	pid_nl->epsilon = eps_nl;
	pid_nl->epsilon_modified = eps_nl_modified;
	pid_nl->iTerm = iTerm_nl;
	pid_nl->dTerm_modified = dTerm_nl;

	pid_nh->epsilon = eps_nh_but;
	pid_nh->epsilon_modified = epsilon_nh_modified;
	pid_nh->iTerm = iTerm_nh;
	pid_nh->dTerm_modified = dTerm_nh;
	*/
	fuel_pump_cmd = output_nh_sat;

	/* Coef tuning */
//	tuning();
	redefine_out_b[10] = active_limits;
	redefine_out_b[11] = eps_nl;		//pid_nl->epsilon = eps_nl;
	redefine_out_b[12] = eps_nh;
	redefine_out_b[13] = pTerm_nl;
	redefine_out_b[14] = iTerm_nl;		//pid_nl->iTerm = iTerm_nl;
	redefine_out_b[15] = dTerm_nl;		//pid_nl->dTerm_modified = dTerm_nl;
	redefine_out_b[16] = output_nl;
	redefine_out_b[17] = kp_nh;
	redefine_out_b[18] = ki_nh;
	redefine_out_b[19] = epsilon_nh_min_variable;
	redefine_out_b[20] = epsilon_nh_max_variable;
	redefine_out_b[21] = eps_nh_sat;
	redefine_out_b[22] = dnhqdt_max;
	redefine_out_b[23] = dnhqdt_min;
	redefine_out_b[24] = dnhqdt;
	redefine_out_b[25] = ednhqdt;
	redefine_out_b[26] = eps_nh_but;	//pid_nh->epsilon = eps_nh_but;
	redefine_out_b[27] = enhmax;
	redefine_out_b[28] = enlmax;
	redefine_out_b[29] = pTerm_nh;
	redefine_out_b[30] = iTerm_nh;		//pid_nh->iTerm = iTerm_nh;
	redefine_out_b[31] = dTerm_nh;		//pid_nh->dTerm_modified = dTerm_nh;
	redefine_out_b[32] = output_nh;
	redefine_out_b[33] = output_nh_sat;
	redefine_out_b[34] = epsilon_nh_modified;	//pid_nh->epsilon_modified = epsilon_nh_modified;
	redefine_out_b[35] = fuel_pump_cmd;

	//*********pid_nh->epsilon_modified = eps_nl_modified should be assigned in the host code**************
	//return output_nh_sat;
}

void regulation_dl(float input[]) {
	//float fuel_pump_setpoint;

	/* compute NH and NL setpoints */
	float nh_setpoint = setpoint_nh(input);
	float nl_setpoint = setpoint_nl(input);

	redefine_out_b[36] = nh_setpoint;
	redefine_out_b[37] = nl_setpoint;

	/* compute the fuel pump speed setpoint */
	//fuel_pump_setpoint = pid_dl_update(nh_setpoint, nl_setpoint, input);
	pid_dl_update(nh_setpoint, nl_setpoint, input);

	//return fuel_pump_setpoint;
}

void redefine_start()
{
	regulation_dl(redefine_in_a);
}
