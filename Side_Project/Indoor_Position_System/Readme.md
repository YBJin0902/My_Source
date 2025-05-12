# Indoor Position System

## IPS 室內定點定位系統技術

# RSSI(Received signal strength indicator)

在無線通訊中，RSSI與距離之間的關係通常使用上述的公式來估計。這是一個基於射頻訊號在自由空間或室內中的衰減模型的簡化公式。然而，實際的環境可能會非常複雜，包括許多的干擾源和阻擋物，這些都可能對RSSI值產生影響，使得這個公式的準確性受到限制。

</br>

```
RSSI  接收信號強度指示(信號、指標)
衡量設備從接收點接收信號能力強度

公式：d = 10^((Tx Power - RSSI) / (10 * n))
    
     d -> 距離
     Tx Power -> 傳輸功率，在一公使距離下的RSSI
     RSSI -> 接收訊號強度指標
     n -> 環境衰減因子，通常在 2(自由空間)或 4(建築物內)之間
```

</br>

在實際的應用中，可能需要使用更複雜的模型來描述RSSI與距離之間的關係，例如考慮到多路徑效應（multipath effect）和陰影效應（shadowing effect）。這可能需要收集大量的實驗數據，並使用機器學習或者其他的統計方法來建立模型。

<font color="#f00">本次研究是使用三角測量法，以提高距離估計的準確性。</font>

另一種可能的方法是使用卡爾曼濾波器（Kalman Filter）或者其他的濾波器來平滑RSSI值，減少隨機變動的影響。

何謂 "TxPower"，也就是傳輸功率，這是一個固定的參數，表示在一公尺的距離下接收訊號強度指標(RSSI)。這是由廠商的Datasheet中所提供。

何謂 "n"，也就是環境參數因子，也被稱為路徑損耗指數（Path Loss Exponent）。這個參數描述了無線訊號隨著距離增加而衰減的速度。n 的值通常取決於環境的特性，例如是否有阻擋物、空氣的濕度等。

在自由空間中，n 的值通常為 2。這是因為在無阻擋物的情況下，無線訊號的強度通常與距離的平方成反比。然而，在建築物內或者其他有阻擋物的環境中，n 的值可能會增大，通常在 2 到 4 之間。

若無法確定 n 的值，也可以通過實驗來估計。方法是在不同的距離測量 RSSI 值，然後用這些數據來擬合上述的公式，求出最佳的 n 值。

## RSSI 與 dBm 之間的關係

1.RSSI為一種指標，訊號強度的指標
2.dBm 是 RSSI 的單位，為一種絕對數值
3.dBm 數值越小代表接收信號強度越好

## 三角測量法

三點測量法，也被稱為三角定位法或三角測量法，是一種利用三個已知位置的點來確定一個未知位置的方法。在無線通訊中，這種方法可以用來估計設備的位置。

</br>

```
假設我們有三個已知的參考點
分別是 (x1, y1)、(x2, y2) 和 (x3, y3)
並且我們知道目標點到這三個參考點的距離分別是 d1、d2 和 d3。
我們可以設目標點的坐標為 (x, y)，然後解以下的方程組來求出 (x, y)：

    (x - x1)^2 + (y - y1)^2 = d1^2
    (x - x2)^2 + (y - y2)^2 = d2^2
    (x - x3)^2 + (y - y3)^2 = d3^2
    
這是一個非線性的方程組，可能需要使用數值方法來求解，例如牛頓法或者梯度下降法。
```

</br>

請注意，這種方法的準確性可能會受到許多因素的影響，包括訊號干擾、物體阻擋、以及環境中的其他無線訊號。因此，這只是一種基本的估計方法，並不能保證絕對的準確性。在實際的應用中，可能需要配合其他的技術，例如RSSI測量或者指紋定位，以提高位置估計的準確性。

</br>

# Wi-Fi fingerprint

**Wi-Fi fingerprint -> 在室內的一個位置各個 Wi-Fi 的信號前度向量**

Ex. 在一個室內中，部屬 5 個 AP(Access Point)，則在室內的一個位置 "Pos1"，可以收到這 5 個 AP 的信號強度，可記為 RSS1、RSS2、RSS3、RSS4、RSS5，這樣就可以構成一個信號強度的向量，則位置 Pos1 的 Fingerprint 就是 : Pos1(RSS1、RSS2、RSS3、RSS4、RSS5)。
```
用於室內定點定位中 -> 
1.紀錄每個位置的 Fingerprint
2.根據得到的 RSS 先區分簡單的相對位置
3.利用 Fingerprint 依次比較，比較不同的度量函數，看與哪個 Fingerprint 最接近就可以定位了
```

Fingerprint 分為兩個階段，離線映射階段與線上定位階段：
1.離線映射階段：收集每個 Wi-Fi Reference Point(RP, 參考點)的 Wi-Fi fingerprint，建立無線電地圖，定將其存入資料庫中。
2.線上定位階段：藉由資料庫中的值，匹配與估計當前DUT的位置。

無線電地圖 -> 架設有 $_i$ 個 RP 、 $_j$ 個 Wi-Fi AP，那地圖的每個"點"就會是($p_i$ , $r_i$)，那 $p_i$ 就會是 RP 在 2D 中的座標，$r_i$ = ( $r_{i1}$ , $r_{i2}$ , $r_{i3}$ ,... , $r_{ij}$ )，這是第 $_i$ RP的 RSSI ，可以將 $r_{ij}$ 當作第 $_j$ 個的 AP 的第 $_i$ 個 RP 的 RSSI 。

</br>

# Calculate WPS(Wi-Fi Positioning System) by ESP

利用 Wi-Fi 無線連線計算所有裝置的相對位置，此想法是基於 IPS(Indoor Position System) 中的其中一種計算方式。

首先透過將所有<font color="#f00">**支點 banch point**</font> 連線至<font color="#f00">**主點 main point**</font>，再讓所有支點裝置讀取跟主點之間的==接收訊號強度指示(RSSI)數據==。

將支點所接收到的數據回傳至 Server，在透過 Server 設計好的演算法與公式計算出所有裝置的相對位置。

## Firmware implementation

以 STM32F103 實作，並搭配 ESP-12s (或其他 ESP 系列皆可) 使用，各裝置關係架構圖如下。

</br>

![image](https://hackmd.io/_uploads/rJxQv8hLa.png)

</br>


### **主點 Main Point** (MP)

設定為 access point mode + station mode，將其稱之為 "**主點 main point**"。

```
AT+CWMODE=3
```

</br>

![image](https://hackmd.io/_uploads/ByXOXpsXR.png)

</br>

在切換 Wi-Fi Mode 時，原本連線的 Wi-Fi 不會斷開連線。

在切換完成之後根據 access point mode 的特性分享自身網路。

```
AT+CWSAP="SSID","password",<channel ID>,<ecn>

AT+CWSAP="ESP_MP","00000000",1,4\r\n
```

</br>

![image](https://hackmd.io/_uploads/BJxJrpimR.png)

</br>

###  **支點 Branch Point** (BP)

設定為 station mode ，將其稱之為"**支點 banch point**"

```
AT+CWMODE=1
```

與 MP 主點連線

```
AT+CWJAP="SSID","password"

AT+CWJAP="ESP_MP","00000000"\r\n
```

查看 RSSI，RSSI 單位為 dBm

```
AT+CWJAP?

+CWJAP:<ssid>, <bssid>, <channel>, <rssi>

Note : 取第三個透號之後之值!!
```

</br>

![image](https://hackmd.io/_uploads/SyYANlqXA.png)

</br>

更換 Wi-Fi 之後 Server 端會斷開連線


---

在 MP 主點有 BP 支點連線時會返回以下資訊

</br>

![image](https://hackmd.io/_uploads/r1btFw9QC.png)

</br>

```
STA_CONNECTED : 有 BP 連線
    
DIST_STA_IP : 給予 BP IP Address    

STA_DISCONNECTED : 有 BP 斷開連線
```

</br>

## Software implementation

將dBm值轉換成距離 

```cpp!

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
```

</br>

# Overall Concept

將轉換好的矩陣回傳給演算法即可。

</br>

![image](https://hackmd.io/_uploads/SkneGbO5T.png)

</br>

# IPS Flow-Chart


### Firmware flow chart

</br>

![IPS - Firmware - FlowChart](https://hackmd.io/_uploads/BydCspjXA.png)

</br>


### Software flow chart (Server)

</br>

![IPS - Software - FlowChart](https://hackmd.io/_uploads/rkxUT6s7A.png)

</br>

# Reference

- [ESP at command](<https://www.espressif.com/sites/default/files/4a-esp8266_at_instruction_set_en_v1.5.4_0.pdf> "Title")
    
- [How to Use the ESP32 Station and Access Point Mode at the Same Time](<https://linuxhint.com/esp32-both-access-station-points/> "Title")
    
- [ESP8266 WiFi Bee: Setting an Access Point with AT commands](<https://techtutorialsx.com/2017/05/20/esp8266-wifi-bee-setting-an-access-point-with-at-commands/> "Title")
    
- [Indoor Positioning, Tracking and Indoor Navigation with Wi-Fi](<https://www.infsoft.com/basics/positioning-technologies/wi-fi/> "Title")

- [Wi-Fi positioning system Wiki](<https://en.wikipedia.org/wiki/Wi-Fi_positioning_system#Signal_strength_based> "Title")
    
- [Received signal strength indicator Wiki](<https://en.wikipedia.org/wiki/Received_signal_strength_indicator> "Title")

- [IoT 的基礎知識 - RSSI](<https://peterpowerfullife.com/blog/tech-rssi/> "Title")

- [Trilateration](<https://en.wikipedia.org/wiki/Trilateration> "Title")

