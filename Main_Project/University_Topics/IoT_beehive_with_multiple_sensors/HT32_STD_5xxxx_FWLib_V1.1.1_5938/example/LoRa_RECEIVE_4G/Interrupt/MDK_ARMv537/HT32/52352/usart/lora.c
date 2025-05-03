#include "ht32f5xxxx_gpio.h"
//#include "HT66F2390.h"
#include <stdint.h>
#include "LoRa.h"

//const unsigned char LoraSetAddr[] ={"AT+ADDRESS=43237"};			          //set address
//const unsigned char LoraSetAddrCheck[] ={"AT+ADDRESS?"};			      	  //set address check
//const unsigned char LoraSetNwID[] ={"AT+NETWORKID=7"};			      	  //set networkid，必須和接收端相同
//const unsigned char LoraSetNwIDCheck[] ={"AT+NETWORKID?"};			      //set networkid check		  
//const unsigned char LoraSetPara[] ={"AT+PARAMETER=10,7,1,7"};       	      //傳輸3km以內Command 
//const unsigned char LoraSend[]    ={"AT+SEND=43228,4,AAAA"};

const uint8_t LoraSetAddr[] ={"AT+ADDRESS=43237"};			          //set address
const uint8_t LoraSetAddrCheck[] ={"AT+ADDRESS?"};			      	  //set address check
const uint8_t LoraSetNwID[] ={"AT+NETWORKID=7"};			      	  //set networkid，必須和接收端相同
const uint8_t LoraSetNwIDCheck[] ={"AT+NETWORKID?"};			      //set networkid check		  
const uint8_t LoraSetPara[] ={"AT+PARAMETER=10,7,1,7"};       	      //傳輸3km以內Command 
const uint8_t LoraSend[]    ={"AT+SEND=43228,4,AAAA"};

int j;

void LoRa_INIT()
{	
	//Lora SetAddress
	delay(100000);
	
	printLoRa(LoraSetAddr);
	delay(100000);
					
	printLoRa(LoraSetAddrCheck);					
	delay(100000);
						
	//Lora SetNetwordID
	printLoRa(LoraSetNwID);	
	delay(100000);
				
	printLoRa(LoraSetNwIDCheck);	
	delay(100000);			
				
	//Lora SetParameter
	printLoRa(LoraSetPara);
	delay(100000);		
	
	printLoRa(LoraSetPara);
	delay(100000);	
}

void PrintLoRa(unsigned char port[])
{
	for(j=0;port[j]!='\0';j++)
	{
		_txr_rxr1 = port[j];
		while(_txif1==0);
	}
	_txr_rxr1 = 0x0D;
	while(_txif1==0);
	_txr_rxr1 = 0x0A;
	//while(_txif1==0);	
}

void printLoRa(uint8_t port[])
{
	for(j=0;port[j]!='\0';j++)
	{
		_txr_rxr1 = port[j];
		while(_txif1==0);	
	}
	_txr_rxr1 = 0x0D;
	while(_txif1==0);
	_txr_rxr1 = 0x0A;
	while(_txif1==0);	
}


