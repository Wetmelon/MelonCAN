/* 
 * MelonCAN Library Teensy 3.1/3.2
 * Copyright (c) 2015, Paul Guenette, pfguenette@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ---------------------------------------------------------------------------
 *
 * This library is designed to act as a bridge between the FlexCAN library for
 * Teensy 3.1/3.2 and the MCP_CAN library designed for interfacing with the
 * MCP2515 CAN Controller from Microchip.  It defines a message in the way
 * that the FlexCAN library does, and in fact expects the user to use the
 * FlexCAN message types.  However, it is capable of handling both reads and
 * writes with the same functions on either interface.
 * 
 */

#ifndef _MELONCAN_h
#define _MELONCAN_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <kinetis_flexcan.h>
#include <FlexCAN.h>
#include <mcp_can.h>

typedef enum
{
	flex_controller,
	mcp2515_controller
} can_controller_type;

class MelonCAN
{
private:
	can_controller_type controllerType;
	FlexCAN* myFlex;
	MCP_CAN* myMCP;
	uint8_t chipSelect;
	uint32_t selectedBaud;

public:
	MelonCAN(can_controller_type = flex_controller, uint8_t cs = 0);
	uint8_t init(uint32_t baud);
	uint8_t begin();
	void write(const CAN_message_t &msg);
	void read(CAN_message_t &msg);
	uint8_t available();
};

#endif