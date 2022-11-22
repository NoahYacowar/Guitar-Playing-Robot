const int STRUMMING_POWER = 20;
const int FRET_SPEED = -20;
const int TIME_UNIT = 500;
const int COLOUR_PORT = S1;
const int ULTRASONIC_PORT = S2;
const int ARR_LENGTH = 100;
const int FULL_ROTATION = 360;
const int FRET_CLICKS = 30;

bool startAllTasks(int ultrasonic, int colour)
{
	SensorType[ultrasonic] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[colour] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[colour] = modeEV3Color_Color;
	wait1Msec(100);

	nMotorEncoder[motorA] = nMotorEncoder[motorD] = 0;

	return true;
}

void pluck(int direction)
{
	motor[motorA] = direction * STRUMMING_POWER;

	nMotorEncoder[motorA] = 0;

	while(abs(nMotorEncoder[motorA]) < 90)
	{}

	motor[motorA] = 0;
}

void setAndPlayFret(int fret, int direction)
{
	int clicksForFret = FRET_CLICKS * (fret - 1);
	int clicksToRotate = FULL_ROTATION - clicksForFret;

	motor[motorD] = FRET_SPEED;
	while(abs(nMotorEncoder[motorD]) < clicksToRotate)
	{}
	motor[motorD] = 0;

	pluck(direction);

	motor[motorD] = FRET_SPEED;
	while(abs(nMotorEncoder[motorD]) < FULL_ROTATION)
	{}
	motor[motorD] = 0;

	nMotorEncoder[motorD] = 0;
}

int playNotes(int fret[], int hold[], int length)
{
	int direction = 1;

	for(int note = 0; note < length; note++)
	{
		setAndPlayFret(fret[note], direction);

		wait1Msec[hold[note]*TIME_UNIT];

		direction *= -1;
	}

	return direction;
}

bool endAllTasks(int fret_start, int strum_start, int direction)
{
	motor[motorA] = motor[motorD] = 0;

	const int CURRENT_CLICKS = nMotorEncoder[motorD];
	const int OFFSET = CURRENT_CLICKS % 360;
	const int ADJUSTMENT = 360 - OFFSET + fret_start;

	nMotorEncoder[motorD] = 0;

	motor[motorD] = FRET_SPEED;
	while(abs(nMotorEncoder[motorD]) < ADJUSTMENT)
	{}
	motor[motorD] = 0;

	motor[motorA] = -1 * direction * STRUMMING_POWER;
	while(abs(nMotorEncoder[motorA]) > strum_start)
	{}
	motor[motorA] = 0;

	return true;
}

task main()
{
	bool startup = startAllTasks(ULTRASONIC_PORT, COLOUR_PORT);

	if(startup)
	{
		displayTextLine(5, "Startup successful");
	}

	int fret[ARR_LENGTH] = {-1};
	int timing[ARR_LENGTH] = {-1};

	//int const NOTE_COUNT = readSheet();
	displayTextLine(5, "Song loaded");

	int direction = playNotes(fret, timing, NOTE_COUNT);

	displayTextLine(5, "Song finished!");

	bool shutdown = endAllTasks(0,0, direction);

	if(shutdown)
	{
		displayTextLine(5, "Shutdown successful");
	}
}
