/*
 * record.h
 *
 *  Created on: Dec 6, 2015
 *      Author: David J. Talley
 *
 *  Purpose: Records joystick positions into a file and plays those joystick positions back.
 *  Intended for playback during autonomous.
 *  Requires the Joystick library.
 *
 */
#ifndef INCLUDE_RECORD_H_
#define INCLUDE_RECORD_H_

#include "joystick.h"

// Record Status Flags
enum RecordStatus_t {REC_ON, REC_OFF, REC_LOAD_OK, REC_LOAD_FAIL, REC_PLAY_ON, REC_PLAY_OFF};

// Checks to see if a recorded file exists.
// If the file exists, sends REC_LOAD_OK to the callback function.
// Otherwise, sends REC_LOAD_FAIL to the callback function.
// The callback can then be used to do something indicating if you have a recorded file or not, such as flash some LEDs.
void recordExists(const char *filename, void (*callback)(enum RecordStatus_t));

// Starts the recording process for the specified number of seconds.
// Requires a pointer to a "move" function that takes a Joysticks pointer as a parameter.
// That move function should be the same that's used during operatorControl. In this way, the driver
// simply operates the robot normally, and all joystick settings are saved in the file during the recording.
// Requires a pointer to a "stop" function that will stop the robot at the end of recording.
// The callback sends REC_ON at the beginning and REC_OFF when recording ends.  Use those statuses to set indicators
// such as LEDs so the driver knows when recording is engaged.
void recordStart(Joysticks *joysticks, const char *filename
		, void (*move)(Joysticks *), void (*stop)(), void (*callback)(enum RecordStatus_t)
		, const unsigned short delayMS, const unsigned int maxSeconds);

// Use this in autonomous. It reads all of the Joystick settings from the file and applies them to the "move" function to move the robot.
// When the playback completes, it calls the "stop" fuction to stop the robot.
// The callback sends REC_PLAY_ON at the start of playback and REC_PLAY_OFF at the end.
void recordPlayback(const char *filename, void (*move)(Joysticks *), void (*stop)(), void (*callback)(enum RecordStatus_t));

#endif /* INCLUDE_RECORD_H_ */
