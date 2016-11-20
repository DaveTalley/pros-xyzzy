/*
 * record.c
 *
 *  Created on: Dec 6, 2015
 *      Author: David J. Talley
 */
#include "record.h"

void recordExists(const char *filename, void (*callback)(enum RecordStatus_t))
{
#ifdef DEBUG
	printf("\tChecking for file %s.\r\n", filename);
#endif
	bool exists = false;
	FILE *fp;
	fp = fopen (filename, "r");
	if (fp)
	{
		unsigned char numJoysticks = fgetc(fp);
		if (numJoysticks != EOF)
		{
			exists = true;
		}
		fclose(fp);
	}
	if (exists) callback(REC_LOAD_OK);
	else callback(REC_LOAD_FAIL);
}

void recordPlayback(const char *filename, void (*move)(Joysticks *), void (*stop)(), void (*callback)(enum RecordStatus_t))
{
	if (!move) return; // Nothing to do without a move function.
#ifdef DEBUG
	printf("vvvvvvvvvv Playback Started - %s vvvvvvvvvv\r\n", filename);
#endif
	if (callback) callback(REC_PLAY_ON); // started

	FILE *fp;
	fp = fopen (filename, "r");
	if (fp)
	{
		// Create the correct joystick configuration
		unsigned char numJoysticks = fgetc(fp);
		Joysticks *joysticks = Joysticks_New(numJoysticks);
		unsigned char useAccel = fgetc(fp);
		if (numJoysticks > 0) joysticks->useAccel[0] = (useAccel == 1);
		useAccel = fgetc(fp);
		if (numJoysticks > 1) joysticks->useAccel[1] = (useAccel == 1);
		// Get the default delay
		unsigned short delayMS = fgetc(fp);
		delayMS += fgetc(fp) << 8;

		// Now keep reading each joystick setting and apply it to the move function.
		while (!feof(fp))
		{
			if (rexJoystickLoad(joysticks, fp))
			{
				move(joysticks);
				if (joysticks->delay > 0)
				{
					delay(joysticks->delay);
				}
				delay(delayMS);
			}
		}

		Joysticks_Init(joysticks);
		if (stop) stop();
		Joysticks_Delete(joysticks);
		fclose(fp);
	}
#ifdef DEBUG
	else
	{
		printf("\tFile %s Not Found.\r\n", filename);
	}
	printf("^^^^^^^^^^ Playback Stopped ^^^^^^^^^^\r\n");
#endif
	if (callback) callback(REC_PLAY_OFF); // stopped
}

void recordStart(Joysticks *joysticks, const char *filename
		, void (*move)(Joysticks *), void (*stop)(), void (*callback)(enum RecordStatus_t)
		, const unsigned short delayMS, const unsigned int maxSeconds)
{
	if (isOnline()) return;
#ifdef DEBUG
	printf("vvvvvvvvvv Record Started - %s vvvvvvvvvv\r\n", filename);
#endif
	delay(delayMS);
	if (callback) callback(REC_ON); // started

	FILE *fp;
	fp = fopen (filename, "w");
	if (fp)
	{
		// First byte in the file indicates the number of joysticks
		unsigned char numJoysticks = joysticks->joyCount;
		fputc(numJoysticks, fp);
		// Save the useAccel flags
		if (numJoysticks > 0 && joysticks->useAccel[0]) fputc(1, fp);
		else fputc(0, fp);
		if (numJoysticks > 1 && joysticks->useAccel[1]) fputc(1, fp);
		else fputc(0, fp);
		// Next set of bytes is the default delay
		fputc(delayMS, fp);
		fputc(delayMS >> 8, fp);
		// Initialize time settings
		unsigned long timeStart = millis(); // Current milliseconds time on the Cortex
		unsigned long time = 0;
		// Initialize a new joystick buffer
		Joysticks *jBuffer = Joysticks_New(numJoysticks);
		for (int x = 0; x < numJoysticks; x++) jBuffer->useAccel[x] = joysticks->useAccel[x];
		// Start the recording loop
		unsigned int i = 0;
		while ((time / 1000) <= maxSeconds)
		{
			rexJoystickRead(joysticks);
			if (i == 0)
			{
				Joysticks_Copy(joysticks, jBuffer);
			}
			else if (Joysticks_Compare(joysticks, jBuffer))
			{
				jBuffer->delay += delayMS;
			}
			else
			{
				rexJoystickSave(jBuffer, fp);
				Joysticks_Copy(joysticks, jBuffer);
				jBuffer->delay = 0;
			}
			move(jBuffer);
			time = millis() - timeStart;
			i++;
			delay(delayMS);
		}
		// Catch the last one
		if (jBuffer->delay)
		{
			rexJoystickSave(jBuffer, fp);
		}
		Joysticks_Delete(jBuffer);
		stop();
		fclose(fp);
	}
#ifdef DEBUG
	printf("^^^^^^^^^^ Record Stopped ^^^^^^^^^^\r\n");
#endif
	if (callback) callback(REC_OFF); // stopped
}
