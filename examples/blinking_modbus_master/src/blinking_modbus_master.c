/* Copyright 2014, Gustavo Muro
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \brief Blinking Modbus example source file
 **
 ** This is a mini example of the CIAA Firmware
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Examples CIAA Firmware Examples
 ** @{ */
/** \addtogroup Blinking_Modbus_master Blinking Modbus master example source file
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * MaCe         Mariano Cerdeiro
 * GMuro        Gustavo Muro
 * PR           Pablo Ridolfi
 * JuCe         Juan Cecconi
 *
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20141108 v0.0.1 GMuro   initial version
 */

/*==================[inclusions]=============================================*/
#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaModbus.h"
#include "ciaak.h"
#include "blinking_modbus_master.h"

/*==================[macros and definitions]=================================*/
#define CIAA_BLINKING_MODBUS_ID     2

#define CIAA_MODBUS_ADDRESS_INPUTS  0X0000
#define CIAA_MODBUS_ADDRESS_OUTPUS  0X0001

typedef enum
{
   CIAA_BLINKING_MOD_MAST_STATE_READING = 0,
   CIAA_BLINKING_MOD_MAST_STATE_WAITING_RESPONSE,
}ciaaBlinkingModMast_stateEnum;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
static int32_t hModbusMaster;
static int32_t hModbusAscii;
static int32_t hModbusGateway;

static uint8_t callBackData_slaveId;
static uint8_t callBackData_numFunc;
static uint8_t callBackData_exceptioncode;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** \brief Call back end of communication
 **
 ** \param[in] slaveId id modbus
 ** \param[in] numFunc number of function performed
 ** \param[in] exceptioncode exception code (0 no error)
 ** \return
 **/
static void cbEndOfComm(uint8_t slaveId, uint8_t numFunc, uint8_t exceptioncode)
{
   callBackData_slaveId = slaveId;

   callBackData_numFunc = numFunc;

   callBackData_exceptioncode = exceptioncode;

   ActivateTask(PollingSlave);
}

/*==================[external functions definition]==========================*/
/** \brief Main function
 *
 * This is the main entry point of the software.
 *
 * \returns 0
 *
 * \remarks This function never returns. Return value is only to avoid compiler
 *          warnings or errors.
 */
int main(void)
{
   /* Starts the operating system in the Application Mode 1 */
   /* This example has only one Application Mode */
   StartOS(AppMode1);

   /* StartOs shall never returns, but to avoid compiler warnings or errors
    * 0 is returned */
   return 0;
}

/** \brief Error Hook function
 *
 * This fucntion is called from the os if an os interface (API) returns an
 * error. Is for debugging proposes. If called this function triggers a
 * ShutdownOs which ends in a while(1).
 *
 * The values:
 *    OSErrorGetServiceId
 *    OSErrorGetParam1
 *    OSErrorGetParam2
 *    OSErrorGetParam3
 *    OSErrorGetRet
 *
 * will provide you the interface, the input parameters and the returned value.
 * For more details see the OSEK specification:
 * http://portal.osek-vdx.org/files/pdf/specs/os223.pdf
 *
 */
void ErrorHook(void)
{
   ciaaPOSIX_printf("ErrorHook was called\n");
   ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n", OSErrorGetServiceId(), OSErrorGetParam1(), OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());
   ShutdownOS(0);
}

/** \brief Initial task
 *
 * This task is started automatically in the application mode 1.
 */
TASK(InitTask)
{
   int32_t fdSerialPort;

   /* init the ciaa kernel */
   ciaak_start();

   fdSerialPort = ciaaPOSIX_open("/dev/serial/uart/1", O_RDWR | O_NONBLOCK);

   /* Open Modbus Master */
   hModbusMaster = ciaaModbus_masterOpen();

   /* Open Transport Modbus Ascii */
   hModbusAscii = ciaaModbus_transportOpen(
         fdSerialPort,
         CIAAMODBUS_TRANSPORT_MODE_ASCII_MASTER);

   /* Open Gateway Modbus */
   hModbusGateway = ciaaModbus_gatewayOpen();

   /* Add Modbus Slave to gateway */
   ciaaModbus_gatewayAddMaster(
         hModbusGateway,
         hModbusMaster);

   /* Add Modbus Transport to gateway */
   ciaaModbus_gatewayAddTransport(
         hModbusGateway,
         hModbusAscii);

   SetRelAlarm(ActivateModbusTask, 5, CIAA_MODBUS_TIME_BASE);

   SetRelAlarm(ActivatePollingSlaveTask, 10, 500);

   /* end InitTask */
   TerminateTask();
}

/** \brief Modbus Task
 *
 * This task is activated by the Alarm ActivateModbusTask.
 */
TASK(ModbusMaster)
{
   ciaaModbus_gatewayMainTask(hModbusGateway);

   TerminateTask();
}

/** \brief Polling Slave Task
 *
 * This task is activated by the Alarm ActivatePollingSlaveTask
 * and call back end of communication modbus master.
 */
TASK(PollingSlave)
{
   static ciaaBlinkingModMast_stateEnum state = CIAA_BLINKING_MOD_MAST_STATE_READING;
   int16_t hrValue;

   switch (state)
   {
      /* reading inputs of CIAA slave modbus */
      case CIAA_BLINKING_MOD_MAST_STATE_READING:

         /* init numFunc in invalid value */
         callBackData_numFunc = 0x00;

         /* read inputs from ciaa modbus slave */
         ciaaModbus_masterCmd0x03ReadHoldingReg(
               hModbusMaster,
               CIAA_MODBUS_ADDRESS_INPUTS,
               1,
               &hrValue,
               CIAA_BLINKING_MODBUS_ID,
               cbEndOfComm);

         /* set next state */
         state = CIAA_BLINKING_MOD_MAST_STATE_WAITING_RESPONSE;
         break;

      /* waiting for response */
      case CIAA_BLINKING_MOD_MAST_STATE_WAITING_RESPONSE:
         if (0x00 != callBackData_numFunc)
         {
            state = CIAA_BLINKING_MOD_MAST_STATE_READING;
         }
         break;

      /* TODO: write input register in to output registers  */
   }

   TerminateTask();
}


/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

