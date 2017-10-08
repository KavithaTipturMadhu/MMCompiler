#define	START_BUTTON		0x00000001
#define	VENTILATION_BUTTON	0x00000002
#define	GP1			0x00000004
#define	GP2			0x00000008
#define	MAIN_VALVE		0x00000010
#define	START_VALVE		0x00000020
#define	FUEL_FILTER		0x00000040
#define	OIL_FILTER		0x00000080
#define	SG_TEMP_WARM		0x00000100
#define	SG_TEMP_HOT		0x00000200
#define	LOW_OIL_LEVEL		0x00000400

int redefine_in_sensors[10];
//int redefine_in_master_on_off;
//int redefine_in_crank1;
//int redefine_in_gp2_current;
//int redefine_in_gp1_current;
//int redefine_in_main_valve;
//int redefine_in_fuel_filter;
//int redefine_in_oil_filter;
//int redefine_in_sg_temp_1;
//int redefine_in_sg_temp_2;
//int redefine_in_oil_level;

int redefine_out_eicas_discretes;

int eicas_discretes = 0;

int AssignFunction(int sensors[])
{
	
	if (sensors[0])
		eicas_discretes |= START_BUTTON;
	if (sensors[1])
		eicas_discretes |= VENTILATION_BUTTON;

	if (sensors[2] > 250 || sensors[3] > 250) 
		eicas_discretes |= GP1;
	/*
	if (sensors[2] > 250) 	
		eicas_discretes |= GP1;
	if(sensors[3] > 250)
		eicas_discretes |= GP1;
	*/
	if (sensors[4])
		eicas_discretes |= MAIN_VALVE;
	if (sensors[5])
		eicas_discretes |= START_VALVE;
	if (sensors[6])
		eicas_discretes |= FUEL_FILTER;
	if (sensors[7])
		eicas_discretes |= OIL_FILTER;
	if (sensors[8])
		eicas_discretes |= SG_TEMP_WARM;
	if (sensors[9])
		eicas_discretes |= SG_TEMP_HOT;	

	
	
	return eicas_discretes;
}
void redefine_start()
{
	redefine_out_eicas_discretes = AssignFunction(redefine_in_sensors);
}

