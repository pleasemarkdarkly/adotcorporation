//****************************************************************************
//
// I2C.C - Routines to communicate to an I2C device.
//
// Copyright (c) 2000 Cirrus Logic, Inc.
//
//****************************************************************************
#include "../hwdefs.h"
#include "globals.h"

//****************************************************************************
//
// Only include this code if we have an I2C clock defined.
//
//****************************************************************************
#ifdef HwPortABCD_I2C_SCL

//****************************************************************************
//
// I2CWrite writes a set of values over the I2C bus.  The first value is the
// target device address, the second value is the register number, and the
// third value is the register value.
//
//****************************************************************************
void
I2CWrite(unsigned char ucAddr, unsigned char ucReg, unsigned char ucValue)
{
    unsigned long * volatile pulPtr = (unsigned long *)HwBaseAddress;
    unsigned long ulVal, ulLoop;
    unsigned char ucData, ucIdx, ucBit;

    //
    // Read the current value of the GPIO register.
    //
    ulVal = pulPtr[HwPortABCD >> 2];

    //
    // Drive the I2C data line low.  Since the I2C clock line is currently
    // high, this is the start condition.
    //
    ulVal &= ~HwPortABCD_I2C_SDA;
    pulPtr[HwPortABCD >> 2] = ulVal;

    //
    // Delay to meet the I2C bus timing specification.
    //
    for(ulLoop = 0; ulLoop < 4; ulLoop++)
    {
    }

    //
    // Drive the I2C clock line low.
    //
    ulVal &= ~HwPortABCD_I2C_SCL;
    pulPtr[HwPortABCD >> 2] = ulVal;

    //
    // Loop through the three bytes which we will send.
    //
    for(ucIdx = 0; ucIdx < 3; ucIdx++)
    {
        //
        // Get the appropriate byte based on the current loop iteration.
        //
        if(ucIdx == 0)
        {
            ucData = ucAddr;
        }
        else if(ucIdx == 1)
        {
            ucData = ucReg;
        }
        else
        {
            ucData = ucValue;
        }

        //
        // Loop through the 8 bits in this byte.
        //
        for(ucBit = 0; ucBit < 8; ucBit++)
        {
            //
            // Set the I2C data line to correspond to the most significant bit
            // of the data byte.
            // 
            if(ucData & 0x80)
            {
                ulVal |= HwPortABCD_I2C_SDA;
            }
            else
            {
                ulVal &= ~HwPortABCD_I2C_SDA;
            }
            pulPtr[HwPortABCD >> 2] = ulVal;

            //
            // Delay to meet the I2C bus timing specification.
            //
            for(ulLoop = 0; ulLoop < 4; ulLoop++)
            {
            }

            //
            // Drive the I2C clock line high.
            //
            pulPtr[HwPortABCD >> 2] = ulVal | HwPortABCD_I2C_SCL;

            //
            // Delay to meet the I2C bus timing specification.
            //
            for(ulLoop = 0; ulLoop < 4; ulLoop++)
            {
            }

            //
            // Drive the I2C clock line low.
            //
            pulPtr[HwPortABCD >> 2] = ulVal;

            //
            // Delay to meet the I2C bus timing specification.
            //
            for(ulLoop = 0; ulLoop < 4; ulLoop++)
            {
            }

            //
            // Shift the data byte to the left by one bit.
            //
            ucData <<= 1;
        }

        //
        // We've sent the eight bits in this data byte, so we need to wait for
        // the acknowledge from the target.  Reconfigure the I2C data line as
        // an input so we can read the acknowledge from the device.
        //
        pulPtr[HwDdrABCD >> 2] ^= HwPortABCD_I2C_SDA;

        //
        // Drive the I2C clock line high.
        //
        pulPtr[HwPortABCD >> 2] = ulVal | HwPortABCD_I2C_SCL;

        //
        // Delay to meet the I2C bus timing specification.
        //
        for(ulLoop = 0; ulLoop < 4; ulLoop++)
        {
        }

        //
        // Wait until the I2C data line is pulled low by the target device.
        //
        while(pulPtr[HwPortABCD >> 2] & HwPortABCD_I2C_SDA)
        {
        }

        //
        // Drive the I2C clock line low.
        //
        pulPtr[HwPortABCD >> 2] = ulVal;

        //
        // Reconfigure the I2C data line as an output.
        //
        pulPtr[HwDdrABCD >> 2] ^= HwPortABCD_I2C_SDA;
    }

    //
    // Drive the I2C data line low.
    //
    ulVal &= ~HwPortABCD_I2C_SDA;
    pulPtr[HwPortABCD >> 2] = ulVal;

    //
    // Delay to meet the I2C bus timing specification.
    //
    for(ulLoop = 0; ulLoop < 4; ulLoop++)
    {
    }

    //
    // Drive the I2C clock line high.
    //
    ulVal |= HwPortABCD_I2C_SCL;
    pulPtr[HwPortABCD >> 2] = ulVal;

    //
    // Delay to meet the I2C bus timing specification.
    //
    for(ulLoop = 0; ulLoop < 4; ulLoop++)
    {
    }

    //
    // Drive the I2C data line high.  Since the I2C clock line is currently
    // high, this is the stop condition.
    //
    ulVal |= HwPortABCD_I2C_SDA;
    pulPtr[HwPortABCD >> 2] = ulVal;
}
#endif
