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

在一個專案中同常我們都會有所謂的 Drivers 或是其它自己寫的標頭檔，那當我的程式中去 include 這些相關 function 時就會有所謂相依性，簡單來說：

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

`.text`：程式碼與唯讀資料

* `*`：表示所有輸入檔（如 main.o、libxxx.a）
* `.text`、`.rodata`：包含程式碼與唯讀資料
* `.text.*`：如 .text.startup、.text.main
* `. = ALIGN(4)`：讓下一段記憶體地址 4-byte 對齊（提升效能與避免錯誤）
* 放在 `>FLASH`：因為程式碼與常數不會改變，可以直接存在 Flash

</br>

`.data`：初始化資料（可寫）

* `.data`：放已初始化的變數（例如 int x = 3;）
* `>RAM`：執行時要放在 RAM
* `AT>FLASH`：初始值存在 FLASH 中，啟動時複製到 RAM
* 這是最常見的 `.data` 結構：初始化資料先存放在 FLASH，執行時複製到 RAM

</br>

`.bss`：未初始化資料（自動清零）

* `.bss`：未初始化的全域或靜態變數（例如 int x;）
* 不佔用映像檔空間，只預留空間
* 啟動時由 C startup code 清為 0（通常用 memset 或迴圈）
* 執行時會佔用 RAM 空間，但不佔用 Flash 空間。

</br>

#### ENTRY : 指定程式的「入口點（entry point）」

寫法：

```ld
ENTRY(Handler)
```

Handler 是啟動後第一個被執行的函式，通常寫在 startup.s 檔中。

</br>

#### NOLOAD

表示在程式啟動期間不應將特定部分載入記憶體。
* Debug 期間使用。

在 LD 的 SECTIONS 裡加上 NOLOAD 會告訴 linker：

> 這段區域在執行時（RAM 中）需要保留空間，但在編譯出來的映像檔中不要載入資料。

寫法為上述的 [constraint]。

</br>

#### ALIAS

在 linker script 中，ALIAS 本身不是一個內建關鍵字，但在 C 語言或啟動碼中，我們經常會看到 __attribute__((alias("..."))) 這樣的語法，用來建立「別名函式（Function Alias）」。

> GCC 的 compiler attribute（不是 linker script 指令）

寫法：

```c
void MyHandler(void) __attribute__((alias("Default_Handler")));
```

表示 MyHandler 其實就是 Default_Handler，編譯器與 linker 都會把它們視為同一個實體地址。

為甚麼需要：
1. 簡化中斷處理：你只要寫一份 Default_Handler()，所有沒定義的中斷都會跳進來。
2. 保留可擴充性：使用者只要定義自己需要的 handler，不需定義全部。

</br>

Linker 角度看 alias 是什麼？

```ld
00000001 <Default_Handler>:
00000001 <HardFault_Handler>:
00000001 <NMI_Handler>:
```
這些 symbol 最終都會指向同一個記憶體地址。

</br>

#### ASSERT

用於在連結過程中如果不滿足指定的條件則產生錯誤。這有助於確保滿足某些約束，確保某個部分不會溢出其分配的記憶體區域。

寫法：

```
ASSERT(expression, "error_message")
```

| 參數                | 說明              |
| :----------------- | :--------------- |
| `expression`      | 一個條件（為 0 表示錯誤）  |
| `"error_message"` | 如果條件不成立，顯示的錯誤訊息 |

</br>

#### Heap memory

用於動態記憶體分配的 RAM 區域。在嵌入式系統中，Heap 允許在運作時使用 malloc()、calloc()、realloc() 和 free() 等 API 分配和釋放記憶體。

</br>

執行階段（runtime）動態配置記憶體用的區域，像：

```c
int *p = malloc(100);
```

這 100 bytes 就是從 heap 拿來的。

</br>

在 linker script 裡，你通常要手動劃出一塊記憶體給 heap 使用，然後由底層 malloc() 實作使用這塊區域。

通常都會在 RAM 的最後一塊配置 HEAP：

```ld
.heap (NOLOAD) :
{
  _sheap = .;             /* heap start symbol */
  . = . + 0x2000;         /* 分配 8KB heap 空間 */
  _eheap = .;             /* heap end symbol */
} > RAM
```

然後在 C 這樣實作：

```c
extern char _sheap;  // linker script 定義的 heap 起點
extern char _eheap;  // heap 結束

static char *heap_ptr = &_sheap;

void *_sbrk(ptrdiff_t increment) {
    char *prev_heap = heap_ptr;
    if (heap_ptr + increment > &_eheap) {
        return (void *)-1; // 堆溢出
    }
    heap_ptr += increment;
    return (void *)prev_heap;
}
```

> `_sbrk()` 是 GNU libc malloc 內部呼叫的函式，用來向系統「要空間」。

</br>

#### Stack memory

用於儲存局部變數、函數呼叫參數和返回地址。

堆疊向下增長，這意味著隨著新資料的添加，堆疊指標會向較低的記憶體位址移動。

要在連結檔案中定義堆疊大小，建立一個代表堆疊頂部位址的自訂符號。通常，堆疊位於 RAM 區域的末端。

</br>

Stack 是一種 後進先出（LIFO） 的記憶體區域，用於儲存：
* 函式的 區域變數
* 函式呼叫時的 返回位址
* 參數傳遞
* 中斷上下文保存

</br>

通常我們會將 Steak 配置在 RAM 的頂部：

```ld
_estack = ORIGIN(RAM) + LENGTH(RAM);  /* 堆疊頂部（SP 初始化值） */

.stack (NOLOAD):
{
  _sstack = .;
  . = . + 0x1000;  /* 分配 4KB 的 stack 空間 */
  _estack_region = .;
} > RAM
```

> 通常會同步搭配 ASSERT 檢查 Stack 的空間是否足夠 ～ 避免 heap 與 stack 碰撞 ～

</br>

#### Heap & Stack

這時一個經典的問題就會出現了：我的 Heap 跟 Stack 都是放在 RAM 的區段並且都是自由成長，一個向上一個向下那我怎麼知道他會不會有衝突的問題，而且我又要確保有 data 與 dss 的記憶體空間？

</br>

哎呀！頭好痛，此時不如讓我們喝杯咖啡緩衝一下，先來了解 Heap 與 Stack 的區別：

| 項目   | Stack          | Heap                  |
| :---- | :-------------- | :--------------------- |
| 分配方式 | 編譯器自動分配        | `malloc` / `new` 手動分配 |
| 生命周期 | 函式作用域          | 直到 `free` 或程式結束       |
| 成長方向 | 通常「向下」         | 通常「向上」                |
| 速度   | 非常快（在暫存器或 RAM） | 較慢                    |

> 這就是為甚麼 `free` 很重要！！！

</br>

讓我們再複習一下，LD 的記憶體配置，觀察一下下面的表格：

```txt
RAM 起始地址（低）0x1000_0000
▼
+---------------------+ <- ORIGIN(RAM)
| .data               | ← 有初始值的變數
+---------------------+
| .bss                | ← 未初始化變數
+---------------------+
| .heap               | ← malloc 用，向上成長 ↑
+---------------------+
| free space / guard  |
+---------------------+
| .stack              | ← 向下成長 ↓
+---------------------+ <- ORIGIN(RAM) + LENGTH(RAM)
RAM 結束地址（高）0x10FF_FFFF
```

相信看完這個表格，聰明的你已經知道了問題的答案 ～

以下這是相對應的 LD :

```ld
MEMORY
{
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
  /* .text 留在 Flash 就略過 */

  .data : AT(_sidata)
  {
    _sdata = .;
    *(.data)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  /* heap 從 _ebss 開始，長度自己定義 */
  .heap (NOLOAD) :
  {
    _sheap = .;
    . = . + 0x1000;  /* 4KB heap */
    _eheap = .;
  } > RAM

  /* stack 從 RAM 頂端開始向下分配 */
  .stack (NOLOAD) :
  {
    . = ORIGIN(RAM) + LENGTH(RAM) - 0x1000; /* stack 預留 4KB */
    _estack = .;
    . = . + 0x1000;
    _sstack = .;
  } > RAM

  /* 最後檢查 heap 與 stack 沒有碰撞 */
  ASSERT(_eheap <= _estack, "ERROR: Heap and Stack overlap!")
}
```

</br>

### 小結

讓我們來小小總結一下：

相信各位都有用過 Keil 的經驗，在那個 IDE 中我們很常會操作所謂的 `魔術棒`，其中一點開就會出現 `Target` 的選項，在下方其實就是相關的燒錄記憶體設定。

下方這張圖就是 linker 結合後會有的樣子：

![linker 結合](images/Linker_1.png#pic_center=100x150)

</br>

### Try Try 看

根據 linker 資料夾內部的 Makefile 與 linker 開始試試看

</br>

### 補充

#### KEEP

保留某個符號不要被 Garbage Collection。

不希望 ARM 的 ISR vector 會被優化掉。

</br>

#### address map

在設計裸機開發中，個人習慣加上 Address Map 的功能，這是 Makefile 的功能可以直觀的輸出一個我們寫好的 Address Map 供我們檢查。

</br>

# Chapter 4. Startup Code

> Startup code 是用組合語言所撰寫的，這裡不講述過多的語言，指講述專門用到的。

</br>

## 簡介

Start-up code 是第一個在 MCU POR(Power on reset)後運行的程式。

引導 MCU 所需的關鍵部分初始化以及隨後目標 Application 啟動。

執行基本任務，使 MCU 準備好運行 Application 。

> 它們是彙編程式指令，這意味著它們告訴彙編程式一些事情，但不產生「程式碼」。

</br>

主要任務：
1. 堆疊指標初始化
   * ​設定堆疊指標，指向 RAM 中的特定位置，用於儲存函數調用的堆疊和區域變數。
2. 資料段初始化
   * 將已初始化的全域和靜態變數從 FLASH/ROM 複製到 RAM。
   * 確保這些變數在執行時具有正確的初始值。
3. BSS 初始化
   * 將未初始化的全域和靜態變數區域（BSS段）清零。
   * 確保所有變數都獲得某個已知值。
4. 向量表初始化
   * 設定中斷向量表的位置。
   * 確保中斷發生時能正確調用對應的中斷處理函數(IRC)。
5. CLK 與 Debug 初始化
   * 根據應用需求設定系統 CLK，確保程式以正確的速度運行。
   * 設定除錯介面，如啟用或禁用 JTAG/SWD，設定 UART 用於除錯輸出。

</br>

這些初始化步驟通常由重置處理函數（Reset Handler）執行，完成後將控制權轉交給主程式（main() 函數），開始執行應用程式。

</br>

## 程式部分

讓我們先看一個基本的 Startup code 附上逐行說明：

```asm
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata     ; 目的地起始位址（RAM 中 data 段開頭）
  ldr r1, =_edata     ; 目的地結束位址（RAM 中 data 段結尾）
  ldr r2, =_sidata    ; 資料來源（Flash 中 data 初始化值所在）
  movs r3, #0         ; 資料拷貝的 offset
  b LoopCopyDataInit  ; 跳至拷貝迴圈

CopyDataInit:
  ldr r4, [r2, r3]    ; 從 flash 的 sidata+r3 讀取 4 byte
  str r4, [r0, r3]    ; 寫入 RAM 的 sdata+r3
  adds r3, r3, #4     ; 下一筆資料

LoopCopyDataInit:
  adds r4, r0, r3     ; 當前目的地指標
  cmp r4, r1          ; 是否到達 sdata 的結尾（edata）
  bcc CopyDataInit    ; 如果還沒到，繼續拷貝（bcc = unsigned <）

/* Zero fill the bss segment. */
  ldr r2, =_sbss      ; .bss 段起始地址（在 RAM 中）
  ldr r4, =_ebss      ; .bss 段結束地址
  movs r3, #0         ; 要寫入的值（0）
  b LoopFillZerobss   ; 跳至清除迴圈

FillZerobss:
  str r3, [r2]        ; 將 0 寫入 r2（清掉一個變數）
  adds r2, r2, #4     ; 前進到下一個變數位址

LoopFillZerobss:
  cmp r2, r4          ; 是否到達 .bss 段結尾
  bcc FillZerobss     ; 還沒到就繼續清

bl SystemInit             ; 初始化時鐘等硬體系統設定
  ; bl __libc_init_array  ; C++ 用：呼叫靜態物件建構子（可選）
  bl main                 ; 呼叫主程式
  bx lr                   ; 結束函式（實際不會回來）
  .size Reset_Handler, . - Reset_Handler
```

整體流程簡介：
1. 複製 .data 區段（已初始化變數）從 Flash 到 RAM
2. 清除 .bss 區段（未初始化變數）為零
3. 呼叫系統初始化（SystemInit）、C 序言（__libc_init_array）與 main

## 指令部分

#### .syntax	unified

使用現代組譯器語法 + 自動生成 IT 指令。放在源文件的頂部。

</br>

#### .weak		label{,label}

允許 'label' 未定義。如果未定義，它將具有 NULL (0x00000000) 的值。

</br>

#### .weakref         label,defaultLabel

允許 'label' 未定義。如果未定義，它將具有另一個標籤的值。

</br>

#### .section         sectionName

從現在開始，所有輸出都進入名為 'sectionName' 的區段

</br>

#### .align           [bitposition]

對齊輸出偏移

</br>

#### .long            value

輸出一個 32 位的值

</br>

#### .text 

從現在開始，所有輸出都進入名為 '.text' 的區段

與 .section .text 相同

</br>

#### .func            label[,actualLabel]

標記函數 'label' 的開始，以便鏈接器可以在未被引用時排除該塊

</br>

#### .endfunc

標記函數的結束

</br>

#### .pool

允許組譯器在此處放置常數

</br>

#### .size            label,size

告訴鏈接器該符號指向的塊的長度（以位元組為單位）

</br>

#### .thumb_func      label

標記這是一個 thumb 函數，如果函數是通過 'bx' 或 'blx' 調用的，則需要

</br>

#### .type            label,%type

指定符號的類型。如果某處有指向該函數的指標，則需要。

</br>

#### .cpu             cpuType

cpuType 可能是 cortex-m0、cortex-m3 或 cortex-m4。

</br>

> 每個晶片都會要有不同的軟體函數與需要啟動的部分請根據 Datasheet 與使用者手冊設計。

</br>

## 開發重點

> 老話一句：程式碼上網查都會有答案，接下來講述轉寫思路與設計方向

### 思路

先設想一下，在一個晶片開機時我們會需要的動作是甚麼，在前面也有講述過。

1. 我們需要設定程式進入點，與程式入口操作
2. linker 的搬移
3. 有可能需要統一語法，這裡的語法指 ARM 與 Thumb 指令通用的語法風格，支援更簡潔的語法與組譯器指令集混合
4. 在 linker 定義好的區段需要的相關操作
5. 主程式的開始
6. 自己的硬體需要的相關初始化

</br>

### 設計方向

Startup code 離不開我們的主程式、中斷、系統初始化還有連結器，所以在設計時基本上可以說是同步。

</br>

# Chapter 5. System Initialization

### 簡介

在開始前讓我們先知道一個重要觀念，一個晶片之所以可以運作除了我們前面所提及的那些程式之外，還有像是 System Clock、晶片內部的匯流排（Bus）與中斷向量表等等。

在開始繼續講解 System Initialization 前，請一定確保自己可以看懂晶片的內部硬體圖，這邊舉個簡單的例子：

</br>

這是一張 STM32F103 的硬體 Block 圖：

![stm32f103_hw_block](images/stm32f103_hw_block.png#pic_center=100x150)

</br>

我們可以知道以下幾件事：
1. CPU core 的主時脈頻率是 72MHz
2. Clock 來源有兩個：RC 8MHz、RC 40kHz
3. Bus 的走向，哪個協定是通過哪個 Bus 控制，這在下一章會說明
4. NVIC 中斷控制

這時讓我們在往下翻翻找找一定可以找到我們開機所需要初始化的部分，那接下來我的工作就是跟著說明書一步一步往下做。

</br>

接下來我只會說明一樣部會以特定開發版為例，我會同步以前很多的程式與觀念並行說明。

---

</br>

## System NVIC

NVIC 這個詞我們很常聽到，那他是甚麼，我們先來了解一下。

NVIC（Nested Vectored Interrupt Controller），中文就是<font color = red>**嵌套向量中斷控制器**</font>，那顧名思義這東西就是 CPU 管理中斷的核心區域，接下來我們需要根據這些區域去設定我們的中斷表。



</br>

### Interrupt 與 IRQ



</br>

### Handler 與導向



</br>

## System Clock



</br>

# Chapter 6. C code to I/O & System control

</br>

# 最後

OK 以上全部都是個人涉及到的基本裸機開發知識接下來就搭配著猛男專屬的音樂一起開發吧！

[最棒的音樂](https://youtu.be/tQJIf9mTigc?si=QUFVnc42w9GcifWf)
