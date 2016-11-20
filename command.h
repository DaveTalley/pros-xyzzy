/*
 * command.h
 *
 *  Created on: Dec 11, 2015
 *      Author: David J. Talley
 *
 *  Purpose: Implementation of a Command design pattern.
 *  The intent is to simplify the execution of joystick commands so that actions can be implemented as toggles or press-and-hold.
 *  Requires use of the Joystick library.
 *
 */

#ifndef INCLUDE_COMMAND_H_
#define INCLUDE_COMMAND_H_

#include <API.h>
#include "joystick.h"

typedef struct _command
{
	signed char triggered;
	void (*execute)(Joysticks *);
} Command;

// Initialize a new array of commands.
// Call during initialize() from init.c
// Allocates memory to hold the Command array.
// Each command is really just a function pointer to your own implementation that accepts a Joysticks pointer as a parameter.
Command * Command_New(int count);
// Try to execute the command, based on Joystick settings. The command is called every iteration of your control loop.
// This would typically be used for something like driving the robot.
void tryExecute(Command *command, Joysticks *joysticks);
// Try to execute the command, but only while the joystick combination is held (e.g. holding a button to keep a motor on).
void tryExecuteHold(Command *commandOn, Command *commandOff, Joysticks *joysticks, bool on);
// Try to execute the command, but as a toggle. Every time you press the button combination, it fires the command just once.
// It uses the triggered field in the command structure to keep the command from firing again while the button combination is held down.
// Once the button combination is released, the triggered field is cleared and you can press the button combination again to fire the command
// another time.
void tryExecuteOnce(Command *command, Joysticks *joysticks, bool triggered);

#endif /* INCLUDE_COMMAND_H_ */

/* EXAMPLE IMPLEMENTATION

#include "joystick.h"
#include "drive.h"
#include "command.h"
Joysticks *_joysticks;
Wheels *_wheels;
Command *_commands;
const unsigned char CMD_DRIVE = 0;
const unsigned char CMD_TOGGLE_DRIVE = 1;
const unsigned char CMD_TURN_ON = 2;
const unsigned char CMD_TURN_OFF = 3;

// Executes each command.
void move(Joysticks *joysticks)
{
	// Turn on a motor while a button is pressed, and turn it off when it is not pressed.
	tryExecuteHold(&_commands[CMD_TURN_ON], &_commands[CMD_TURN_OFF], joysticks, xJoystickGetDigital(&(joysticks->joy[0]), 7, JOY_UP));
	// Toggle drive mode when two button are pressed at the same time.
	tryExecuteOnce(&_commands[CMD_TOGGLE_DRIVE], joysticks, (xJoystickGetDigital(&(joysticks->joy[0]), 7, JOY_DOWN) && xJoystickGetDigital(&(joysticks->joy[0]), 7, JOY_RIGHT)));
	// Drive the robot.
	tryExecute(&_commands[CMD_DRIVE], joysticks);
}

// Assigned to the CMD_DRIVE command.
void cmdDrive(Joysticks *joysticks)
{
	drive(&(joysticks->joy[0]), _wheels, 15);
	motorSet(1, _wheels->backLeft);
	motorSet(2, _wheels->backRight);
	motorSet(3, _wheels->frontLeft);
	motorSet(4, _wheels->frontRight);
}

// Assigned to the CMD_TOGGLE_DRIVE command.
void cmdToggleDrive(Joysticks *joysticks)
{
	toggleDrive(&(joysticks->joy[0]));
}

// Assigned to the CMD_TURN_ON command
void cmdTurnOn(Joysticks *joysticks)
{
	motorSet(5, 100);
}

// Assigned to the CMD_TURN_OFF command
void cmdTurnOff(Joysticks *joysticks)
{
	motorSet(5, 0);
}

void operatorControl()
{
	// Initialize one joystick.
	_joysticks = Joysticks_New(1);
	// Initialize the wheels.
	_wheels = Wheels_New();
	// Initialize the commands.
	_commands = Command_New(4);
	_commands[CMD_DRIVE].execute = &cmdDrive;
	_commands[CMD_TOGGLE_DRIVE].execute = &cmdToggleDrive;
	_commands[CMD_TURN_ON].execute = &cmdTurnOn;
	_commands[CMD_TURN_OFF].execute = &cmdTurnOff;

	while (1)
	{
		// Read the joystick.
		xJoystickRead(_joysticks);
		// Move everything.
		move(_joysticks);
		// Delay to allow cortex to timeslice threads.
		delay(20);
	}
}

*/
