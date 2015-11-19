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

#include "MelonCAN.h"

// Constructor
// Pick the CAN controller type (FlexCAN for Teensy, MCP2515 for MCP2515)
// If it's a 2515, we need to pass in the chip select pin too (SPI)
MelonCAN::MelonCAN(can_controller_type canType, uint8_t cs)
{
    controllerType = canType;
    if (controllerType == mcp2515_controller)
        chipSelect = cs;
}

// Initialize
// Create a new object and assign it to the appropriate pointer
// based on the controller type selected in the constructor.
// Set the baud rate; MCP2515 expects a value from 0->13 which
// correspond to the defines in mcp_can_dfs.h
// 
// This function has the ability to accept either a baud rate
// or a define value, though it only translates the four most
// popular speeds, and defaults to 250KBPS otherwise.
// 
// Finally, it calls the begin() function
uint8_t MelonCAN::init(uint32_t baud)
{
    if (controllerType == flex_controller)
    {
        if (baud <= 13)
        {
            switch (baud)
            {
            case CAN_125KBPS: baud = 125000; break;
            case CAN_250KBPS: baud = 250000; break;
            case CAN_500KBPS: baud = 500000; break;
            case CAN_1000KBPS: baud = 1000000; break;
            default: baud = 250000;
            }
        }
        myFlex = new FlexCAN(baud);
    }
    else if (controllerType == mcp2515_controller)
    {
        if (baud > 13)
        {
            switch (baud){
            case 125000: baud = CAN_125KBPS; break;
            case 250000: baud = CAN_250KBPS; break;
            case 500000: baud = CAN_500KBPS; break;
            case 1000000: baud = CAN_1000KBPS; break;
            default: baud = CAN_250KBPS;
            }
        }
        myMCP = new MCP_CAN(chipSelect);
        selectedBaud = baud;
    }

    return begin();
}

// Begin
// Calls the Begin() function for FlexCAN
// Calls the Begin(baud) function for MCP_CAN, using the
// baud rate selected in init()
uint8_t MelonCAN::begin()
{
    if (controllerType == flex_controller)
        myFlex->begin();
    if (controllerType == mcp2515_controller)
        return myMCP->begin(selectedBaud);
    return 1;
}

// Write
// Write a message to the CAN bus.
// This function expects a CAN_message_t as defined in FlexCAN.h
void MelonCAN::write(const CAN_message_t &message)
{
    if (controllerType == flex_controller)
    {
        myFlex->write(message);
    }
    else if (controllerType == mcp2515_controller)
    {
        myMCP->sendMsgBuf(message.id, message.ext, message.len, (byte*)message.buf);
    }
}

// Read
// Read a message from the CAN bus.
// This function expects a CAN_message_t as defined in FlexCAN.h
//
// The MCP_CAN library does not make the "ext" flag public,
// but it *does* handle the extended message identifiers in the background.
// Therefore, if the identifier is > ((2^11)-1), it must be an extended id.
void MelonCAN::read(CAN_message_t &message)
{
    if (controllerType == flex_controller)
    {
        myFlex->read(message);
    }
    else if (controllerType == mcp2515_controller)
    {
        uint8_t len = 0;
        myMCP->readMsgBuf(&len, (byte *)message.buf);
        message.len = len;
        message.id = myMCP->getCanId();
        message.ext = (message.id >= 2048) ? 1 : 0;
    }
}

// Available
// This function checks to see if a frame is available
// In FlexCAN this is done with a flag.  In MCP_CAN, it's done by
// reading a register on the MCP2515.
// It is not necessary to use this function for MCP2515, as it can
// provide an external interrupt.
uint8_t MelonCAN::available(){
    if (controllerType == flex_controller)
        return myFlex->available();
    else if (controllerType == mcp2515_controller)
        return (myMCP->checkReceive() == CAN_MSGAVAIL) ? 1 : 0;
    else
        return 0;
}