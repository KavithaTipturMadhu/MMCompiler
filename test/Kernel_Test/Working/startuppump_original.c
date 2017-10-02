#define NH_FUEL_RAMP_START 	2700
#define NH_FUEL_RAMP_END 	15000
#define NH_REGUL_START 		15000
#define ARRAY_SIZE   		25
#define PLA_IDLE		10
#define STAB_ARRAY_SIZE		50

#define MODE_IDLE		0x00000001
#define MODE_STARTUP		0x00000002
#define MODE_STARTREG		0x00000004
#define	MODE_RUNNING		0x00000008
#define	MODE_STOP		0x00000010
#define	MODE_VENTILATION	0x00000020
#define	MODE_OILPURGE		0x00000100


float CMD_FUEL_RAMP_START = 0.14;
float CMD_FUEL_RAMP_END = 0.14;
float NH_IDLE_GROUND = 53.5;
float NL_IDLE_GROUND = 30.0;
float zero_point_zero_zero_one_four = 0.0014;
float zero_point_zero_zero_one = 0.001;
float one_point_o = 1.0;
float zero_point_one_eight = 0.18;
float NL_REF = 43636.0F;
float NH_REF = 50719.0F;
float hundred = 100.0F;
float zero_point_zero_two = 0.02;
float zero_point_zero_two_five = 0.025;
float zero_point_nine_nine_five = 0.995;
float zero_point_one = 0.1;
float zero_point_zero_one = 0.01;
float nh_setpoint;
float nh_idle;
float exp1 = 2e-7;
float exp3 = 5e-5;
int ten = 10;
int fifty = 50;
int thirty = 30;
int twenty = 20;
int two = 2;
int t_fuel_press_control;
float fuel_press_setpoint;
float fuel_pump_integrator;
float fuel_pump_cmd;
float ramp_filter_memory;
float epsilon_global;
float iTerm;
float epsilon_modified;
float dTerm_modified;
int operation_mode;
int stab_array_index;
float nh_average;

int redefine_in_stab[STAB_ARRAY_SIZE];
int redefine_in_parameters[16];
int redefine_out_parameters[12];


void pid_init(float iTermtemp) {
	epsilon_global = 0.0F;				//output[2]
	iTerm = iTermtemp;				//output[3]
	epsilon_modified = 0.0F;			//output[4]
	dTerm_modified = 0.0F;				//output[5]
}

void regulation_nh_init(float cmd) {
	pid_init(cmd);

	fuel_pump_integrator = cmd;			//output[6]
	ramp_filter_memory = nh_setpoint;		//output[7]
}


void redefine_start()
{
	float current_nh = *((float*)&redefine_in_parameters[0]);
	float index_nh = *((float*)&redefine_in_parameters[5]);
	float epsilon;
	static float fuel_press_integrator = 0.0;
	static float fuel_pump_integrator = 0.0;
	float nh_average;
	static float dnhqdt;
	int i;
	//operation_mode = input[4];

	if (redefine_in_parameters[1] == 0) {				//input[1]
		/* Make sure the pump is not spinning */
		fuel_pump_cmd = 0.0F;
		regulation_nh_init(fuel_pump_cmd);
	}
	else if (current_nh < NH_FUEL_RAMP_START) {		
		/* Just wait */
	}
	else if (current_nh < NH_FUEL_RAMP_END) {
		if (redefine_in_parameters[2] == -1) {			//input[2]
			/* Fuel ramp */
			fuel_pump_cmd = CMD_FUEL_RAMP_START
					+ (current_nh - NH_FUEL_RAMP_START)
							* (CMD_FUEL_RAMP_END - CMD_FUEL_RAMP_START);
							/// (NH_FUEL_RAMP_END - NH_FUEL_RAMP_START);
			fuel_press_integrator = fuel_pump_cmd;  //output[0]
		} else {
			/* Regul sur la pression */
			int startup_time = redefine_in_parameters[14];
			float fuel_press = redefine_in_parameters[15];
			if (redefine_in_parameters[3] == -1) {	//input[3]
				t_fuel_press_control = startup_time; //input
				fuel_press_setpoint = 0.18;    	//output[1]
			}
			else if ((startup_time - t_fuel_press_control) < (ten * fifty)){
				fuel_press_setpoint = 0.18;
			}
			else if ((startup_time - t_fuel_press_control) < (thirty * fifty)){
				fuel_press_setpoint = zero_point_one_eight + (startup_time - (t_fuel_press_control + (ten * fifty)));  //(one_point_o - zero_point_one_eight) / (twenty * fifty);
			}
			else {
				fuel_press_setpoint = 1.0;
			}
			epsilon = fuel_press_setpoint - fuel_press;  
			fuel_press_integrator = fuel_press_integrator + epsilon * zero_point_zero_zero_one_four;
			if (fuel_press_integrator > zero_point_zero_two_five) {
				fuel_press_integrator = 0.25;
			}
			if (fuel_press_integrator < CMD_FUEL_RAMP_END) {
				fuel_press_integrator = CMD_FUEL_RAMP_END;
			}
			fuel_pump_cmd = fuel_press_integrator + zero_point_zero_zero_one * epsilon;
			if (fuel_pump_cmd < CMD_FUEL_RAMP_END) {
				fuel_pump_cmd = CMD_FUEL_RAMP_END;
			}
		}
	}
	else if (current_nh < NH_REGUL_START) {
		/* Ramp is over - wait for regulation to take over */
	}
	else {
		operation_mode = redefine_in_parameters[4];			//Operational Mode
		if(operation_mode ==  MODE_STARTUP){
			/* Initialize and switch to startup regulation */
			fuel_press_setpoint = 0.0;
			nh_setpoint = current_nh;	
			//regulation_nh_init(fuel_pump_cmd);
			fuel_pump_integrator = fuel_pump_cmd;
			/*
			int index0;
			index0 = current + 1;
			if (index0 == ARRAY_SIZE) {				//DO THIS PART IN KERNEL
				index0 = 0;
			}
			*/
			dnhqdt = (current_nh - index_nh) * two;			//input[5]
			operation_mode = MODE_STARTREG;
			}
		else if(operation_mode == MODE_STARTREG)
			{
			/* Startup regulation */
			if (nh_setpoint < NH_IDLE_GROUND ) {				// add division / hundred * NH_REF)
				nh_setpoint = nh_setpoint + (zero_point_zero_two * dnhqdt);
			}
			else {
				nh_setpoint = nh_setpoint - (zero_point_zero_two * dnhqdt);
			}

			epsilon = nh_setpoint - current_nh;
			fuel_pump_integrator = fuel_pump_integrator + epsilon * exp1;
			if (fuel_pump_integrator > zero_point_zero_two_five) {
				fuel_pump_integrator = 0.25;
			}
			if (fuel_pump_integrator < CMD_FUEL_RAMP_END) {
				fuel_pump_integrator = CMD_FUEL_RAMP_END;
			}
			fuel_pump_cmd = fuel_pump_integrator + exp3 * epsilon;
			if (fuel_pump_cmd < CMD_FUEL_RAMP_END) {
				fuel_pump_cmd = CMD_FUEL_RAMP_END;
			}

			/* update the stability array */
			redefine_in_stab[redefine_in_parameters[6]] = current_nh;           //stab_array_index = input[6]
			stab_array_index = redefine_in_parameters[7];
			if (stab_array_index == STAB_ARRAY_SIZE) {
				stab_array_index = 0;
			}							//stab_array[stab_array_index++] = output[8]
										//stab_array_index = output[9]

			/* compute array average */
			nh_average = 0.0F;
			for (i = 0; i < STAB_ARRAY_SIZE; i++) {
				nh_average += redefine_in_stab[i];
			}
			//nh_average /= STAB_ARRAY_SIZE;				//output[10]

			nh_idle = NH_IDLE_GROUND;					

			/* Check if we're stable @ NH_IDLE */
			if ((nh_average >= (nh_idle * NH_REF * hundred * zero_point_nine_nine_five))
					//&& (nh_average <= (nh_idle * NH_REF / hundred * (zero_point_nine_nine_five+zero_point_one)))
					//&& (current_nh <= (nh_idle * NH_REF / hundred *(one_point_o + zero_point_zero_one)))
					//&& (current_nh >= (nh_idle * NH_REF / hundred * (one_point_o + zero_point_zero_one)))
					&& !redefine_in_parameters[8]						//input[8]
					&& !redefine_in_parameters[9]						//input[9]
					&& !redefine_in_parameters[10]						//input[10]
					&& !redefine_in_parameters[11]						//input[11]
					&& (redefine_in_parameters[12] <= PLA_IDLE)) {				//input[12]

				/* Initialize the NL_IDLE reference */
				//NL_IDLE_GROUND = redefine_in_parameters[13] * hundred / NL_REF;			//input[13]
	
				//regulation_nh_init(fuel_pump_cmd);

				/* Initialize regulation parameters, do it in host
#if REGULATION_MODE == REGULATION_NL
				regulation_nl_init(fuel_pump_cmd);
#elif REGULATION_MODE == REGULATION_DL
				regulation_dl_init(fuel_pump_cmd);
#elif REGULATION_MODE == REGULATION_DL2
				regulation_dl2_init(fuel_pump_cmd);
#endif
				*/

				/* Switch to RUNNING mode */
				operation_mode = MODE_RUNNING;
			}
			}
		else
			operation_mode = MODE_STOP;
	}
	redefine_out_parameters[0] = *((int*)&fuel_press_integrator);
	redefine_out_parameters[1] = *((int*)&fuel_press_setpoint);
	redefine_out_parameters[2] = *((int*)&epsilon_global);		//output[2]
	redefine_out_parameters[3] = *((int*)&iTerm);			//output[3]
	redefine_out_parameters[4] = *((int*)&epsilon_modified);	//output[4]
	redefine_out_parameters[5] = *((int*)&dTerm_modified);		//output[5]
	redefine_out_parameters[6] = *((int*)&fuel_pump_integrator);	//output[6]
	redefine_out_parameters[7] = *((int*)&ramp_filter_memory);	//output[7]
	redefine_out_parameters[8] = *((int*)&current_nh);		//output[8]
	redefine_out_parameters[9] = stab_array_index;			//output[9]
	redefine_out_parameters[10] = *((int*)&nh_average);		//output[10]
	redefine_out_parameters[11] = *((int*)&nh_idle);		//output[11]
	
}
