
float redefine_in_a[9];
//float redefine_in_current_nh;
//float redefine_in_dtisa;
//float redefine_in_p0;
//float redefine_in_p2;
//float redefine_in_mode;
//float redefine_in_pla;
//float redefine_in_current_nl;
//float redefine_in_integrator_nl;
//float redefine_in_fuel_pump_integrator;

float redefine_out_b[15];

float setpoint_nh(float input[]) {
	float setpoint;

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

	if (wow) {
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

#ifndef POWER_MANAGEMENT
		setpoint = nh_idle + (input[5] * (nh_top - nh_idle) / 100.0F);
#endif

	setpoint = setpoint / 100.0F * NH_REF;

	redefine_out_b[0] = nh_top;
	redefine_out_b[1]= nh_mcm;
	redefine_out_b[2]= nh_mcr;
	redefine_out_b[3]= nh_idle;

	return setpoint;
}

float setpoint_nl(float input[]) {
	float setpoint;
	float pam = input[2] * 100.0F;

	float nl_max_top;
	float nl_max_flat;
	float nl_max;
	float nl_mcm;
	float nl_mcr;
	float nl_idle;

	nl_max_top = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * __ieee754_pow(101.325F - pam, 2)  - 0.305F * input[1];
	nl_max_flat = 0.0F + 99.44F + 0.0948F * (101.325F - pam) + 0.0004F * __ieee754_pow(101.325F - pam, 2) + 0.175F * input[1];

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

	if (wow) {
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

#ifndef POWER_MANAGEMENT
	setpoint = nl_idle + (input[5] * (nl_max - nl_idle) / 100.0F);
#endif

	setpoint = setpoint / 100.0F * NL_REF;

	redefine_out_b[4] = nl_max_top;
	redefine_out_b[5] = nl_max_flat;
	redefine_out_b[6] = nl_max;
	redefine_out_b[7] = nl_mcm;
	redefine_out_b[8] = nl_mcr;
	redefine_out_b[9] = nl_idle;
	return setpoint;
}

void regulation_dl2(float input[]) {
	float fuel_pump_cmd;
	float epsilon_nl;
	float epsilon_nh;
	float output_nl;
	float eps_setpoint;
	float nh_pla_setpoint;

	float nh_pla_setpoint;
	float nl_setpoint;
	float integrator_nl;
	float inl;
	float fuel_pump_integrator;

	/* Compute the NH setpoint */
	nh_pla_setpoint = setpoint_nh(input);
	nl_setpoint = setpoint_nl(input);

	/* PID NL */
	epsilon_nl = nl_setpoint - input[6];
	integrator_nl = input[7] + epsilon_nl * 0.02 * 3e-1;
	output_nl = integrator_nl + 0.1 * epsilon_nl + nh_pla_setpoint;

	if (integrator_nl > 2000) {
		integrator_nl = 2000;
	}
	else if (integrator_nl < -2000) {
		integrator_nl = -2000;
	}
	inl = integrator_nl;

	eps_setpoint = output_nl - nh_setpoint;
	if (eps_setpoint > 0) {
		eps_setpoint *= 4.0;
	} else {
		eps_setpoint *= 0.20;
	}
	if (eps_setpoint > 4500) {
		eps_setpoint = 4500;
	}
	else if (eps_setpoint < -3000) {
		eps_setpoint = - 3000;
	}
	else {
		/* Nothing */
	}

	nh_setpoint = nh_setpoint + (eps_setpoint * 0.02);

	if (nh_setpoint > 52000) {
		nh_setpoint = 52000;
	}

	epsilon_nh = nh_setpoint - input[0];
	fuel_pump_integrator = input[8] + epsilon_nh * 6e-5 * 0.02;
	fuel_pump_cmd = fuel_pump_integrator + 3.5e-3 * 0.02 * epsilon_nh;

	if (fuel_pump_cmd > FUEL_PUMP_CMD_MAX_PULSE_WIDTH) {
		fuel_pump_cmd = FUEL_PUMP_CMD_MAX_PULSE_WIDTH;
	}

	redefine_out_b[10] = nh_pla_setpoint;
	redefine_out_b[11] = nl_setpoint;
	redefine_out_b[12] = fuel_pump_cmd;
	redefine_out_b[13] = integrator_nl;		//inl = integrator_nl, hence not returned again!!
	redefine_out_b[14] = fuel_pump_integrator;
	//return fuel_pump_cmd;
}

void redefine_start()
{
	regulation_dl2(redefine_in_a);
}
