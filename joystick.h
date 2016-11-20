/*
 * joystick.h
 *
 *  Created on: Dec 5, 2015
 *      Author: David J. Talley
 *
 *  Purpose: The Joystick library creates a common structure that holds the state of up to two joysticks.
 *  This data can be saved to a file and passed to other functions to implement actions.
 *
 *  Written for PROS for a VEX Cortex
 *  http://pros.cs.purdue.edu/
 *
 */

#ifndef INCLUDE_JOYSTICK_H_
#define INCLUDE_JOYSTICK_H_

#include <API.h>

typedef struct _joyInfo
{
	unsigned int flags;
	char analog[6];
} JoyInfo;

typedef struct _joysticks
{
	unsigned char joyCount;
	JoyInfo *joy;
	bool *useAccel;
	unsigned short delay; // used for autonomous
} Joysticks;

// Creates a new copy of Joysticks.
// Call during initialize() from init.c
// Allocates memory to hold the Joysticks structure.
// Implicitly calls Joysticks_Init.
// Returns a pointer to the new structure.
// You must call Joysticks_Delete to free the allocated memory.
Joysticks * Joysticks_New(unsigned char numJoysticks);
// Initializes Joysticks to zero.
// Zeros out the whole Joysticks structure.
void Joysticks_Init(Joysticks *self);
// Delete the copy of Joysticks.
// Frees the allocated memory for a Joysticks structure.
void Joysticks_Delete(Joysticks *self);
// Copy joystick settings.
// Copies the values from one Joysticks structure to a different Joysticks structure.
void Joysticks_Copy(Joysticks *from, Joysticks *to);
// Compares settings to see if they are the same.
// Useful for compressing recorded data.
// Returns 0 (false) if there are differences, 1 (true) if they are identical.
signed char Joysticks_Compare(Joysticks *first, Joysticks *second);

// Get values from the joystick structure like the actual thing.
// Use this with a Joystick pointer in place of the API JoystickGetDigital function.
bool xJoystickGetDigital(JoyInfo *joy, unsigned char buttonGroup, unsigned char button);
// Get values from the joystick structure like the actual thing.
// Use this with a Joystick pointer in place of the API JoystickGetDigital function.
signed char xJoystickGetAnalog(JoyInfo *joy, unsigned char axis);
// Reads the actual joystick information into the structure.
// Call this within a while(1) loop in operatorControl() in opcontrol.c
void xJoystickRead(Joysticks *joysticks);
// Saves the joystick structure to a file.
// Call this to record joystick data into a file.
void xJoystickSave(Joysticks *joysticks, FILE *fp);
// Reads the file data into a joystick structure.
// This would be done during autonomous() in auto.c
bool xJoystickLoad(Joysticks *joysticks, FILE *fp);

#endif /* INCLUDE_JOYSTICK_H_ */
