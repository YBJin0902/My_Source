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

# Bare-Matel 概述

甚麼是 Bare-Matel（俗稱裸機），也就是在沒有任何資源的情況下進行晶片的開發（這裡的晶片不限於 MCU SoC 等）。拿 STM32 開發來舉例，我們可以使用 STM32CubeMX 利用 UI 介面對想要的功能直接點選後生成 Project，整份專案就會直接出現，這就是最簡單的例子，在生成好的專案中我們只需要撰寫 main 即可，其實在很多廠商提供的 Example 中我們都可以直接這樣做，因為他們已經幫我們做好了很多前置工作，像是：

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

```
那我們燒錄的是甚麼，最常聽到的其實就是 Program Image 也就是在我們編譯好整份專案之後所產出來的檔案，檔案很可能是 .elf .bin 或是 .hex 都有可能，那編譯的部分是 Makefile 的工作並不是這裡要贅述的部分。
```

</br>

假設在產品的 Datasheet，我們可以看到有關於 Flash、SRAM 的位置：

![example memory map](images/memory_map.png#pic_center=1x1)

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

