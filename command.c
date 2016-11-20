/*
 * command.c
 *
 *  Created on: Dec 11, 2015
 *      Author: David J. Talley
 */

#include "command.h"

Command * Command_New(int count)
{
	Command *cmd;
	cmd = malloc(count * sizeof(Command));
	for (int i = 0; i < count; i++)
	{
		cmd[i].triggered = 0;
		cmd[i].execute = 0;
	}
	return cmd;
}

// Execute a command.
void tryExecute(Command *command, Joysticks *joysticks)
{
	if (command->execute) command->execute(joysticks);
}

void tryExecuteHold(Command *commandOn, Command *commandOff, Joysticks *joysticks, bool on)
{
	if (on)
	{
		if (commandOn->execute) commandOn->execute(joysticks);
	}
	else
	{
		if (commandOff->execute) commandOff->execute(joysticks);
	}
}

// Execute a command only once while triggered.
void tryExecuteOnce(Command *command, Joysticks *joysticks, bool triggered)
{
	if (triggered) // Want it triggered only once while it's being triggered
	{
		if (!command->triggered)
		{
			command->triggered = 1; // This locks it
			if (command->execute) command->execute(joysticks);
		}
	}
	else if (command->triggered) // Allow to be triggered again
	{
		command->triggered = 0;
	}
}
