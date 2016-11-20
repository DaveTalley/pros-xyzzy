/*
 * drive.c
 *
 *  Created on: Dec 5, 2015
 *      Author: David J. Talley
 */
#include "drive.h"

Wheels *Wheels_New()
{
	Wheels *wheels = malloc(sizeof(Wheels));
	wheels->frontLeft = 0;
	wheels->frontRight = 0;
	wheels->backLeft = 0;
	wheels->backRight = 0;
	return wheels;
}

void toggleDrive(JoyInfo *joy)
{
	if (joy->flags & 32)
	{
		joy->flags -= 32;
	}
	else
	{
		// Set Arcade Drive on the joystick, for autonomous
		joy->flags += 32;
	}
}

void _arcadeDrive(JoyInfo *joy, Wheels *wheels, char deadzone)
{
	char channelA = rexJoystickGetAnalog(joy, 1); // right X
	if (abs(channelA) <= deadzone) channelA = 0;
	char channelB = rexJoystickGetAnalog(joy, 4); // left X
	if (abs(channelB) <= deadzone) channelB = 0;
	char channelC = rexJoystickGetAnalog(joy, 3); // left Y
	if (abs(channelC) <= deadzone) channelC = 0;

	// Assign the joystick position to the wheels.
	// Front Left
	//wheels->frontLeft = channelC + channelA + channelB;
	int v = channelC + channelA + channelB;
	if (v > 127) wheels->frontLeft = 127;
	else if (v < -127) wheels->frontLeft = -127;
	else wheels->frontLeft = v;
	// Front Right
	//wheels->frontRight = -channelC + channelA + channelB;
	v = -channelC + channelA + channelB;
	if (v > 127) wheels->frontRight = 127;
	else if (v < -127) wheels->frontRight = -127;
	else wheels->frontRight = v;
	// Back Left
	//wheels->backLeft = channelC + channelA - channelB;
	v = channelC + channelA - channelB;
	if (v > 127) wheels->backLeft = 127;
	else if (v < -127) wheels->backLeft = -127;
	else wheels->backLeft = v;
	// Back Right
	//wheels->backRight = -channelC + channelA - channelB;
	v = -channelC + channelA - channelB;
	if (v > 127) wheels->backRight = 127;
	else if (v < -127) wheels->backRight = -127;
	else wheels->backRight = v;
}

void _tankDrive(JoyInfo *joy, Wheels *wheels, char deadzone)
{
	/* Tank Drive Configuration
	 * Notes: Motor spins as you look down on axle
	 *    Axis 1 - Left = Clockwise
	 *    Axis 2 - Forward = Counter-Clockwise
	 *    Axis 3 - Forward = Clockwise
	 *    Axis 4 - Left = Counter-Clockwise
	 */
	char channelA = rexJoystickGetAnalog(joy, 2); // right Y
	if (abs(channelA) <= deadzone) channelA = 0;
	char channelB = rexJoystickGetAnalog(joy, 3); // left Y
	if (abs(channelB) <= deadzone) channelB = 0;
	// Strafe if the X axis absolute value is larger than the Y
	bool isStrafeRight = false;
	bool isStrafeLeft = false;
	signed char rightStrafe = rexJoystickGetAnalog(joy, 1); // right X
	if (abs(rightStrafe) <= deadzone) rightStrafe = 0;
	signed char leftStrafe = rexJoystickGetAnalog(joy, 4); // left X
	if (abs(leftStrafe) <= deadzone) leftStrafe = 0;
	if (abs(leftStrafe) > abs(channelB))
	{
		channelB = leftStrafe;
		isStrafeLeft = true;
	}
	else leftStrafe = 0;
	if (abs(rightStrafe) > abs(channelA))
	{
		if ((leftStrafe >= 0 && rightStrafe > 0) ||
			(leftStrafe <= 0 && rightStrafe < 0))
		{
			channelA = rightStrafe;
			isStrafeRight = true;
		}
	}
	// Assign the joystick position to the wheels.
	if (isStrafeRight)
	{
		wheels->frontRight = channelA;
		wheels->backRight = -channelA;
	}
	else
	{
		wheels->frontRight = -channelA;
		wheels->backRight = -channelA;
	}
	if (isStrafeLeft)
	{
		wheels->frontLeft = channelB;
		wheels->backLeft = -channelB;
	}
	else
	{
		wheels->frontLeft = channelB;
		wheels->backLeft = channelB;
	}
}

void drive(JoyInfo *joy, Wheels *wheels, char deadzone)
{
	if (joy->flags & 32) // Arcade
	{
		_arcadeDrive(joy, wheels, deadzone);
	}
	else
	{
		_tankDrive(joy, wheels, deadzone);
	}
}
