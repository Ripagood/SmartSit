/*
  Copyright (c) 2012, Dust Networks 
  All rights reserved. 
  
  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are met: 
    * Redistributions of source code must retain the above copyright 
      notice, this list of conditions and the following disclaimer. 
    * Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution. 
    * Neither the name of Dust Networks nor the 
      names of its contributors may be used to endorse or promote products 
      derived from this software without specific prior written permission. 
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL DUST NETWORKS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "hdlc.h"
#include "fcs.h"
#include <avr/pgmspace.h>



/*
 * Decode an HDLC packet into destination (strip framing and CRC)
 * Arguments:
 *    pOut:      Destination to write decoded data
 *    pIn:       HDLC data to decode
 *    inLen:     Length of input HDLC data
 *    maxOutLen: Max output length  
 * Returns:
 *    > 0: length of decoded data
 *    < 0: error code
 */
INT32S hdlc_decode(INT8U* pOut, INT8U* pIn, INT8U* inLen, INT32U maxOutLen)
{
   INT32U i;
   INT32U wIdx = 0;
   INT16U calcCrc = FCS_INITIAL_FCS16;
   INT8U fOpenFlag = 0;
   INT8U fDataStart = 0;
   INT8U curByte;
   INT8U fEscape = 0;
   
   for (i = 0; i < *inLen; i++) {
      curByte = pIn[i];
      if (curByte == HDLC_FRAME_BYTE) {
         if (!fOpenFlag) {
            fOpenFlag = 1;
         }
         else if (fDataStart) {
            // output amount of input read
            *inLen = i+1;
            if (calcCrc == FCS_GOOD_FCS16) {
               // return length excluding CRC bytes
               return (wIdx - 2);
            }
            else {
               return HDLC_ERR_CRC;
            }
         }
      }
      else if (curByte == HDLC_ESCAPE_BYTE) {
         fEscape = 1;
      }
      else {
         if (fOpenFlag) {
            fDataStart = 1;
            if (fEscape) {
               fEscape = 0;
               pOut[wIdx] = (curByte ^ 0x20);
            }
            else {
               pOut[wIdx] = curByte;
            }
            if (wIdx > maxOutLen) {
               return HDLC_ERR_LEN;
            }
            calcCrc = fcs_fcs16(calcCrc, pOut[wIdx]);
            wIdx++;
         }
      }
   }
   return HDLC_ERR_FRAME;
}

/*
 * HDLC stuff byte into destination buffer,
 * inserts escape bytes where appropriate
 * Arguments:
 *    pDest:  destination pointer for encoded bytes
 *    byte:   byte to encode
 * Returns:
 *    Number of bytes written to destination (1 or 2 if escaped) 
 */
INT8U hdlc_stuffByte(INT8U* pDest, INT8U byte)
{
   if (byte == HDLC_FRAME_BYTE || byte == HDLC_ESCAPE_BYTE) {
      *pDest = HDLC_ESCAPE_BYTE;
      *(pDest+1) = byte ^ 0x20;
      return 2;
   }
   else {
      *pDest = byte;
      return 1;
   }
}

/*
 * Encode an input buffer into an HDLC frame, 
 * performs byte stuffing and adds CRC
 * Arguments:
 *    pOut:  destination pointer for encoded bytes
 *    pIn:   pointer to unencoded source data
 *    inLen:     Length of input data
 *    maxOutLen: Max output length  
 * Returns:
 *    Number of bytes written to destination 
 */
INT32S hdlc_encode_D(INT8U* pOut, INT8U* pIn, INT32U inLen, INT32U maxOutLen)
{
   INT16U i;
   INT16U wIdx = 0;
   INT16U txFcs = 0;
   INT8U fcsHi, fcsLo;

   // calculate the FCS
   txFcs = fcs_calcFcs_D(pIn, inLen);

   // stuff input into output buffer, escaping framing characters in input
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   for (i = 0; i < inLen; i++) {
      wIdx += hdlc_stuffByte(&pOut[wIdx], pIn[i]);
	  //wIdx += hdlc_stuffByte(&pOut[wIdx], pgm_read_byte(pIn[i]));
      if (wIdx > (maxOutLen - 4)) {
         return HDLC_ERR_LEN; 
      }
   }
   
   // append CRC
   fcsHi = (txFcs & 0xFF);
   fcsLo = ((txFcs >> 8) & 0xFF);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsHi);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsLo);
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   return wIdx;
}


/*USES PGM_READ_BYTE
 * Encode an input buffer into an HDLC frame, 
 * performs byte stuffing and adds CRC
 * Arguments:
 *    pOut:  destination pointer for encoded bytes
 *    pIn:   pointer to unencoded source data
 *    inLen:     Length of input data
 *    maxOutLen: Max output length  
 * Returns:
 *    Number of bytes written to destination 
 */
INT32S hdlc_encode(INT8U* pOut, INT8U* pIn, INT32U inLen, INT32U maxOutLen)
{
   INT16U i;
   INT16U wIdx = 0;
   INT16U txFcs = 0;
   INT8U fcsHi, fcsLo;

   // calculate the FCS
   txFcs = fcs_calcFcs(pIn, inLen);

   // stuff input into output buffer, escaping framing characters in input
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   for (i = 0; i < inLen; i++) {
     // wIdx += hdlc_stuffByte(&pOut[wIdx], pIn[i]);
	  wIdx += hdlc_stuffByte(&pOut[wIdx], pgm_read_byte(&pIn[i]));
      if (wIdx > (maxOutLen - 4)) {
         return HDLC_ERR_LEN; 
      }
   }
   
   // append CRC
   fcsHi = (txFcs & 0xFF);
   fcsLo = ((txFcs >> 8) & 0xFF);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsHi);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsLo);
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   return wIdx;
}





