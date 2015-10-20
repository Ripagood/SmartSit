/***
BareBones WHT V1.0
Created 18/09/2015 00:00 AM
Author: Elias Ventura Hernandez E.
****/


#include "BareBonesWHT.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "semphr.h"

// constants for events and acks
// These are kept on the RAM for easier porting, otherwise store them in flash

static const uint8_t bootEvent[] PROGMEM ={0x0F,0x09,0x08,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00};//ya tiene el sync flag

//const uint8_t bootEventP[] PROGMEM={0x0F,0x09,0x08,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00};//ya tiene el sync flag

static const uint8_t eventACK[] PROGMEM ={0x0F,0x00,0x05,0x00};//

//const uint8_t eventACKP[] PROGMEM={0x0F,0x00,0x05,0x00};//

const uint8_t bootEvent1[] PROGMEM ={0x0F,0x09,0x0A,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00};

const uint8_t joinCommand[] PROGMEM ={0x06,0x00,0x04};//ignore packetid, request
const uint8_t joinACK[] PROGMEM ={0x06,0x00,0x05,0x00};//ignore packet id, response

const uint8_t moteSearchingEvent[] PROGMEM ={0x0F,0x09,0x04,0x00,0x00,0x00,0x02,0x02,0x00,0x00,0x00,0x08};
//mandar un eventACK

const uint8_t negotiatingEvent[] PROGMEM ={0x0f,0x09,0x04,0x00,0x00,0x01,0x00,0x03,0x00,0x00,0x00,0x08};
//mandar un event ACK

const uint8_t alarmEvent[] PROGMEM ={0x0f,0x09,0x04,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x00,0x00};
//mandar un eventACK
const uint8_t operationalEvent[] PROGMEM ={0x0f,0x09,0x02,0x00,0x00,0x00,0x24,0x05,0x00,0x00,0x00,0x00};
//mandar un eventACK
const uint8_t svcIndication[] PROGMEM ={0x0e,0x0c,0x04,0x00,0x00,0x80,0x01,0x03,0x02,0xf9,0x81,0x00,0x00,0x03,0xe8};
const uint8_t serviceACK[] PROGMEM ={0x0e,0x00,0x05,0x00};


//const uint8_t setService[]={0x01,0x0a,0x05,0x08,0x01,0x01,0x00,0xf9,0x81,0x00,0x00,0x01,0x2c};//request
	//REMEMBER TIME ASKED FOR = 012C = 300ms
const uint8_t setService[] PROGMEM ={0x01,0x0a,0x05,0x08,serviceID,0x01,0x00,0xf9,0x81,0x00,0x00,0x01,0x2c};//request

const uint8_t setServiceACK[] PROGMEM ={0x01,0x02,0x00,0x00,0x08,0x01};

const uint8_t sendComand[]={0x05,0x0d,0x04,0xf9,0x81,0x01,0x00,0x02,0xff,0xff,0xff,0x04,0xb1,0x6b,0x00,0xb5};
const uint8_t sendACK[] PROGMEM={0x05,0x00,0x05,0x00};
/*
05 = send
f9 81 = address
01 = serviceId
00 = app domain
02 = prioridad
ff ff = reservado
seq num = ff
04 paylod length
b16b00b5 = payload
*/

extern SemaphoreHandle_t frameHandle;



uint8_t InterruptReception (MoteData_p moteData) {
//Place this function inside your Serial Receipt Interrupt
//Dont read the byte, the function serialMoteReceive will do it

moteData->RXbuffer[moteData->RXindex]=moteData->serialMoteReceive();
//moteData->RXbuffer[moteData->RXindex]=rxedChar;
	moteData->RXindex++;
	//serialMoteSend(moteData->RXbuffer[moteData->RXindex]);
	//sei();
	
	//serialPCtx(moteData->RXbuffer[moteData->RXindex-1]);
	
	
	if ((moteData->RXbuffer[moteData->RXindex-1] == 0x7E) && ((moteData->RXindex-1) > 0) )
	{
		//serialPCtxArray(moteData->RXbuffer,moteData->RXindex);
	
		//moteData->RXindex++;
		//serialPCtxArray(moteData->RXbuffer,moteData->RXindex);
	 moteData->decodeResult = hdlc_decode(moteData->hdlcDecoded, moteData->RXbuffer,&moteData->RXindex, sizeof(moteData->hdlcDecoded));
		if (moteData->decodeResult>0)
		{
			moteData->validFrame=1;
			//serialPCtxArray(moteData->hdlcDecoded,moteData->decodeResult);
		}
		moteData->RXindex=0;
		return moteData->validFrame;
	}else{
		moteData->validFrame=0;
		return moteData->validFrame;}//moteData->RXindex++;}


}




void MoteInitialization( MoteData_p moteData){

while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	moteData->hdlcDecoded[2]=0x08;
	if (compareBuffer(moteData->hdlcDecoded,bootEvent,moteData->decodeResult))
	{
		//serialPCtx(0x00);
		
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,eventACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtxArray(moteData->hdlcOutput,moteData->sizeToSend);
	
	}
	//_delay_ms(100);
	moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,joinCommand,sizeof(joinCommand),sizeof(moteData->hdlcOutput));
	serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
	
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,joinACK,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,joinACK,sizeof(joinACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x01);
	}
	
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,moteSearchingEvent,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,eventACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x02);
	}
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,negotiatingEvent,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,eventACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x03);
	}
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,alarmEvent,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,eventACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x04);
		
	}
	
	while(xSemaphoreTake(frameHandle,0xFFFF) !=  pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,operationalEvent,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,eventACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x05);
		
	}
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,svcIndication,moteData->decodeResult))
	{
		moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,serviceACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
		serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
		//serialPCtx(0x06);
	}else
	{
		//return to state machine
	}


}

void MoteSvcInitialization( MoteData_p moteData){

// requesting service
	
	moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,setService,sizeof(setService),sizeof(moteData->hdlcOutput));
	serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
	
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer(moteData->hdlcDecoded,setServiceACK,moteData->decodeResult))
	{
		//serialPCtx(0x00);// RC = OK
	}else
	{
		//serialPCtx(0xFF); // RC = NOT OK
	}
	
	uint8_t serviceReady=0;
	while (!serviceReady)
	{
		while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
		moteData->validFrame=0;
		//en este punto esperamos un svcIndication con el tiempo pedido, puede llegar uno de delayed response
		//asi que todos los respondes con el serviceACK y checamos si se nos otorgo el tiempo
			moteData->sizeToSend=(int8_t)hdlc_encode(moteData->hdlcOutput,serviceACK,sizeof(eventACK),sizeof(moteData->hdlcOutput));
			serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
			
		if (moteData->hdlcDecoded[3]==0x03)//delayed response
		{
			serviceReady=0;
		}
		else{
			if (moteData->hdlcDecoded[3]==0x00)//created
			{
				serviceReady=1;
				//serialPCtx(0xF0);
			}
		}
	}
}



void serialMoteSendArray(uint8_t* datos, uint8_t tam, MoteData_p moteData){
	//send many bytes
	for (uint8_t i=0;i<tam;i++)
	{
		//serialMoteSend(datos[i]);
		moteData->serialMoteSend(datos[i]);
	}
	
}

uint8_t sendData(uint8_t* data, uint8_t tam, uint8_t svcId, MoteData_p moteData){
	
	
	moteData->sizeToSend=0;
	uint8_t newTam = 12+tam;
	//uint8_t* arr;
	static uint8_t arr[BUFFER_LENGHT];
	//arr = (uint8_t*)malloc(newTam);
	memcpy(arr,sendComand,11);
	arr[1]=newTam-3;//menos el header
	arr[11]=tam;//tanaio del payload
	memcpy(&arr[12],data,tam);
	arr[5]=svcId;
	moteData->sizeToSend=(int8_t)hdlc_encode_D(moteData->hdlcOutput,arr,newTam,sizeof(moteData->hdlcOutput));
	serialMoteSendArray(moteData->hdlcOutput,moteData->sizeToSend,moteData);
	//free(arr);
	while(xSemaphoreTake(frameHandle,0xFFFF) != pdTRUE);
	moteData->validFrame=0;
	if (compareBuffer_D(moteData->hdlcDecoded,sendACK,moteData->decodeResult))
	{
		return 1;//send = OK
	}
	return 0;//send = NOT OK!
	
	
	
	
	
}

uint8_t compareBuffer(uint8_t* datos, uint8_t*datos2,uint8_t tam){
	uint8_t comparacion=0;
	for (uint8_t i=0;i<tam;i++)
	{
		//if (datos[i]==datos2[i])
		if (datos[i]==pgm_read_byte(&(datos2[i])))
		{
			comparacion++;
		}
	}
	
	if (comparacion>=tam)
	{
		return 1;
	}else return 0;
	
	
	
}


uint8_t compareBuffer_D(uint8_t* datos, uint8_t*datos2,uint8_t tam){
	uint8_t comparacion=0;
	for (uint8_t i=0;i<tam;i++)
	{
		//if (datos[i]==datos2[i])
		if (datos[i]==(datos2[i]))
		{
			comparacion++;
		}
	}
	
	if (comparacion>=tam)
	{
		return 1;
	}else return 0;
	
	
	
}





