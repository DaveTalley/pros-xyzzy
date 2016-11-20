/*
 * drive.h
 *
 *  Created on: Dec 6, 2015
 *      Author: David J. Talley
 *
 *  Purpose: Implements either tank or arcade drive controls
 *  Requires the Joystick library, which would allow you to save drive commands to a file and replay during autonomous.
 *
 */
#ifndef INCLUDE_DRIVE_H_
#define INCLUDE_DRIVE_H_

#include "joystick.h"

typedef struct _wheels
{
	char frontLeft, frontRight, backLeft, backRight;
} Wheels;

// Creates a new Wheels structure that has been initialized.
// Call during initialize() from init.c
// Allocates memory to hold the Wheels structure.
Wheels *Wheels_New();
// Use this to toggle to/from Arcade Drive.
// Tank drive is always used by default.
// If you want to default to Arcade drive call toggleDrive right after initializing the Joysticks.
// You can also program button combinations if you want to be able to toggle between drives during driver control.
void toggleDrive(JoyInfo *joy);
// Sets the Wheels for driving.
// Call this within a while(1) loop in operatorControl() in opcontrol.c
// It doesn't actually move the robot. It simply sets the Wheels structure with the motor values that would
// move the robot in the right direction.
void drive(JoyInfo *joy, Wheels *wheels, char deadzone);

#endif /* INCLUDE_DRIVE_H_ */
