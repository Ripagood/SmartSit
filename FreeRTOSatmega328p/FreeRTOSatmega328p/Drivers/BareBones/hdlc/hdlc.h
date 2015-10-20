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

#ifndef DN_INCL_HDLC_H
#define DN_INCL_HDLC_H

/*
 * HDLC encode and decode operations
 *
 * The encode operation takes an input buffer representing a single data
 * packet and returns an HDLC frame with special characters escaped in the
 * output buffer.
 * 
 * The decode operation takes a buffer as input and decodes the first complete
 * HDLC packet. If a complete packet (two framing bytes) was found, the decode
 * operation sets the input length argument to the length of data read. The
 * input data is expected to contain framing bytes at the start and end of the
 * packet.
 *
 */

#include "dn_typedef.h"

#define HDLC_FRAME_BYTE     0x7E
#define HDLC_ESCAPE_BYTE    0x7D

#define HDLC_ERR_CRC     -1
#define HDLC_ERR_FRAME   -2
#define HDLC_ERR_LEN     -3

/**
 * Encode an input buffer into an HDLC frame, 
 * performs byte stuffing and adds CRC
 * 
 * @param   pOut:  destination pointer for encoded bytes
 * @param   pIn:   pointer to unencoded source data
 * @param   inLen:     Length of input data
 * @param   maxOutLen: Maximum output length  
 * 
 * @return  INT32S Number of bytes written to destination
 */
INT32S hdlc_encode(INT8U* pOut, INT8U* pIn, INT32U inLen, INT32U maxOutLen);

/**
 * Decode an HDLC packet into destination (strip framing and CRC)
 *
 * @param  pOut:   destination to write decoded data
 * @param  pIn:    pointer to HDLC data to decode
 * @param  inLen:  Length of input HDLC data
 *                 (output) length of input data read
 * @param  maxOutLen: Maximum output length  
 *
 * @return INT32S length of decoded data, or error code
 */
INT32S hdlc_decode(INT8U* pOut, INT8U* pIn, INT8U* inLen, INT32U maxOutLen);
INT32S hdlc_encode_D(INT8U* pOut, INT8U* pIn, INT32U inLen, INT32U maxOutLen);
#endif
