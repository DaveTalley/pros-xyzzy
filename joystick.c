/*
 * joystick.c
 *
 *  Created on: Dec 5, 2015
 *      Author: 1727X
 */
#include "joystick.h"

void Joysticks_Init(Joysticks *self)
{
	self->delay = 0;
	for (int i = 0; i < self->joyCount; i++)
	{
		self->joy[i].flags = 0;
		self->joy[i].analog[0] = 0;
		self->joy[i].analog[1] = 0;
		self->joy[i].analog[2] = 0;
		self->joy[i].analog[3] = 0;
		self->joy[i].analog[4] = 0;
		self->joy[i].analog[5] = 0;
		self->useAccel[i] = false;
	}
}

Joysticks * Joysticks_New(unsigned char numJoysticks)
{
	Joysticks *self = malloc(sizeof(Joysticks));
	self->joyCount = numJoysticks;
	self->joy = malloc(numJoysticks * sizeof(JoyInfo));
	self->useAccel = malloc(numJoysticks * sizeof(bool));
	Joysticks_Init(self);
	return self;
}

void Joysticks_Delete(Joysticks *self)
{
	free(self->joy);
	free(self);
}

void Joysticks_Copy(Joysticks *from, Joysticks *to)
{
    to->delay = from->delay;
	for (int i = 0; i < from->joyCount; i++)
	{
		to->joy[i].flags = from->joy[i].flags;
		to->joy[i].analog[0] = from->joy[i].analog[0];
		to->joy[i].analog[1] = from->joy[i].analog[1];
		to->joy[i].analog[2] = from->joy[i].analog[2];
		to->joy[i].analog[3] = from->joy[i].analog[3];
		to->joy[i].analog[4] = from->joy[i].analog[4];
		to->joy[i].analog[5] = from->joy[i].analog[5];
	}
}
signed char Joysticks_Compare(Joysticks *first, Joysticks *second)
{
	if (first->joyCount != second->joyCount) return 0;
	for (int i = 0; i < first->joyCount; i++)
	{
		if (first->joy[i].flags != second->joy[i].flags ||
				first->joy[i].analog[0] != second->joy[i].analog[0] ||
				first->joy[i].analog[1] != second->joy[i].analog[1] ||
				first->joy[i].analog[2] != second->joy[i].analog[2] ||
				first->joy[i].analog[3] != second->joy[i].analog[3] ||
				first->joy[i].analog[4] != second->joy[i].analog[4] ||
				first->joy[i].analog[5] != second->joy[i].analog[5])
			return 0;
	}
	return 1;
}


bool xJoystickGetDigital(JoyInfo *joy, unsigned char buttonGroup, unsigned char button)
{
	// Turn the button into a flag.
	// The button groups 5, 6, 7, 8 map to a group of 4 bits.
	// Each button in that group is mapped to one of those 4 bits.
	//   0000 0000 0000 0000
	//   |||| |||| |||| |||+---     1 Button 5, JOY_DOWN (1)
	//   |||| |||| |||| ||+----     2 N/A
	//   |||| |||| |||| |+-----     4 Button 5, JOY_UP (4)
	//   |||| |||| |||| +------     8 N/A
	//   |||| |||| |||+--------    16 Button 6, JOY_DOWN (1)
	//   |||| |||| ||+---------    32 N/A (Reserved for Arcade Drive: Button 6, JOY_LEFT)
	//   |||| |||| |+----------    64 Button 6, JOY_UP (4)
	//   |||| |||| +-----------   128 N/A
	//   |||| |||+-------------   256 Button 7, JOY_DOWN (1)
	//   |||| ||+--------------   512 Button 7, JOY_LEFT (2)
	//   |||| |+---------------  1024 Button 7, JOY_UP (4)
	//   |||| +----------------  2048 Button 7, JOY_RIGHT (8)
	//   |||+------------------  4096 Button 8, JOY_DOWN (1)
	//   ||+-------------------  8172 Button 8, JOY_LEFT (2)
	//   |+-------------------- 16384 Button 8, JOY_UP (4)
	//   +--------------------- 32768 Button 8, JOY_RIGHT (8)
	// To convert, need to figure out which group of bits to use. Can do that by
	// calculating the value of the first bit (1, 16, 256, 4096).
	// Each of those are 2 to the power of x (2^x) where x = 1, 4, 8, or 12.
	// Since calculations of integers are truncated (i.e. 1.75 becomes 1),
	// the value of x can be calculated using the formula x = (g - 5) * 4
	// where g is the group number (5, 6, 7, or 8).
	// A fast way to raise 2 to the power of x is to simply left-shift 1 by x bits.
	// Finally, multiply the resulting first bit by the button number to get the final flag.
	// e.g. Button 7: x = (7 - 5) * 4 = 8
	//                2^x = 2^8 = 2 << 8 = 256
	//      JOY_UP (4): 256 * 4 = 1024
	unsigned int flag = (1 << ((buttonGroup - 5)*4)) * button;
	// Use bitwise 'and' to see if the button flag is turned on (pressed) or not.
	if (joy) return (joy->flags & flag) > 0;
	else return false;
}

signed char xJoystickGetAnalog(JoyInfo *joy, unsigned char axis)
{
	if (joy) return joy->analog[axis-1];
	else return 0;
}

// Reads the actual joystick information into memory.
void _joystickRead(unsigned char joystick, JoyInfo *joy, bool useAccel)
{
	unsigned int oldFlags = joy->flags;
	joy->flags = 0;
	// Restore the reserved flags
	joy->flags += (oldFlags & 2) + (oldFlags & 8) + (oldFlags & 32) + (oldFlags & 128);
	// Iterate through all 12 buttons
	for (int i = 0; i < 16; i++)
	{
		if (i != 1 && i != 3 && i != 5 && i != 7) // Only legit buttons (by position)
		{
			unsigned char button = (1 << (i % 4)); // Button Number (1, 2, 4, or 8)
			unsigned char buttonGrp = ((i + 4) / 4); // Button Group (5, 6, 7, or 8)
			if (joystickGetDigital(joystick, buttonGrp, button))
				joy->flags += (1 << i); // Same as 2 to the power of i
		}
	}
	for (unsigned char x = 0; x < 6; x++)
	{
		if (!useAccel && x > 3) joy->analog[x] = 0;
		else joy->analog[x] = joystickGetAnalog(joystick, x + 1);
	}
}

void xJoystickRead(Joysticks *joysticks)
{
	for (int i = 0; i < joysticks->joyCount; i++)
	{
		_joystickRead(i + 1, &(joysticks->joy[i]), joysticks->useAccel[i]);
	}
}

void xJoystickSave(Joysticks *joysticks, FILE *fp)
{
	fputc(joysticks->delay, fp);
	fputc(joysticks->delay >> 8, fp);
	for (int i = 0; i < joysticks->joyCount; i++)
	{
		for (signed char a = 0; a < 6; a++)
		{
			fputc(joysticks->joy[i].analog[a] + 127, fp);
		}
		fputc(joysticks->joy[i].flags, fp);
		fputc(joysticks->joy[i].flags >> 8, fp);
		fputc(joysticks->joy[i].flags >> 16, fp);
		fputc(joysticks->joy[i].flags >> 24, fp);
	}
}

bool xJoystickLoad(Joysticks *joysticks, FILE *fp)
{
	int buffer = fgetc(fp);
	if (buffer != EOF)
	{
		joysticks->delay = buffer;
		joysticks->delay += fgetc(fp) << 8;
		for (int i = 0; i < joysticks->joyCount; i++)
		{
			for (signed char a = 0; a < 6; a++)
			{
				joysticks->joy[i].analog[a] = fgetc(fp) - 127;
			}
			joysticks->joy[i].flags = fgetc(fp);
			joysticks->joy[i].flags += fgetc(fp) << 8;
			joysticks->joy[i].flags += fgetc(fp) << 16;
			joysticks->joy[i].flags += fgetc(fp) << 24;
		}
	}
	return (buffer != EOF);
}
