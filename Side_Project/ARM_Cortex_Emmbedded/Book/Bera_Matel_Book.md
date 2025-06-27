# Bera-Metal Book

不以任何開發版、單晶片為例純粹講述觀念與各語法知識。

須注意不同的編譯器有不同的語法與設定需要注意。
像是 Keil 不是使用 Makefile 與 Linker Script 而是其他設定檔。本系列會以 Ecipse 為主要編譯器。

### 章節
#### Chapter 1 Bare-Matel 概述
#### Chapter 2 系統論述
#### Chapter 3 專案設計與構思
#### Chapter 4 初步設計
#### Chapter 5 周邊設計

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

# Makefile

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

講到這裡，重點：<font color = red>Make 只在意依賴性</font>，而 Makefile 則是處理我需要的相依性並彙整成一個規則，在最後產出一個我需要的檔案。

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

說那麼多我們不如開始自己試試看。

---

</br>

#### 規則 

在上面我們已經知道怎麼撰寫規則
