# Bera-Metal Book

不以任何開發版、單晶片為例純粹講述觀念與各語法知識。

須注意不同的編譯器有不同的語法與設定需要注意。
像是 Keil 不是使用 Makefile 與 Linker Script 而是其他設定檔。本系列會以 Ecipse 為主要編譯器。

### 章節
#### Chapter 1 Bare-Matel 概述
#### Chapter 2 Makefile
#### Chapter 3 Linker Script
#### Chapter 4 Startup code
#### Chapter 5 System Initialization

</br>

---

</br>

# Chapter 1. Bare-Matel 概述

甚麼是 Bare-Matel（俗稱裸機），也就是在沒有任何資源的情況下進行晶片的開發（這裡的晶片不限於 MCU SoC 等）。

拿 STM32 開發來舉例，我們可以使用 STM32CubeMX 利用 UI 介面對想要的功能直接點選後生成 Project，整份專案就會直接出現，這就是最簡單的例子，在生成好的專案中我們只需要撰寫 main 即可，其實在很多廠商提供的 Example 中我們都可以直接這樣做，因為他們已經幫我們做好了很多前置工作，像是：

1. 周邊的設計（Drivers I/O）
2. C code 與硬體之間的溝通、控制
3. 記憶體的連結
4. 專案的編譯規則
5. 最重要的 Toolchain

</br>

接下來，請在有基本韌體知識的情況下再開始學習！

</br>

### 第一步，知道程式放在哪裡

我們都知道要讓晶片可以動起來，最基本的就是要把我們寫好精美的程式燒錄進晶片中。

程式會被燒錄進 Flash 記憶體中。當 MCU 開機（如 POR）時，處理器從 Flash 開始抓取指令執行。某些特定資料（如 .data 區段）會從 Flash 搬到 RAM，而大部分的程式碼（.text 區段）是直接在 Flash 執行的。


那我們燒錄的是甚麼，最常聽到的其實就是 Program Image 也就是在我們編譯好整份專案之後所產出來的檔案，檔案很可能是 .elf .bin 或是 .hex 都有可能，那編譯的部分是 Makefile 的工作並不是這裡要贅述的部分（後續會有章節專門介紹）。

</br>

假設在產品的 Datasheet，我們可以看到有關於 Flash、SRAM 的位置：

| Memory Name| Start Address | End Address |
|:---:|:---:|:---:|
| Flash | 0x0800_0000 | 0x2000_0000 |
| RAM | 0x2000_0000 | 0x2003_FFFF |

其中，RAM 有 256KB，Flash 有 1MB，在這張圖中我們可以直接知道以下兩點：</br>
1. 主程式儲存區域有 1MB 的大小
2. 其餘資料的空間有 256KB

</br>

那簡單來說，我們第一個工作就是要把程式燒錄到 Flash 中，那我們該如何指定記憶體位置。

Linker 是最主要幫助我們的，這時我們就需要 Linker Script 與組合語言。
（後續會有章節專門介紹）

</br>

### 開機

我們都知道當我們為晶片或開發版上電時晶片就會根據我們所寫好的程式動作。

</br>

開機也就是所謂的 boot，通常晶片都會有 boot 腳位，透過該腳位可以選擇開機的模式，例如從 Flash 開機、SRAM 或是 System Memory 之類的，當然若是較為複雜的系統像是 SoC 還可以指定 eMMC 或是 NAND。

</br>

讓晶片開機的步驟通常分為兩種：Power on 或 POR，Power on 就是簡單的直接上電開機，POR（Power on Reset）也就是 Reset 按鍵讓晶片重新開機。

</br>

在無作業系統的環境下，通常都會以以下流程執行：

![boot flot chart](images/berametal-boot.png#pic_center=100x150)

</br>

接下來一步一步解說：

#### 1. Power on Reset：開機

#### 2. Memory Alias：
   * 指記憶體中的一個資料位址可以通過程序中的多個名稱來訪問。
   * 通過某一個名稱修改數據，其他別名關聯的值也會改變。
   * 根據 BOOT 設定，把我們寫的 Linker 對應到實際的 Flash、System Memory 或 RAM 中。

#### 3. Startup code：
   * 利用組合語言所撰寫之程式。
   * 其中應該包括：
     * 堆疊指標初始化
     * 資料儲存區塊初始化
     * 將 .data（初始化變數）從 Flash 複製到 RAM
     * 將 .bss 區（未初始化變數）清為 0
     * 若有設定 SCB->VTOR，將中斷向量表位址指向 Flash 起始
     * 配置 System Clock（如 HSE/PLL）、FPU、Trace 等設定

#### 4. Application entry：完成記憶體初始化後，跳轉到 main() 或 SystemInit() 等程式進入點。

#### 5. CPU Initialization：配置 Stack、FPU、向量表位址、Cache（若使用 CMSIS）。

#### 6. Peripheral Initialization：系統周邊初始化（USART、GPIO、SPI ... 等）。

#### 7. Main Loop：主程式迴圈。

#### 8. Interrupts Handlers：中斷期間跳離主程式處理外部事件，結束後回到主程式。

</br>

### 檔案 Files

沒錯！你可能會很好奇，我明明在學習 MCU 的裸機開發為甚麼我會需要學習檔案相關的知識？

不要急聽我說：

首先，根據最上面我們所說的我們**沒有所謂的 IDE**，那就代表所有膽案之間的相依性都是我們需要處理的，同時我需要什麼樣的編譯工具（```Tool-Chain```）也是需要去自行選擇的，那這時我們可以學到目前最簡單的就是 ```Makefile```。</br>
當然你可能也看過 CMakeLists 但那大部分都是針對 C++ 所設計的。

</br>

![Makefile](images/Project_file_dependent_surname.png#pic_center=100x150)

在上圖中可以很明顯得看出檔案之間是會有相依性，並且可能會互相參考！


</br>

---

### 小結

透過以上的敘述與概括，我們可以知道我們即將要學到甚麼：如何正確的讓晶片開機、如何正確地設定記憶體、如何讓自己的程式語言與晶片連接在一起...等。

學習指標與順序： </br>
1. 晶片內部記憶體設定 ```Linker``` 與 ```Linker Script```

2. MCU 初始化與程式進入點 ```Startup code``` 與 ```Assembly```

3. 專案編輯規則 ```Makefile```

4. 基本 Emmbedded C code

</br>

---

</br>

在開始之前我來說一下這裡的學習守則，我會依據要用的工具一個一個往下教學，順序大概會是 Makefile → Linker → Startup code → System Init → I/O 再繼續往下延伸，若你手邊有開發版請不要懶，打一遍程式自己試試看， Book 中會附簡單的 PC 程式供驗證與學習。

接下來就開始一起開心學習吧 ～

</br>

# Chapter 2. Makefile

### 簡介

萬事起頭難，讓我們先從基本的編譯開始！

在軟體開發中，make 是一個工具程式，經由讀取一個叫做 Makefile 的檔案，自動化建構軟體。相信資工系的各位衣錠都學過編譯程式還有看過 make，那就不贅述所謂的<font color = red >編譯</font>。

</br>

Makefile 主要用於協助決定大型程式的哪些部分需要重新編譯與檔案之間的相依性。絕大多數情況下，編譯的是 C 或 C++ 檔。其他語言通常有自己的工具，其用途與 make 類似。

</br>

在一個專案中同常我們都會有所謂的 Drivers 或是其它自己寫的標頭檔，那當我的城市中去 include 這些相關 function 時就會有所謂相依性，簡單來說：

```C header
// file name : uart.h

viod uart_tx(uint32_t address, uint32_t data)
{
 ...
}

```

```C
// file name : main.c

include "uart.h"

int main()
{
   uart_tx(0x00200000, "Hello Word\r\n")

   while(1)
   {

   }
}
```

在以上這兩個程式可以知道一件事：在 main.c 中我 include uart.h 並使用其中的 function，這時這兩個檔案就會有所謂的相依性，那在 Makefile 中我們就需要處理這兩個檔案的相依性，如下圖所示這是一個基本的 main 相依性的圖。

![makefile_depend](images/makefile_depend.png#pic_center=100x150)

如果任何檔案的依賴關係發生變化，則該檔案將被重新編譯。

講到這裡，重點：<font color = red>Make 只在意依賴性</font>，而 Makefile 則是處理我需要的相依性並彙整成一個規則，在最後產出我需要的檔案。

</br>

## 程式部分

### Makefile 主要五部分

* 顯式規則：顯式規則表示如何生成一個或多個目標文件。

* 隱式規則：比較簡略地書寫 Makefile 規則，例如規則中有 .o 文件，make 會自動的把 .c 文件也加入依賴關係中。
  
* 變數定義：類似 C 中的 define，定義的變數都會置換到引用位置上。

* 文件指示：
  * 類似 C 語言中的 include，一個 Makefile 中引用另一個 Makefile，如 include makefile.inc。
  * 類似 C 語言中的 預編譯 #if，根據某些情況指定 Makefile 中的有效部分。

* 註釋與換行：Makefile 中只有行註釋，用 # 符號；換行則是使用 \ 符號 。

</br>

#### 顯式規則

**最重要的規則**，顯式規則會直接指定如何從一組檔案產生另一個檔案。

```makefile
target: dependencies
   cmd1
   cmd2
   cmd3
```

基本結構：
* 目標：Target，一個目標檔案，可以是 object 檔，也可以是執行檔，還可以是一個標籤。
  * Target 為檔名，以空格分隔。通常，每個規則只有一個。

* 依賴：Dependencies，要產生的目標檔（Target）所依賴的檔案。
  * Dependencies 為檔名，以空格分隔。
  * 這些檔案需要在運行目標命令之前存在。這些也稱為依賴關係。

* 命令：Command，建立專案時需要執行的 shell 命令。
  * 命令部分的每行的縮進必須要使用 Tab 鍵而不能使用多個空格。

</br>

實際上可能會長得像這樣：

```makefile
main.o: main.c main.h
	gcc -c main.c -o main.o
```

</br>

#### 顯式 Make 命令

Make 預設的假工作目標有 all, install, clean, distclean, TAGS, info 和 check。

以 clean 為例：這種沒有被第一個目標文件直接或間接關聯的 CMD，那麼它後面所定義的命令將不會被自動執行，不過我們可以顯式要求 make 執行。即 make clean。

偽目標：```.PHONY```
* .PHONY 會將目標設成假目標，使 Make 目錄下沒有目標檔案或目標檔案為最新時，仍可執行 Make <target>。
* .PHONY 寫法也可以讓程式設計師知道哪些工作目標不是針對檔案，增加可讀性。

在 Makefile 中，make 的邏輯是：「如果目標檔案不存在，或是它的依賴檔案有更新，就執行對應指令。」
這個邏輯對於一般的 .o 或 .exe 檔案是合理的，但對於 clean 這種「只是想執行某段指令而非生成檔案」的目標就會出問題，所以這時我們就會加上 PHONY。

</br>

#### 隱式規則

GNU Make 內建了許多預設規則，例如它會自動知道如何從 .c 生成 .o，因此你不需要每次都手動寫出來。

```makefile
objects = main.o utils.o

myapp: $(objects)
	gcc $(objects) -o myapp
```

在這段程式中雖然沒有寫明 main.o: main.c 的規則，但 make 會根據檔名自動套用「.c → .o」的隱式規則。

</br>

#### 變數定義與使用

變數宣告時要使用 = 或 := 給予初始值 (注意兩者在代換時稍有不同)。

Ex. ```obj = hello.o foo.o```，取用時寫成 ```(obj)``` 或 ```{obj}```。如果我們想定義一些比較類似的文件，可以使用 ```Unix-like``` 的 ```*``` ， ```?``` 和 ```~```。

自動化變數：
* $@：目前的目標項目名稱。
* $<：代表目前的相依性項目。
* $*：代表目前的相依性項目，但不含副檔名。
* $?：代表需要重建（被修改）的相依性項目。

</br>

```txt
萬用配對字元是 %
```

實際上可能會長得像這樣：

```Makefile
CC:=gcc
exe:=main
obj:=main.o a.o b.o c.o

all: $(obj)
   $(CC) -o $(exe) $(obj)

%.o:%.c
   $(CC) -c $^ -o $@

.PHONY:clean
clean:
   rm -rf $(obj) $(exe)
```

</br>

#### 文件指示

Makefile 支援一些特殊指令，例如：
* include：將另一個 Makefile 的內容插入進來。

* 條件編譯：根據條件選擇性使用某些規則或變數。

```makefile
include config.mk

ifeq ($(DEBUG),1)
  CFLAGS += -g
endif
```

這段表示會引入 config.mk，且若變數 DEBUG 為 1，則加入除錯旗標 -g。

</br>

#### 註釋與換行

* ```#``` 表示註解（整行或行尾）。

* ```\``` 表示換行續行，用於一行寫不完時。

```makefile
CFLAGS = -Wall -O2 \ # 這是換行符號
         -std=c11
```

</br>

以上我們學完 Makefile 我們需要的全部基本知識跟他的構成需要甚麼，接下來我們來詳細學習其中的語法。

</br>

### Makefile 語法

Makefile 是由很多組規一起則所構成的，所以我們需要先設計所有檔案的**相依性**，然而在檔案中又會有他們自己本身需要 include 的部分，此時我們會需要指定相關的**路徑**，接下來我們需要針對自己的開發環境選擇相對定的工具鏈，最後在 all 與 clean 中撰寫規則，all 也就是我的編譯產物，clean 則是清理中間產物，clean 非常重要，若是沒有先行清理則 Make 很有可能會吃到舊的中間產物造成編譯失敗或是產物錯誤。

其中有一個很重要的觀念，在解讀 Makefile 十個人建議先從依賴性的檔案開始看，一路回推到最一開始的檔案，因為 Makefile 在運作時會優先讀取 dependencies，若 dependencies 本身擁有其他規則，則會優先去處理，若是一路往上推都有的話則以此類推。

說那麼多我們不如開始自己試試看。

可以根據 Makefile 資料夾內部的 Sources code 一邊觀看一邊學習。

```
這裡的 Sources code 都是 C。
```

---

</br>

#### 規則 

```makefile
target: dependencies
   cmd1
   cmd2
   cmd3
```

這是我們已經知道的基本結構，現在來多深入了解一下。

Target 可以是對單一檔案的規則，也可以是對最終產物的規則，還可以是對命令的規則。所以有以下特點需要注意：
1. 一個 Target 就只會有一個規則。
2. 一個 Target 可以多個 dependencies，也可以有多個命令。

</br>

撰寫上需要注意的：
1. Target ```:```後與 dependencies 之間需要有空白。
2. 每個 CMD 前都需要 ```Tab```。

</br>

接下來讓我們來解讀一些簡單的程式

```makefile
hello: hello.c
   gcc -o hello hello.c

clean: 
   rm -rf hello
```

* 解讀：
  * Makefile 包含兩個 Target。
  * 第一個 Target 為 hello，他依賴 hello.c，當執行 make hello 時，將使用 gcc 編譯 C 文件定產生執行檔。
  * 第二個 Target 是 clean，用於刪除生成的執行檔。

</br>

#### 多檔案時的編譯

多檔案的編譯時，Makefile 執行時會逐條比對規則。

若某規則的所有 input 均滿足，才會執行該規則。否則，Makefile 會先執行其他可以先執行的規則，最後再回去執行該規則。

```makefile
main: main.o sub.o
   gcc main.o sub.o -o main

main.o: main.cpp
   gcc main.cpp -c

sub.o: sub.cpp
   gcc sub.cpp -c

clean:
   rm -rf main.o sub.o
```

讓我們來依順序解讀一下上面這個程式：
1. Makefile 執行後第一個抓到的 Target 為 main， main 需要 main.o 跟 sub.o 這兩個目的檔。如果gcc找得到這兩個目的檔，才會開始執行main規則。

2. gcc 無法找到這兩個檔案（因為還沒有編譯），因此 gcc 會尋找第一個 dependency，也就是 main.o，接續 main.o 的規則。

3. 到了 main.o，其 dependency 是 main.cpp。 main.cpp就在這個目錄下，因此 gcc 執行 command（gcc main.cpp -c），產生 main.o，並回到 main 規則。

4. 有了main.o，gcc 繼續尋找第二個 dependency (sub.o)。

5. 於是進入 sub.o 規則，找到了 sub.cpp，執行此規則的command (gcc sub.cpp -c)，產生了sub.o。

6. 再次回到 main 規則，發現此時所有 dependencies 都滿足了，終於可以開始進行真正的 command，把所有的 obj 編譯成 main 這隻程式。

</br>

#### 變數使用

現在讓我們加入一些變數，在 Makefile 中變數可以為很多東西，可以為路徑、甚至為編譯工具，根據以上程式加入一個變數就可以寫成以下程式的樣子：

```makefile
CC = gcc

main: main.o sub.o
   ${CC} main.o sub.o -o main

main.o: main.cpp
   ${CC} main.cpp -c

sub.o: sub.cpp
   ${CC} sub.cpp -c

clean:
   rm -rf main.o sub.o
```

</br>

#### Include

在 Makefile 我們可以明確告訴專案與編譯器標頭檔的位置：

```makefile
FreeRTOSINC = FreeRTOS/Include

INCLUDE = -Iinc \
          -IDrivers/Device/inc \
          -I${FreeRTOS_INC}
```

</br>

#### Tool-Chain Setup

這裡需要注意！每個環境與晶片所需要的工具鏈設定不一樣；這裡只舉例，真正在設計與撰寫時請根據 Datasheet 去查詢與實作。

在 Bare-Metal 中最常使用的 Tool-Chain Setup：
1. CC：編譯器，用來編譯 C 程式。
2. LD：連結器，雖然這裡最終用 CC 來做連結，但仍定義了 LD。
3. OBJCOPY：可以用來轉換格式（例如 ELF ➜ bin），雖然這份 Makefile 沒用到。
4. SIZE：顯示可執行檔（ELF）的記憶體佔用資訊。
5. AS：彙編器，用來編譯 startup.s。

```
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
AS = arm-none-eabi-as
```

</br>

####  MCU 設定

在我們開發單晶片時會需要設定晶片的架構，以一般的 ARM Cortex-M3 為例：

```makefile
MCU_FLAGS = -mcpu=cortex-m3 -mthub -mfloat-abi=soft

# -mcpu=cortex-m3：CPU 為 Cortex-M3。
# -mthumb：編譯為 Thumb 指令集。
# -mfloat-abi=soft：使用軟體浮點數運算（無 FPU）。
```

</br>

####  編譯參數設定

通常會叫做 ```CFLAGS```，此為 GNU Make 的慣例變數，用來指定 C 語言的編譯器參數。

若你用 make 呼叫 gcc 編譯 .c → .o，這些參數就會自動加上。

```
CFLAGS = $(MCU_FLAGS) -Wall -O2 $(INCLUDES)

# -02 編譯器優化等級 2，權衡執行效率與編譯時間，適用於大部分開發情境
```

</br>

### Try Try 看

根據 Makefile 資料夾內部的 Makefile 與 C code 開始試試看

### 補充

兩個重要的 GUN Make 內建函數：

1. patsubst </br> 「字串模式取代」，常用於把副檔名 .c 轉成 .o、或是改變路徑、目標目錄等。

2. wildcard </br> 搜尋符合特定模式的檔案名稱。

</br>

# Chapter 3. Linker Script

### 簡介

Linker Script（連結器腳本）是一種指令文件，用來告訴「連結器（linker）」如何將各個編譯出來的目標檔（.o）排列、放進記憶體中。

Linker 的作用就是把輸入檔（object file）的 section 整理到輸出檔的 section。除此之外也會定下每個 object file 中尚未確定的符號位址，所以如果有 object file 用到不存在的 symbol，就會出現常看到的 <font color=red>undefined reference error</font>。

而 linker script 就是提供給 linker 參考的文件，它告訴 linker 我想要怎麼擺放這些 section，甚至也可以定義程式的起始點在哪邊。

在嵌入式開發中，沒有作業系統幫你載入與排程程式，你必須自己告訴系統「程式從哪裡執行、資料放在哪裡」，這正是 linker script 的功能。

```
Note : 這裡的 section 翻譯是指區段
```

</br>

### 流程

在我們知道開機時我的晶片在做甚麼之後，就可以來了解 Linker。

MCU 開機（或 POR）會判斷開機模式，由 Flash memory 開始讀取，並抓取中斷向量表，之後藉由 Entry point 開始進行初始化與跳入 main ，其中主程式會一直保留在 Flash 中執行。

那中斷向量的記憶體位置由這裡做宣告，但註冊則是在 Assembly 中；初始化部分也會是在 Assembly 完成。

</br>

經由這些說明可以清楚知道我們的 Linker Script 應該要做的事：</br>
##### 1. 將需要運行的主程式區塊宣告好
##### 2. 將 RAM 宣告好
##### 3. 設定向量表區塊
##### 4. 設定資料區塊
##### 5. 決定資料的儲存位置

</br>

## 程式部分

### 五個關鍵部分

* memory：定義 embedded 可以使用的記憶體區域，需指定起始位置、大小和存取權。
  * 每個區塊都需要有一個名稱，例如：Flash、RAM。
  * 存取權限：ｒ讀取、ｗ寫入、ｘ執行。

* sections：memory 中定義的記憶體區域內不同部分的位置。
  * 例：.text 程式、.data 初始化資料、.bss  未初始化資料。

* entry：設定程式執行的入口點。

* symbols：在連結器檔案中定義自訂符號。
  * 這些符號可用於各種目的，例如：定義特定部分或記憶體區域的起始和結束位址，或作為程式碼中的常數。

</br>

![linker_script_fromt](images/linker_script_fromt.png#pic_center=100x150)

</br>

### Linker Script 語法

在開始前先讓我們看一下 Linker Script Code 會長甚麼樣：

![Linker Script Code](images/linkerscript_1.png#pic_center=100x150)

</br>

根據這張程式碼可以總結出一個設計流程：

Step 1. 設定程式入口點 (entry) </br>
Step 2. 定義記憶體區域 (memory) </br>
Step 3. 指定 sections </br>
Step 4. 定義符號 </br>
Step 5. 儲存附檔名為：.ld </br>

</br>

接下來開始講解語法，接下來不會根據上述的流程逐步講述，因為不是全部都會用到。

#### 重點：請想像自己是在寫記憶體堆疊，一層一層的寫，才不容易亂掉

</br>

#### MEMORY

對於指定記憶體區塊的大小和位置。

```ld
MEMROY
{
  name (attributes) : ORIGIN = address, LENGTH = size
}
```

此區域用於宣告記憶體區塊，請根據實際大小作宣告。在這裡的 FLASH 與 RAM 就像是 ```#define``` 的定義，name 請自便。

1. ORIGIN：記憶體最一開始的基本位址。
   * 接下來用到時會根據這個位置開始往下長。

2. LENGTH：該區段的容量大小。
   * Byte 作為單位來進行計算。
   * 假設一個記憶體的區段為 0x30000000 ~ 0x303FFFFF，則為 4MB。

3. 再次複習，存取權限 (attributes)：ｒ讀取、ｗ寫入、ｘ執行。

</br>

實際上可能會長得像這樣：

```ld
MEMROY
{
  FLASH (rx) : ORIGIN = 0x30000000, LENGTH = 4M
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 256M
}
```

</br>

#### SECTIONS

Linker Script 的核心：SECTIONS 區塊。這是決定程式碼與資料實際如何放入記憶體的地方。

</br>

常見的記憶體區段：

| Section 名稱 | 用途說明                   | 放在哪裡              |
| :----------: | :----------: | :----------: |
| `.text`    | **程式碼**段（function）     | 通常放在 FLASH（唯讀可執行） |
| `.rodata`  | **唯讀資料**（如 const 字串）   | FLASH             |
| `.data`    | **已初始化變數**（int a = 3;） | RAM（執行時）          |
| `.bss`     | **未初始化變數**（int b;）     | RAM               |
| `.stack`   | **堆疊**空間               | RAM               |
| `.heap`    | **動態記憶體**（malloc）      | RAM               |


請注意以上為常見，不一定是每個都會需要，我相信聰明的你一定可以看出來那些是必要的！

</br>

總體概觀：

```ld
SECTIONS
{
  .text : { ... } > FLASH
  .data : { ... } > RAM
  .bss  : { ... } > RAM
}
```

須注意，在 LD 中是不吃 ```tab``` 的，一律要使用 ```space```。

</br>

---

請注意這個點 ```.```，他是 Location Counter，代表指向的位址，一開始這樣講可能會很抽象，你可以把它想像成是一個滑鼠的指標，現在只到哪記憶體就會寫到哪，舉個例子：

```ld
MEMORY
{
   FLASH = ORIGIN = 0x30000000 , LENGTH = 4MB
}

SECTIONS
{
  .text : { *(.text*) }  > FLASH  /*1KB*/
  .rodata : { *(.rodata*) }  > FLASH  /*1KB*/
}
```

假設我的程式與唯讀資料的大小都剛好是 1KB，讓我們依序來解讀一下：
1. Location Counter 目前在 ```text``` 的位置
2. 將程式區段寫入 FLASH 中，從 0 開始，因為是第一個，0x30000000 + 1KB = 0x30000400
3. 接下來 Location Counter 在 ```rodata``` 的位置，接續上次寫入後的位置開始繼續往下寫
4. 將唯讀資料寫入 FLASH 中，從 0x30000400 開始，0x30000400 + 1KB = 0x30000800

</br>

再多舉一個例子加深印象，因為很重要！！！

```ld
SECTONS
{
  . = 0x10000;
  .text : { *(.text*) }
  . = 0x8000000;
  . data : { *(.data*) }
  .bss : { *(.bss*) }
}
```

依序解讀：
1. Location Counter 移到 0x10000
2. 在這裡寫入 .text 的 section
3. Location Counter 移到 0x8000000
4. 在這裡寫入 .data 與 .bss 的 section

---

</br>

再有以上基本認知後，開始來了解我該如何設計我的 LD secion 本體。

**SECTIONS 內部各區段基本架構：**

```ld
section [address] [(type)] :
  [AT(lma)]
  [ALIGN(section_align) | ALIGN_WITH_INPUT]
  [SUBALIGN(subsection_align)]
  [constraint]
{
  output-section-command
  output-section-command
  .
  .
  .

} >region [AT>lma_region] [:phdr :phdr ...] [=fillexp]

```

逐一說明：

| 欄位                 | 說明                                            |
| :------------------: | :--------------------------------------------- |
| `section`          | 區段名稱（如 `.text`, `.data`, `.bss`）              |
| `[address]`        | **虛擬地址（VMA）**，代表此區段執行時的起始地址                   |
| `[(type)]`         | 區段類型，可省略，較少使用（除非使用特殊段屬性）                      |
| `:`                | 開始定義此段內容                                      |
| `AT(lma)`          | **載入地址（LMA）**，代表此段在映像檔中的位置（例如在 FLASH 中）       |
| `ALIGN(...)`       | 設定段起始地址的對齊大小（例如 `ALIGN(4)` 就會四位元組對齊）          |
| `ALIGN_WITH_INPUT` | 對齊方式與輸入段一致                                    |
| `SUBALIGN(...)`    | 子段的對齊方式（例如內部小段落 .text.startup）                |
| `[constraint]`     | 條件設定，如 `ONLY_IF_RO`（僅唯讀時使用）或 `SORT(...)`      |
| `{ output-section-command }` | 包含此區段內容，也就是我們要怎麼擺放每個 section（例如 `*(.text)`）                        |
| `>region`          | 將這個 section 放入哪個記憶體區段（對應 `MEMORY` 中定義）        |
| `AT>lma_region`    | 指定 LMA 對應的記憶體區段名稱（與 `MEMORY` 中相對應）            |
| `:phdr`            | 指定此 section 要對應到哪個 Program Header（for ELF 檔案） |
| `=fillexp`         | 使用指定值填滿空隙，例如 `=0xFF` 表示填充 0xFF                |

</br>

在繼續往下之前，先了解甚麼是 VMA 與 LMA：

* Link Script 中設計了兩種位址：VMA 和 LMA
  * LMA 是 output file 的位置。
  * VMA 是載入 section 到 RAM 時的位置。
  * 大多數情況下兩者會是一樣的。

| 項目                          | 說明                      |
| :--------------------------- | :----------------------- |
| VMA（Virtual Memory Address） | 程式執行時載入到的位址（通常在 RAM）    |
| LMA（Load Memory Address）    | 編譯後映像檔中儲存的位址，程式碼保存的位置（通常在 ROM/FLASH） |

簡單記憶法：
* 有 AT 就是 LMA
* 沒有就是 VMA

</br>

---

</br>

解讀一段 section 看看

```ld
.data : AT(0x08004000)
ALIGN(4)
{
  _sdata = .; /*data 的起址*/
  *(.data)
  _edata = .; /*data 的終址*/
} > RAM
```

這是一個簡單的 LD data 區段也是最經典的，前面都很好懂比較有問題的可能是後面的搬移部分，讓我們一起簡單解讀：
1. data 的位址會在 0x08004000 開始，並以 4 位元組對齊。
2. _sdata 的 Symbol 對應到 Location Counter 也就是目前的 data 起址。
3. 將 data 放置到這個區段中。其中 `*` 代表「所有輸入檔案」。
4. _edata 的 Symbol 對應到寫入完成後的 Location Counter 也就是目前的 data 終址。
5. 最後將其搬移到 RAM 中。

</br>

那問題來了：為甚麼要搬移到 RAM 中 ～

雖然 .data 區段的初始化資料儲存在 FLASH（不可改變），但程式執行時，這些變數是 會被修改的，所以它們必須搬到 RAM 來執行！

舉個例子：

```c
int a = 5; // 編譯時會變成 .data
```

* 5 的初始值會被儲存在 FLASH。
* 但變數 a 是可以修改的（例如 a = 10;）
* 而 FLASH 是唯讀的，不能在執行時修改 → 所以必須把 a 搬到 RAM 去執行。

相信聰明的你也可以同步了解 `.bss` 了。

</br>

---

</br>

接下來讓我們來談談 Symbol，通常我們會用來代表一個區段的開始與結束的位址，例如 _smodule 代表 module 的開始，而 _emodule 則代表 module 的結束。

它的本質是一個<font color=red>符號</font>，對應到一個記憶體位址，那想當然他不會占用記憶體空間。

特點：
* 它可以被 extern 到 C code 或 starup code 中使用。
* 它可以在 C code 中當作變數來使用。
* 它的值是地址，不是變數的值。
* 通常會用在啟動階段的 .data 初始化與 .bss 清除流程中。

</br>

---

</br>

恭喜你會了 LD 中最重要的部分，那我們來試試看解讀難一點的：

```ld
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 128K
  CCMRAM(rwx): ORIGIN = 0x10000000, LENGTH = 64K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
  } >FLASH

  .data :
  {
    *(.data)
    . = ALIGN(4);
  } >RAM AT>FLASH

  .bss :
  {
    *(.bss)
    . = ALIGN(4);
  } >RAM
}
```

</br>

#### ENTRY : 指定程式的「入口點（entry point）」

</br>

#### NOLOAD

表示在程式啟動期間不應將特定部分載入記憶體。

Debug 期間使用。

</br>

#### ALIAS

為現有符號建立別名。這對於提供與舊程式碼的相容性或為常用符號創建更具描述性的名稱很有用。

</br>

#### ASSERT

用於在連結過程中如果不滿足指定的條件則產生錯誤。這有助於確保滿足某些約束，確保某個部分不會溢出其分配的記憶體區域。

</br>

#### Heap memory

#### Stack memory

</br>

# Chapter 4. Startup Code

</br>

# Chapter 5. System Initialization

</br>

# Chapter 6. C code to I/O & System control

</br>

# 最後

OK 以上全部都是個人涉及到的基本裸機開發知識接下來就搭配著猛男專主的音樂一起開發吧！

[最棒的音樂](https://youtu.be/tQJIf9mTigc?si=QUFVnc42w9GcifWf)