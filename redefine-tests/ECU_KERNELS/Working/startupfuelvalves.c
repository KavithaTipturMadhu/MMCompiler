#define NH_START_VALVE_OPEN				4000
#define NH_START_VALVE_SHUTOFF			12500
#define NH_MAIN_RAMP_VALVE_OPEN			8800
#define FUEL_MAIN_VALVE_OPEN_CMD		0
#define FUEL_MAIN_VALVE_CLOSE_CMD		1
#define FUEL_START_VALVE_CLOSE_CMD		1
#define FUEL_START_VALVE_OPEN_CMD		0

int redefine_in_a[7];
//int redefine_in_current_nh;
//int redefine_in_startup_time;
//int redefine_in_fuel_main_valve_cmd;
//int redefine_in_fuel_start_valve_cmd;
//int redefine_in_start_valve_has_been_closed;
//int redefine_in_t_start_valve_open;
//int redefine_in_t_main_valve_open;

int redefine_out_b[5];
//int redefine_out_fuel_main_valve_cmd;
//int redefine_out_fuel_start_valve_cmd;
//int redefine_out_start_valve_has_been_closed;
//int redefine_out_t_start_valve_open;
//int redefine_out_t_main_valve_open;

/*
int asl_open_fuel_main_valve() {
	return FUEL_MAIN_VALVE_OPEN_CMD;
}

int asl_close_fuel_main_valve() {
	return FUEL_MAIN_VALVE_CLOSE_CMD;
}

int asl_close_fuel_startup_valve() {
	return FUEL_START_VALVE_CLOSE_CMD;
}

int asl_open_fuel_startup_valve() {
	return FUEL_START_VALVE_OPEN_CMD;
}
*/
int *startup_fuel_valves(int input[]) {//int current_nh, int startup_time, int *fuel_main_valve_cmd, int *fuel_start_valve_cmd, int *start_valve_has_been_closed, int *t_start_valve_open, int *t_main_valve_open) {
	float current_nh = *((float*)&input[0]);
	if (input[1] == 0) {
		/* Make sure that fuel valves are closed */
		//*fuel_main_valve_cmd = asl_close_fuel_main_valve();
		//*fuel_start_valve_cmd = asl_close_fuel_startup_valve();
		input[2] = FUEL_MAIN_VALVE_CLOSE_CMD; //asl_close_fuel_main_valve();
		input[3] = FUEL_START_VALVE_CLOSE_CMD; //asl_close_fuel_startup_valve();
		/* Start valve flag init */
		//*start_valve_has_been_closed = 0;
		input[4] = 0;
	}
	else {
		/* Start valve */
		if (current_nh < NH_START_VALVE_OPEN) {
			/* Do nothing */
		}
		else if (current_nh < NH_START_VALVE_SHUTOFF) {
			if (input[4] == 0) {
				/* The start valve hasn't been closed yet -> open it */
				//*fuel_start_valve_cmd = asl_open_fuel_startup_valve();
				input[3] = FUEL_START_VALVE_OPEN_CMD; //asl_open_fuel_startup_valve();
				/* Set time stamp */
				if (input[5] == -1) {
					//*t_start_valve_open = startup_time;
					input[5] = input[1];				
				}
				else {}
			}
			else {
				/* The start valve has already been closed but NH probably went down. */
				/* Leave it closed by doing nothing and hope for the best. */
			}
		}
		else {
			//*fuel_start_valve_cmd = asl_close_fuel_startup_valve();
			//*start_valve_has_been_closed = 1;
			input[3] = FUEL_START_VALVE_CLOSE_CMD; //asl_close_fuel_startup_valve();
			input[4] = 1;
		}

		/* Main valve */
		if (current_nh > (NH_MAIN_RAMP_VALVE_OPEN)) {
			//*fuel_main_valve_cmd = asl_open_fuel_main_valve();
			input[2] = FUEL_MAIN_VALVE_OPEN_CMD; //asl_open_fuel_main_valve();
			if (input[6] == -1) {
				//*t_main_valve_open = startup_time;
				input[6] = input[1];			
			}
		}
		else {}
	}

    return input;
}

void redefine_start()
{
//	redefine_out_fuel_main_valve_cmd = 
	int *output = startup_fuel_valves(redefine_in_a);//redefine_in_current_nh, redefine_in_startup_time, &redefine_in_fuel_main_valve_cmd, &redefine_in_fuel_start_valve_cmd, &redefine_in_start_valve_has_been_closed, &redefine_in_t_start_valve_open, &redefine_in_t_main_valve_open);

	//redefine_out_b[0] = redefine_in_a[2];
	//redefine_out_b[1] = redefine_in_a[3];
	//redefine_out_b[2] = redefine_in_a[4];
	//redefine_out_b[3] = redefine_in_a[5];
	//redefine_out_b[4] = redefine_in_a[6];

	redefine_out_b[0] = output[2];
	redefine_out_b[1] = output[3];
	redefine_out_b[2] = output[4];
	redefine_out_b[3] = output[5];
	redefine_out_b[4] = output[6];
	//redefine_out_fuel_start_valve_cmd = redefine_in_fuel_start_valve_cmd;
	//redefine_out_start_valve_has_been_closed = redefine_in_start_valve_has_been_closed;
	//redefine_out_t_start_valve_open = redefine_in_t_start_valve_open;
	//redefine_out_t_main_valve_open = redefine_in_t_main_valve_open;
}
