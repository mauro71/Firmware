/* Copyright 2014, Mariano Cerdeiro
 * Copyright 2014, Pablo Ridolfi
 * Copyright 2014, Juan Cecconi
 * Copyright 2014, Gustavo Muro
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

/** \brief teclas source file
 **
 ** This is a mini example of the CIAA Firmware.
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Examples CIAA Firmware Examples
 ** @{ */
/** \addtogroup Blinking Blinking_echo example source file
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * PS           Pablo Solivellas
 * HP           Hernan Ponso
 * DP           Damian Primo
 *
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20151201 v0.0.1   PS   first functional version
 */

/*==================[inclusions]=============================================*/
#include "myFunctions.h"           /* <= own header */

/*==================[macros and definitions]=================================*/
//#define TECLADO_TOTAL_TECLAS     4

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/** \brief File descriptor for digital input ports
 *
 * Device path /dev/dio/in/0
 */
//static int32_t fd_in;

//static uint8_t estadoTeclas;
//static uint8_t teclasFlancoUP;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void initMyVariables(void)
{
   tiltLed = 0x20;  //0B10000;

   tiltFrec = 500;

   tiltCounter = 0;

   //Write the led register
   ciaaPOSIX_write(fd_out, &tiltLed, 1);
}


void procesarTeclas(uint8_t teclas)
{
   GetResource(BLOCK);

   uint8_t outputs, tiltLedTemp;
   uint32_t ui32Loop;

   // si se oprime la tecla parpadea el led
   if (TECLADO_TEC1_BIT & teclas)
   {

	   if (tiltLed>0x01)
	  	   tiltLed = tiltLed >> 1;
	   else
	   {
	  	   tiltLed = 0x20;
	   }

      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);
   }

   if (TECLADO_TEC2_BIT & teclas)
   {
	   if (tiltLed<0x20)
	 	  	   tiltLed = tiltLed << 1;
	 	   else
	 	   {
	 	  	   tiltLed = 0x01;
	 	   }
      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);
   }


   // si se oprime la tecla 4 Decrementa la frecuencia de parpade0 del led
   if (TECLADO_TEC3_BIT & teclas)
   {
	   if (tiltFrec>100)
		   tiltFrec = tiltFrec-100;
	   else
	   {
		   tiltLedTemp = tiltLed;
		   tiltLed = EDU_CIAA_NXP_RGB_BLANCO;
		   tiltFrec = 100;


      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);


      for(ui32Loop = 0; ui32Loop < 90000; ui32Loop++)
             {
             }
      tiltLed = tiltLedTemp;
      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);

       }
   }

   // Led blinking frecuency increment
   if (TECLADO_TEC4_BIT & teclas)
   {

	   if (tiltFrec<1000)
		   tiltFrec = tiltFrec+100;
	   else
	   {
		   tiltLedTemp = tiltLed;
		   tiltLed = EDU_CIAA_NXP_RGB_BLANCO;
		   tiltFrec = 1000;

      
      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);

      for(ui32Loop = 0; ui32Loop < 90000; ui32Loop++)
             {
             }
      tiltLed = tiltLedTemp;
      // Writhe the new state to the register
      ciaaPOSIX_write(fd_out, &tiltLed, 1);
       }

   }

   ReleaseResource(BLOCK);
}




/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

