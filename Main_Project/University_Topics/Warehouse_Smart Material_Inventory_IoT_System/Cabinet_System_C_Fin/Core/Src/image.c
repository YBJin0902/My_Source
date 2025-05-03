#include "epaper.h"
#include "cmsis_os.h"

int displayX,displayY,displayDataType,displayDataValue;
char displayData,_pbc,_pcc;
//int _pbc0=0,_pbc1=1,_pbc2=2,_pbc3=3,_pbc4=4,_pbc5=5,_pbc6=6,_pbc7=7;


int A_[5] = { 0x07,0xdb,0xdd,0xdb,0x07 };
int B_[5] = { 0x7d,0x01,0x6d,0x6d,0x93 };
int C_[5] = { 0x83,0x7d,0x7d,0x7d,0xbb };
int D_[5] = { 0x7d,0x01,0x7d,0x7d,0x83 };
int E_[5] = { 0x01,0x6d,0x6d,0x6d,0x6d };
int F_[5] = { 0x01,0xed,0xed,0xed,0xfd };
int G_[5] = { 0x83,0x7d,0x7d,0x6d,0x0b };
int H_[5] = { 0x01,0xef,0xef,0xef,0x01 };
int I_[5] = { 0xff,0x7d,0x01,0x7d,0xff };
int J_[5] = { 0xbf,0x7f,0x7d,0x81,0xfd };
int K_[5] = { 0x01,0xef,0xd7,0xbb,0x7d };
int L_[5] = { 0x01,0x7f,0x7f,0x7f,0x7f };
int M_[5] = { 0x01,0xfb,0xe7,0xfb,0x01 };
int N_[5] = { 0x01,0xf7,0xef,0xdf,0x01 };
int O_[5] = { 0x83,0x7d,0x7d,0x7d,0x83 };
int P_[5] = { 0x01,0xed,0xed,0xed,0xf3 };
int Q_[5] = { 0x83,0x7d,0x5d,0xbd,0x43 };
int R_[5] = { 0x01,0xed,0xcd,0xad,0x73 };
int S_[5] = { 0xb3,0x6d,0x6d,0x6d,0x9b };
int T_[5] = { 0xfd,0xfd,0x01,0xfd,0xfd };
int U_[5] = { 0x81,0x7f,0x7f,0x7f,0x81 };
int V_[5] = { 0xc1,0xbf,0x7f,0xbf,0xc1 };
int W_[5] = { 0x81,0x7f,0x8f,0x7f,0x81 };
int X_[5] = { 0x39,0xd7,0xef,0xd7,0x39 };
int Y_[5] = { 0xf1,0xef,0x1f,0xef,0xf1 };
int Z_[5] = { 0x3d,0x5d,0x6d,0x75,0x79 };
int a_[5] = { 0xbf,0x57,0x57,0x57,0x0f };
int b_[5] = { 0x01,0x6f,0x77,0x77,0x8f };
int c_[5] = { 0x8f,0x77,0x77,0x77,0xbf };
int d_[5] = { 0x8f,0x77,0x77,0x6f,0x01 };
int e_[5] = { 0x8f,0x57,0x57,0x57,0xcf };
int f_[5] = { 0xef,0x03,0xed,0xfd,0xfb };
int g_[5] = { 0xef,0x57,0x57,0x57,0x87 };
int h_[5] = { 0x01,0xef,0xf7,0xf7,0x0f };
int i_[5] = { 0xff,0x6f,0x05,0x7f,0xff };
int j_[5] = { 0xbf,0x7f,0x77,0x85,0xff };
int k_[5] = { 0x01,0xdf,0xaf,0x77,0xff };
int l_[5] = { 0xff,0x7d,0x01,0x7f,0xff };
int m_[5] = { 0x07,0xf7,0x0f,0xf7,0x0f };
int n_[5] = { 0x07,0xef,0xf7,0xf7,0x0f };
int o_[5] = { 0x8f,0x77,0x77,0x77,0x8f };
int p_[5] = { 0x07,0xd7,0xd7,0xd7,0xef };
int q_[5] = { 0xef,0xd7,0xd7,0xd7,0x07 };
int r_[5] = { 0x07,0xef,0xf7,0xf7,0xef };
int s_[5] = { 0x6f,0x57,0x57,0x57,0xbf };
int t_[5] = { 0xfb,0x81,0x7b,0x7f,0xbf };
int u_[5] = { 0x87,0x7f,0x7f,0xbf,0x07 };
int v_[5] = { 0xc7,0xbf,0x7f,0xbf,0xc7 };
int w_[5] = { 0x87,0x7f,0x9f,0x7f,0x87 };
int x_[5] = { 0x77,0xaf,0xdf,0xaf,0x77 };
int y_[5] = { 0xe7,0x5f,0x5f,0x5f,0x87 };
int z_[5] = { 0x77,0x37,0x57,0x67,0x77 };
int _0[5] = { 0x83,0x5d,0x6d,0x75,0x83 };
int _1[5] = { 0xff,0x7b,0x01,0x7f,0xff };
int _2[5] = { 0x3b,0x5d,0x6d,0x6d,0x73 };
int _3[5] = { 0xbb,0x7d,0x6d,0x6d,0x93 };
int _4[5] = { 0xcf,0xd7,0xdb,0x01,0xdf };
int _5[5] = { 0xb1,0x75,0x75,0x75,0x8d };
int _6[5] = { 0x87,0x6b,0x6d,0x6d,0x9f };
int _7[5] = { 0xfd,0x1d,0xed,0xf5,0xf9 };
int _8[5] = { 0x93,0x6d,0x6d,0x6d,0x93 };
int _9[5] = { 0xf3,0x6d,0x6d,0xad,0xc3 };
int _colon[5] = { 0xff,0xff,0xd7,0xff,0xff };
int _[5] = { 0xff,0xff,0xff,0xff,0xff };


void epaper_partdisplay(int x, int y, char d) {
	if(d=='\n'){
		nextline();
		return;
	}
	
	//pcc=>command
	EPD_2IN9_V2_SendCommand(0x44);
	EPD_2IN9_V2_SendData((y * 16)>>3);
	EPD_2IN9_V2_SendData((y * 16 + 15)>>3);
	EPD_2IN9_V2_SendCommand(0x45);
	EPD_2IN9_V2_SendData(x * 12);
	EPD_2IN9_V2_SendData((x * 12)>>8);
	EPD_2IN9_V2_SendData(x * 12 + 10);
	EPD_2IN9_V2_SendData((x * 12 + 10)>>8);
	EPD_2IN9_V2_SendCommand(0x4E);// SET_RAM_X_ADDRESS_COUNTER
	EPD_2IN9_V2_SendData((y * 16)>>3);//Xstart
	EPD_2IN9_V2_SendCommand(0x4F);// SET_RAM_Y_ADDRESS_COUNTER
	EPD_2IN9_V2_SendData(x * 12);//Ystart
	EPD_2IN9_V2_SendData((x * 12)>>8);//Y>>8       
	EPD_2IN9_V2_SendCommand(0x24);//Transfer new data
	EPD_2IN9_V2_SendData(0xff);
	EPD_2IN9_V2_SendData(0xff);
	int loop0=0;
	while(loop0 < 5) {
		if (d == 'A')_pbc = A_[loop0];
		else if (d == 'B')_pbc = B_[loop0];
		else if (d == 'C')_pbc = C_[loop0];
		else if (d == 'D')_pbc = D_[loop0];
		else if (d == 'E')_pbc = E_[loop0];
		else if (d == 'F')_pbc = F_[loop0];
		else if (d == 'G')_pbc = G_[loop0];
		else if (d == 'H')_pbc = H_[loop0];
		else if (d == 'I')_pbc = I_[loop0];
		else if (d == 'J')_pbc = J_[loop0];
		else if (d == 'K')_pbc = K_[loop0];
		else if (d == 'L')_pbc = L_[loop0];
		else if (d == 'M')_pbc = M_[loop0];
		else if (d == 'N')_pbc = N_[loop0];
		else if (d == 'O')_pbc = O_[loop0];
		else if (d == 'P')_pbc = P_[loop0];
		else if (d == 'Q')_pbc = Q_[loop0];
		else if (d == 'R')_pbc = R_[loop0];
		else if (d == 'S')_pbc = S_[loop0];
		else if (d == 'T')_pbc = T_[loop0];
		else if (d == 'U')_pbc = U_[loop0];
		else if (d == 'V')_pbc = V_[loop0];
		else if (d == 'W')_pbc = W_[loop0];
		else if (d == 'X')_pbc = X_[loop0];
		else if (d == 'Y')_pbc = Y_[loop0];
		else if (d == 'Z')_pbc = Z_[loop0];
		else if (d == 'a')_pbc = a_[loop0];
		else if (d == 'b')_pbc = b_[loop0];
		else if (d == 'c')_pbc = c_[loop0];
		else if (d == 'd')_pbc = d_[loop0];
		else if (d == 'e')_pbc = e_[loop0];
		else if (d == 'f')_pbc = f_[loop0];
		else if (d == 'g')_pbc = g_[loop0];
		else if (d == 'h')_pbc = h_[loop0];
		else if (d == 'i')_pbc = i_[loop0];
		else if (d == 'j')_pbc = j_[loop0];
		else if (d == 'k')_pbc = k_[loop0];
		else if (d == 'l')_pbc = l_[loop0];
		else if (d == 'm')_pbc = m_[loop0];
		else if (d == 'n')_pbc = n_[loop0];
		else if (d == 'o')_pbc = o_[loop0];
		else if (d == 'p')_pbc = p_[loop0];
		else if (d == 'q')_pbc = q_[loop0];
		else if (d == 'r')_pbc = r_[loop0];
		else if (d == 's')_pbc = s_[loop0];
		else if (d == 't')_pbc = t_[loop0];
		else if (d == 'u')_pbc = u_[loop0];
		else if (d == 'v')_pbc = v_[loop0];
		else if (d == 'w')_pbc = w_[loop0];
		else if (d == 'x')_pbc = x_[loop0];
		else if (d == 'y')_pbc = y_[loop0];
		else if (d == 'z')_pbc = z_[loop0];
		else if (d == '0')_pbc = _0[loop0];
		else if (d == '1')_pbc = _1[loop0];
		else if (d == '2')_pbc = _2[loop0];
		else if (d == '3')_pbc = _3[loop0];
		else if (d == '4')_pbc = _4[loop0];
		else if (d == '5')_pbc = _5[loop0];
		else if (d == '6')_pbc = _6[loop0];
		else if (d == '7')_pbc = _7[loop0];
		else if (d == '8')_pbc = _8[loop0];
		else if (d == '9')_pbc = _9[loop0];
		else if (d == ':')_pbc = _colon[loop0];
		else if (d == ' ')_pbc = _[loop0];
		int loop1 = 1;
		while (loop1 >= 0) {
			int loop2 = 1;
			while (loop2 >= 0) {
				_pcc = 0;
				int loop3 = 1;
				while (loop3 >= 0) {
					int number = loop2 * 4 + loop3 * 2;
					if (bbyte(number + 1) == 1)_pcc += (12 * (loop3 == 0 ? 1 : 16));
					if (bbyte(number) == 1)_pcc += (3 * (loop3 == 0 ? 1 : 16));
					loop3--;
				}
				EPD_2IN9_V2_SendData(_pcc);
				loop2--;
			}
			loop1--;
		}
       	loop0++;
	}
}

int bbyte(int addr) {
    int pbc = _pbc;
    if (addr == 7) { if (pbc >= 128)return 1; else return 0; }
    if (pbc >= 128)pbc -= 128;
    if (addr == 6) { if (pbc >= 64)return 1; else return 0; }
    if (pbc >= 64)pbc -= 64;
    if (addr == 5) { if (pbc >= 32)return 1; else return 0; }
    if (pbc >= 32)pbc -= 32;
    if (addr == 4) { if (pbc >= 16)return 1; else return 0; }
    if (pbc >= 16)pbc -= 16;
    if (addr == 3) { if (pbc >= 8)return 1; else return 0; }
    if (pbc >= 8)pbc -= 8;
    if (addr == 2) { if (pbc >= 4)return 1; else return 0; }
    if (pbc >= 4)pbc -= 4;
    if (addr == 1) { if (pbc >= 2)return 1; else return 0; }
    if (pbc >= 2)pbc -= 2;
    if (addr == 0) { if (pbc >= 1)return 1; else return 0; }
    if (pbc >= 1)pbc -= 1;
    return 0;
}



void epaper_strdisplay(char *s){
	while(*s)
	{
		int X=get_displayX(),Y=get_displayY();
		epaper_partdisplay(X,Y,*s);//Max(16,5)
		s++;
	}
	//s=0;
}

/*void show_data(char *ins)
{
	for(int i =0 ; i<1 ; i++)
	{
		epaper_strdisplay(ins[i]);
	}
}*/

int get_displayX(void){
    int re=displayX++;
    return re;
}
int get_displayY(void){
    int re=displayY;
    if(displayX>19){
        --displayY;
        displayX=7;
    }
    if(displayY<0){
        displayY=7;
    }
    return re;
}

void nextline(void){
	displayX=7;
	--displayY;
}

void epaper_resetXY(void){
	displayX=7;
	displayY=7;
}
