#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const double TxPower = -59, n = 2;

using namespace std;

int main()
{
	while(1)//計算相對位置 
	{ 
		double RSSI = 0, d = 0;
		
		cin >> RSSI;
		
		//RSSI = 120 - RSSI;
		
		d = pow(10.0, ((TxPower - RSSI) / (10 * n)));
		
		cout << "Distiance : " << d << " m" << endl;
		cout << "Distiance : " << (int)(d * 100) << " cm" << endl << endl;
	}
} 
