#include "gps.h"

u8 rxgps[256];
u8 gpsTempData[128];
char ctemp[128];

void gpscheck(void){
	u32 Len;
	char *tempt2;
	char ugps[] = "$GPRMC";
	u8 ps[10];
	bool rxget = true;
		//UARTM_WriteByte(UARTM_CH0, 'B');
		

	while(1){
		
		
		Usart_Sendbyte_gps(COM3_PORT,'A');
		//Usart_Sendbyte_gps(HTCFG_UART_PORT,'A');
		printf("gps\n");
		if(rxget){
			while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
			rxgps[0] = USART_ReceiveData(COM3_PORT);
			//Usart_Sendbyte_gps(COM3_PORT,rxgps[0]);
			
			if(rxgps[0] == '$')gpshead();
			if(rxgps[3] == 'R'){
				for(Len = 6 ;Len < 80;Len++){
					while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
					rxgps[Len] = USART_ReceiveData(COM3_PORT);
					Usart_Sendbyte_gps(COM3_PORT,rxgps[Len]);
				}
			}
			if((tempt2 = strstr((char*)rxgps,ugps))){
				//printf("tempt2 = %c\n",tempt2[count]);
				int lentemp2 = strlen(tempt2);
				//printf("lentemp2 = %d\n",lentemp2);
				gpsinfo(lentemp2,tempt2);
				//printf("132\n");
				//UARTM_Write(UARTM_CH0, Test, 5);		
				memset(rxgps,'\0',255);		
				rxget = false;				
			}
		}
		if(!rxget)break;
	}
}

void gpsinfo(const int lentemp2 , const char *tempt2){
	int count , cma = 0 , templen[20] = {0};
	for(count = 0;count<lentemp2;count++){
		bool ngpsdata = true;
		if(tempt2[count] == ','){
			templen[cma] = count;
			//printf("templen[%d] = %d\n",cma,count);
			cma++;
			switch(cma){
				case 2:{
					int lencma,itempT,utc = 8;
					char ctempT[20];
					printf("time:");
					for(lencma = templen[0]+1;lencma < templen[1];lencma++){
						ctempT[lencma - (templen[0]+1)] = tempt2[lencma];
					}
					itempT = atoi(ctempT) + (utc * 10000);
					gpsint_to_u8(itempT);
					Usart_SendStr(COM3_PORT, gpsTempData);
					//printf("%d",itempT);
					printf("\n");
					}break;
				case 3:{
					char ctempT[20];
					ctempT[0] = tempt2[templen[1]+1];
					//printf("%c",ctempT[0]);
					if(ctempT[0] != 'A')ngpsdata = false;
				}break;
				case 4:{	//N
					int lencma,itempN,fcount = 1,icount = 0;
					char ctempN[20];
					float ftempN;
					printf("%c:",tempt2[templen[3]+1]);
					for(lencma = templen[2]+1;lencma < templen[3];lencma++){
						ctempN[lencma - (templen[2]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempN = atof(ctempN);
					itempN = ftempN / 100;
					ftempN = ((((ftempN/100) - itempN)/60)*100)+itempN;// ftempN = 2342.12541,itempN = 23
					itempN = ftempN * fcount * 10;
					gpsint_to_u8(itempN);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[1]);
					if(gpsTempData[icount]){
						Usart_Sendbyte_gps(COM3_PORT,'.');
						for(icount = 2;gpsTempData[icount]!=NULL;icount++){
						Usart_Sendbyte_gps(COM3_PORT,gpsTempData[icount]);
						}
					}
					
					printf("\n");
					}break;
				case 6:{	//E
					int lencma,itempE,fcount = 1,icount = 0;
					char ctempE[20];
					float ftempE;
					printf("%c:",tempt2[templen[5]+1]);
					for(lencma = templen[4]+1;lencma < templen[5];lencma++){
						ctempE[lencma - (templen[4]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempE = atof(ctempE);
					itempE = ftempE / 100;
					ftempE = ((((ftempE/100) - itempE)/60)*100)+itempE;
					itempE = ftempE * fcount*10;
					gpsint_to_u8(itempE);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[1]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[2]);
					if(gpsTempData[icount]){
						Usart_Sendbyte_gps(COM3_PORT,'.');
						for(icount = 3;gpsTempData[icount]!=NULL;icount++){
							Usart_Sendbyte_gps(COM3_PORT,gpsTempData[icount]);
						}
					}
					//printf("%f",ftempE);
					printf("\n");
					}break;
				default:break;
			}
		}
		if(ngpsdata == 0)break;
	}
		printf("break\n");
}

void gpshead(){
	int n;
	for(n = 1 ;n < 6;n++){
				while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
				rxgps[n] = USART_ReceiveData(COM3_PORT);
				//Usart_Sendbyte_gps(COM3_PORT,rxgps[n]);
			}
	//Usart_SendStr(COM3_PORT, rxgps);
	//Usart_Sendbyte_gps(COM3_PORT,'\n');
}

void gpsint_to_u8 (int te){
	memset(gpsTempData,'\0',128);
	int nn = 0;
	sprintf(ctemp,"%d",te); //int te
	size_t length = strlen(ctemp);
	for(nn = 0;nn<length;nn++)gpsTempData[nn] = ctemp[nn]; //u8 gpsTempData[tsize]
}