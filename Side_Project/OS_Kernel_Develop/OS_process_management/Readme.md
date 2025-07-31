# 並行計算基本認知

在開始之前先介紹基本觀念：並行計算。</br>
這是作業系統中一定要先理解的觀念。</br>

先前已經有學過的韌體知識可以先丟掉，接下來會是不一樣的觀念。</br>

## 交錯執行、平行、並行

在不同的系統中執行流程會有不同的執行方式，也是我們常講的執行緒，在以前的觀念中我們所繪製的流程圖都是單執行緒（單核心、單CPU）。</br>

但後續我們開始了 RTOS，也就是所謂的類多執行緒，在類多執行緒中，我們會把一個單執行緒（單核心、單CPU）的系統所需要處理的事，切分成多個任務，在這樣的情況下我們做的事其實只是將程序切分成多個任務去執行而已，並不是所謂的並行或平行執行。注意，就算切成多任務也會設立明確的切割點。</br>

平行執行（Parallel），所謂的平行執行是多個程序在同一時間同時執行，最常見的情況就是在多核心晶片中，在開發這樣的系統架構時，最簡單的做法就是在不同的 CPU 中分別寫不同的程式並直接讓晶片執行，在不考慮其他的情況下（如對稱架構、非對稱架構），通常都可以直接讓 CPU 分開執行程序。</br>
用專業術語來說的話，平⾏執⾏程式的 process 或 thread 數⽬和 CPU 或核⼼
⼀樣多，每⼀個 process 或 thread 都佔⽤⼀個 CPU 或核心，使得所有 process 或 thread 都能同時執⾏。</br>

並行執行（Concurrent），並行執行的情況則較為特殊，通常他的程序會遠大於 CPU 和核心，這樣的情況下這些程序是在交錯或是平行之間執行。一開始這樣說可能很抽象，拿我們常用的作業系統舉例的話，當我們開啟很多視窗時屬於平行執行，因為 CPU 本身會一直執行我們所賦予的工作並不會隨意 Kill 掉任何程序，頂多給予的資源較少。</br>
再多一層想像，我們現在正在操作一個四核心的 CPU，此時開啟四個程式，其中一個包含記事本（具有輸入與輸出的功能），現在我們開始操作記事本，在操作記事本的同時由於四核心的架構無法負荷超過四個程序（需要輸入輸出，加上原本的四項程式，所以為六項），在這個記事本中我們一邊輸入可以一邊看到我們所輸入的文字同時顯示（交錯），此時系統會讓某些程序執行很短的時間後把核心交給其他程序使用，於是任意時刻，每一個程序在執行上都會有進展，在執行完成之前都會是做一下、停一下、再繼續做一下。</br>

</br>

### 1. 交錯執行
   ![Interleaved](../images/Interleaved.png#pic_center=100x150) </br>
### 2. 平行執行
   ![Parallel](../images/Parallel.png#pic_center=100x150) </br>
### 3. 並行執行 </br>
   Process 是在交錯執⾏或平⾏執⾏ </br>

</br>

## 同步（Synchronization）

在知道這些執行方式之後，有一個明顯的問題將會浮顯出來，也就是這些程序之間會需要互相溝通。舉個例子，今天專題小組分工，分成硬、韌、軟體，但若是組員之間沒有良好的溝通，那這個小組的運作一定有問題，甚至可能會無法完成原本預計的工作，由上述可見溝通的重要性。</br>

</br>

---

</br>

# 作業系統和硬體基本知識

## 多處理機系統、對稱多處理機系統、多核心系統 

何謂多處理機系統：多處理機系統（ Multiprocessor Systems）通常也叫做平⾏系統（Parallel Systems）或緊密連接系統（Tightly Coupled Systems）有若⼲個CPU。

多處理機系統它的優點：
* 增加⽣產⼒：可以完成更多的⼯作。
* 較⼤的系統經濟效益：因為系統中有共⽤的資源，⼀個多處理機系統會⽐很多個單處理機系統更經濟。
* 增加可靠度：⼀個處理機的問題不會使整個系統「當機」。

</br>

在多核心系統中我們可以先以硬體與軟體設計兩種方式進行區分：

### 1. 硬體

* 同構多核心架構：系統中的處理器在架構上是相同的，像是雙核心的架構中兩個都為 Cortex-A9

* 異構多核心架構：系統中的處理器在架構上是不同的，像是雙核心的架構中一個為 Cortex-A9、一個為 Cortex-M4

### 2. 軟體設計

* SMP：Symmetric multiprocessing（對稱式架構），多個核心運行一個作業系統，而這個作業系統同等的管理多個內核，這種運作模式就是簡單提升效能。
  * 目前支援此運行模式的作業系統有：Linux，Windows，Vxworks。
  * 目前，我們的PC機使用的就是這種運作模式，一般適用於功能複雜，對即時性要求不高的系統。

* AMP：Asymmetric multiprocessing（非對稱式架構），多個核心相對獨立運作不同的任務，每個核心之間相互隔離，可以運行不同的作業系統或裸機程式。
  * AMP 的運作模式基本上不會有開銷問題，尤其是在執行裸機程式時，甚至沒有開銷，這種模式比較適合即時性高的應用。
  * 兩個核心之間的通訊與資源共享需要有一套優秀的處理機制。
  * 雖然多個核心可以運行不同的系統，但是需要有一個主要的核心，需要使用該核心來控制整個系統以及其他的核心。
    * 例如：一個核心運行運行即時性較高的任務，另一個核心運行 UI 介面。

* BMP：bound multiprocessing，BMP 運作模式與 SMP 類似，同樣也是一個 OS 管理所有的核心，但開發者可以指定將某個任務只在某個指定核心上執行。

</br>

## 雙執行模式

雙執⾏模式（Dual-Mode）：使⽤⼈模式（user mode）和作業系統模式（supervisor mode）。這兩個模式由⼀個模式數元（mode bit）決定。

</br>

## Interrupt & Trap

在作業系統中，一個需要 OS 處理的事件就是一個 Interrupt，這些事件包含像是：輸出/輸入、程式請求服務、程式中用 0 除、程式使用不再配置記憶體內的位置 等等。

Interrupt 可能由硬體產⽣（譬如終端機斷線、輸出/輸⼊完成），也可能由軟體產⽣（譬如程式中⽤ 0 除、程式要求系統服務）。這是在基本處理機就知道的知識！

若⼀個 Interrupt 是由軟體產⽣（譬如⽤0除），這個 Interrupt 通常叫做 Trap；所以，所有的 Trap 是所有 Interrupt 的部份集合。

</br>

Interrupt 與 Trap 是兩種不同的事件，Trap 是由使用者的程式所觸發，而 Interrupt 是由硬體設備觸發。

先來探討 Interrupt 與 Trap 之間的差異 </br>

### Trap

Trap 是一種<font color = red>**同步中斷**</font>，由使用者程序中的異常觸發，以允許執行功能。

作業系統中的 Trap 可能會由異常情況觸發，例如不正確的記憶體存取、除以零或斷點。

Trap 會將作業系統切換到核心模式。作業系統隨後會執行一些操作，然後將控制權交還給先前的進程。在 Trap 期間，Process 的執行優先於使用者程式碼。當作業系統偵測到 Trap 時，它會暫停使用者 Process。系統呼叫完成後，作業系統會恢復並繼續執行使用者 Process。

例如：假設有類似 printf("%s\n", str);。這將呼叫 write 函數將輸出列印到標準輸出（即監視器）。這將觸發一個 Trap 並將控制權發送給 Trap 處理程序。然後，Uaer mode 切換到 Kernel mode，作業系統執行 write 呼叫。任務完成後，控制權從 Kernel mode 返回 User model。

</br>

以下列出的階段可用來總結 Trap 在作業系統中的工作方式：

* 程式執行過程中可能發生的錯誤或異常包括除零錯誤、嘗試存取錯誤記憶體、無效指令。
  * 辨識出錯誤或異常後，CPU 會發出陷阱訊號
 
* CPU 在該訊號的幫助下進入核心模式，並將控制權轉移到 Trap 處理程序，該處理程序是作業系統的預先定義區域
 
* 作業系統中的 Trap 處理程序會決定陷阱的來源並執行適當的操作。這可能包括終止應用程式、告知使用者錯誤或提供者請求的特定功能。
 
* Trap 處理程序完成其任務後，控制權將恢復到創建 Trap 的程序，並且 CPU 返回到使用者模式。

</br>

### Interrupt

Interrupt 由硬體設備引發。USB 裝置、網路卡和鍵盤都可能引發中斷。Interrupt 是<font color=red>**異步**</font>的，因此可能隨時發生。

Interrupt 引腳是 CPU 上的專用引腳，也稱為 INT 腳。中斷引腳將鍵盤等裝置連接到處理器。當按下某個鍵時，會產生中斷。CPU 將從目前程序轉換為中斷處理程序，在這種情況下，將呼叫鍵盤中斷處理程序。完成中斷處理程序程式碼後，CPU 將返回到正在執行的原始程式。發生中斷時，CPU 會切換上下文並執行中斷處理程序。完成後，CPU 將會回到先前的狀態。

</br>

以下列出的階段總結了中斷在作業系統中的工作方式：

* 中斷訊號是由外部硬體設備產生的，用於通知 CPU 需要執行某些操作。
  * 由於 CPU 會暫停目前正在執行的程式並進入核心模式，因此作業系統可能會處理該中斷
 
* 作業系統呼叫其中斷處理程序來處理中斷，當中斷發生時，中斷處理程序會確定原因，並透過從裝置讀取資料或執行其他輸入/輸出 (I/O) 操作做出正確回應。
 
* 中斷處理完畢後，被中斷的程序將從被掛起的地方繼續執行

</br>

### 兩者之間的區別

| Trap | Interrupt |
| :--- | :-------- |
| 陷阱是使用者軟體向作業系統發送的訊號，指示其立即執行特定操作。	|中斷是一種硬體訊號，它通知 CPU 必須立即處理某些緊急事件。|
| 這是一個同步的過程。|	這是一個非同步過程。|
| 它是由使用者程式指令產生的。|	它們是由硬體產生的。|
| 它的另一個術語是軟體中斷。|	它也被稱為硬體中斷。|
| 所有陷阱都會被打斷。|	並非所有中斷都是陷阱。|
| 執行作業系統的專門功能，並將控制權傳遞給陷阱處理程序。|	它強制處理器運行特定的中斷處理程序軟體。|

</br>

## Interrupt 導向

近代的作業系統是 Interrupt 導向（Interrupt driven）的；當作業系統進⼊電腦系統之後，<font color=red>不論是否有程式執⾏，作業系統本身都在閑置狀態</font>，有了 Interrupt 就⽴刻處理。<br>
那需要注意<font color=red>作業系統閒置不代表 CPU 閒置</font>，因為就算作業系統步開啟任何程式，或是執行任何程式，CPU 很有可能還會執行某些系統的背景程式。

</br>

![Interrupt 導向](../images/Interrupt_導向.png#pic_center=100x150)

</br>

## System Calls

叫⽤系統（system calls）是作業系統提供服務的⼀個介⾯（interface）。是指運行在 user space 的程式向作業系統核心請求需要更高權限運行的服務。系統呼叫提供 user space 和作業系統之間的介面。

簡單來說，system call 是 Process 和 OS 之間的介面，當使用者程式需要 OS 的服務時，使用者程式便去呼叫 system call。

* 執⾏叫⽤系統時會產⽣⼀個 Trap，於是使⽤叫⽤系統的程式就會被暫停執⾏。
* 典型的叫⽤系統服務有：
  * Process控制：譬如產⽣和摧毀 Process
  * 檔案管理：譬如開檔和結檔
  * 設備管理：讀取或寫⼊資料
  * 資訊維護：譬如取得⽇期或時間
  * 通訊：譬如收發訊息

要建立的實際的 system call，硬體需要提供的一個特殊的指令，以產生 trap 而從 user space 轉入 kernel space。

例如 x86_64 的 syscall，arm64 的 svc #0，或者是 RISCV 使用的 ecall。細節上，這個指令會把 program counter 轉向 kernel 所定義的入口點，例如 x86 的入口為 entry_SYSCALL_64，地址在啟動時透過 wrmsrl(MSR_LSTAR, (unsigned long)entry_SYSCALL_64) 儲存於 MSR_LSTAR，後者是一個 Model-specific register)。然後 kernel 就可以透過相應的 register 取得 system call number 和參數，找到相應的 system call handler 給予對應的服務，之後根據 ABI 所定義的 register 設置返回值，最後重設回當初產生 trap 之指令的下個 program conter 位置。

用比較日常發生的事情來說，像是我們去超商取貨，一定是告訴店員手機末 3 碼以及名字，然後店員找到包裹後再拿給我們確認。這種翻包裹的動作不會讓我們自己拿，因為可能有其他人包裹的安全性之類的考量。跟店員說要取包裹就像是一種 system call，而提供手機末 3 碼及名字就是相關參數，包裹就像是結果。

</br>

### 表示與解釋 System Call ~

glibc 等 standard library 對 system call 都做了很好的封裝，例如以下經典的 hello world! 程式：

```c
#include <stdio.h>

int main(){
    printf("hello world!\n");

    return 0;
}
```

其中，`printf` 就是一種 System Call，他是使用到封裝的 write 進行 system call。透過這樣的封裝，我們可以不必寫直接寫組語設定相關的 register 並且呼叫特殊的指令產生 trap。

這也讓標準函式庫可以針對不同的系統環境實作背後的細節，而應用程式的撰寫者只要使用一致的介面即可。此外，函式庫也可以受益於此避免不必要的 system call，例如 getpid 的結果可以只有第一次真正使用 system call，後續則可以 cache 住該值並且直接返回即可。

</br>

### 叫用系統機制

回想一下作業系統的意義：提供一個抽象層，讓使用者能夠在一個方便的環境下使用或開發應用程式，這個方便的環境就是 user space。

不同於 kernel space 的程式必須面對 CPU 的指令集和對各式各樣的硬體打交道，user space 的應用程式若需要與作業系統或硬體相關的功能，就必須要仰賴系統呼叫。

而在 Linux 中依照其權限分為兩種模式 － user mode 和 kernel mode。目的為保護系統安全，一般在 user space 的應用程式不允許直接對 kernel space 的資料做存取。

</br>

System Call 是怎麼進行參數傳遞的，分為三種方法：

* 利用 Registers 儲存參數
  * Pros : 快速
  * Cons : register 數量有限，參數不能太多
* 將參數存在 Memory Block 後，把該 block 的起始位址存在一個 register 中，之後將此 register 傳遞給 OS
  * Pros : 可存參數較多
  * Cons : 速度較慢
* 利用 system stack 儲存參數，藉由 push 來保存參數，pop 來取出參數
  * Pros : 可存參數較多
  * Cons : 速度較慢

</br>

![叫用系統機制流程](../images/叫用系統機制流程.png#pic_center=100x150)

</br>

上面這圖為執行 system call 所經過的流程：
1. 在使用者程式中呼叫 system call
2. 藉由一個軟體中斷 trap (svc #0) 進入 kernel mode，此時系統會將 mode bit 由 user mode 改成
3. kernel mode (1 -> 0)
4. 查詢 system call table 來找尋對應的 trap service routine
5. 當執行完 trap service routine 後發出中斷通知 OS 已經完成

</br>


## Timer

一個作業系統之所以能運作，仰賴著某種計時的機制，通常會借助硬體振盪器產生週期性訊號，並透過軟體計數。

Linux 核心在系統啟動之際，會參照 HZ 的數值，將計時器中斷次數對應於 jiffies 數值，在核心計算後換算為時間間隔，這也是排程的時間依據。不過現代的 Linux 核心已不只如此，在 tickless kernel (即 Dynamic Tick Timer，簡稱 dyn-tick) 的引入後，新型態的 NO_HZ 處理機制就大異於典型週期 tick。

```
關於 jiffies 數值：

全域變數 jiffies 用來記錄來自系統啟動以來產生的節拍總數。 

啟動時，核心將該變數初始化為 0，在 1s 內時脈中斷的次數等於Hz

所以 jiffies 1s 內增加的值為 Hz。 

系統執行時間以秒 s 為單位，等於jiffies/Hz。 

如果在應用程式中需要測量程式執行時間，則可以利用 Linux 的 timeval 結構來實作。
```

HRT (high-resolution timer) 的引入，除了帶來微秒 (microsecond) 等級的時鐘精準度，更將 Linux 核心的時間管理機制推上另一個新層次，不僅大為強化系統分析的精準度，也是 Linux 核心強化即時處理的關鍵特徵。

</br>

作業系統必須控制 CPU 的使⽤⽅式，不能讓⼀個使⽤者程式⻑期佔⽤ CPU ⽽不叫⽤系統的服務（譬如輸出和輸⼊）。

系統⼀般有兩種計時器：實時時鐘（Real Time Clock）和區間計時器（Interval Timer）。實時時鐘和牆上的鐘和⼿錶無異。
* 區間計時器是個倒數的，當值降到 0 時會產生一個 Interrupt。

</br>

當⼀個使⽤者程式開始（或繼續）執⾏前，OS 在區間計時器上設定⼀個很⼩的時間值（time quantum），然後讓程式執⾏。

同時，區間計時器開始倒數，⼀旦到 0 時，Interrupt 就中斷該程式的執⾏⽽把控制權交給 OS、讓 OS 採取適當的處理。這樣，⼀個⼀直佔⽤ CPU 只做計算⽽沒有其它⾏為的程式就無法⻑期佔⽤CPU。

</br>

## 讀取-解碼-執行

這裡是解釋 CPU 如何執行**程式**，CPU 執⾏⼀道機器指令時會有幾個階段：讀取（fetch）、解碼（decode）和執⾏（execution）。

我們可以把這三個階段再細分如下：
* 讀取（fetch）：從記憶體中把下一道指令抄入 CPU
* 解碼（decode）：分析該指令並且決定運算碼（operation code）和運算元（operand）
* 載入運算元（load operands）：把運算元的內容從記憶體或暫存器（register）抄入 CPU
* 執行（execute）：執行運算碼指定的工作並且得出結果
* 儲存（save）：把得來的結果存⼊暫存器和記憶體

</br>

```
這裡很重要！！！
```

</br>

大多數現代處理器的工作原理是取譯碼執行（Fetch-Decode-Execute Cycle）。

也被稱為馮·諾依曼架構（Von Neumann Architecture）。這是一種將程式指令記憶體和資料記憶體合併在一起的電腦設計概念結構。下圖為馮諾依曼架構框架：

</br>

![Von Neumann Architecture](../images/VonNeumannArchitecture.png#pic_center=100x150)

</br>

簡單來說，馮諾依曼架構由五個元件組成，分別為輸入(input)、輸出(output)、運算器(Arithmetic Logic Unit)、記憶體(Memory)、控制器(Control Unit)，其中，運算器、控制器都在CPU中央處理器中。記憶體也可以分為記憶體、外存。

</br>

1. 讀取（Fetch）
* 這就是裝載需要執行的指令到IR的過程。
* 首先將PC暫存器的內容裝入MAR，並且將PC暫存器的值 +1 (因為我們完成目前這條指令之後還需要繼續進行下一條，就是透過這種方式來實現指令的順序執行的)。然後將位址指向的內容裝入MDR，最後控制單元將MDR中的內容送入IR，至此，我們完成了指令的讀取。

2. 解碼（Decode）
* 在這個步驟中，指令由處理器解碼。
* 如果指令需要，處理器將獲得任何操作數。例如，指令MOV AX, 0。將值 0 儲存在 Ax 暫存器中。在執行指令之前，處理器將從記憶體中的下一個位置取得常數值 0。

3. 執行（Execute）
* 在最後一個階段，處理器執行指令，它在暫存器 AX 中儲存 0。
* 處理器執行指令 MOV AX, 0。最後，它調整指令指標指向儲存在位址 0102 的下一條要執行的指令。

</br>

### 指令集(Instruction Set)

硬體 (Hardware) 與軟體 (Software) 溝通的橋樑，也就是指令集，為什麼可以說是溝通的橋樑？</br>
因為軟體與硬體彼此不需要去知道對方的資訊，只要知道指令集的格式就可以進行溝通了，編譯器依據指令集的格式把軟體編譯成硬體看得懂的機器碼，而硬體只要依據這個機器碼去執行相對應的運算，就可以完成我們想要的操作。

指令集其實就是我們平常在操作電腦時讓 CPU 去執行的指令，我們執行的程式最後變成由 0 和 1 所組成的機器碼讓 CPU 去讀取，那這個機器碼就是一道道的指令。

以 C 語言為例，會經過：前處理 (Preprocessing) -> 編譯 -> 組合語言(assembly code) -> 組譯 -> 機器碼(machine code) -> 連結 (Linking)

### CPU Pipeline

![CPU Pipeline](../images/CPU_Pipeline.png#pic_center=100x150)

</br>

#### IF (Instruction Fetch)

Fetch 指令，先到這個要執行指令的 address，去將該指令從 memory 提取到 CPU。

</br>

#### ID (Instruction Decode)

Decode，根據這道指令 0 和 1 的組成去判斷這道指令的內容。

</br>

#### EX (Execution) 

CPU 知道這道指令的內容之後，就要執行相對應的運算，這裡由算術邏輯單元 (ALU)，根據這道指令的目的、輸入去計算出結果，需要計算的不只是有運算指令，還包含了 load store 指令，因為要計算出 memory 的 address，才能夠去進行訪問 (Access) 對 memory 進行讀寫，這裡還不包含 memory 讀寫。

</br>

#### MEM (Memory access)

memory 讀寫的部分就是由 MEM stage 進行，在這裡向 ram 或是 cache 及外部的 memory 進行 access 並讀寫，將 CPU register 資料寫出去或者是將資料讀到 register 內。

</br>

#### WB (Write-Back)

一道指令的完成，如果是計算指令會將運算後的結果寫回 destination register。

</br>

### 小結

在這裡可以觀察到一個很直觀的問題，在並行計算中，若是多個 Process 有計算又同時需要共用這個計算結果的資源，那我們該如何知道哪個計算結果才是對的？也就是所謂的資料衝突（Data Hazards）。

資料衝突（Data Hazards）：這指的是一道指令依賴已經被抄入 pipeline 的指令的結果第⼆道指令中的 C 使⽤到第⼀道已經被抄⼊ pipeline 的指令的結果。

</br>

## 特殊機器指令

* Atomic 指令在執行時不容許有交錯執行或被其它指令分割。當一道 atomic 指令被 CPU 偵測到並且執行時：
  * 所有在 CPU 內各階段的其它指令都會被暫停、等到這一道 atomic 指令執行完畢後才會被繼續，而且有些指令（譬如用到 atomic 指令產生的結果的指令）可能會重頭來過（至少得再次抄入它所使用的各運算元）。
  * Atomic 指令在執行時不能被 Interrupt 打斷，必須從頭做到尾、一氣呵成。
  * 如果有若⼲道 Atomic 指令進⼊ CPU 或核⼼，它們都會被⼀個接⼀個地順序執⾏，但順序為何則是由硬體決定。這種情況通常在有若⼲個 CPU 或核⼼的系統中出現。

</br>

# Process 基本概念

## 從編譯到執行

這邊有點需要 Bare-Metal 的概念 ~

* 編譯程式（compiler）把原始程式編譯到 .o 檔案。
* 連結程式（linker）把.o 檔和其它程式庫函數串成⼀個可執⾏檔（譬如 a.out）。
* 載⼊程式（loader）把可執⾏檔抄⼊記憶體準備執⾏。

</br>

![從編譯到執行 流程](../images/從編譯到執行_流程.png#pic_center=100x150)

</br>



</br>

# 參考資料：

[冼鏡光並行計算講堂](https://pages.mtu.edu/~shene/VIDEOS/CONCURRENT/index-TW.html)

</br>

[Linux 核心設計: System call](https://hackmd.io/@RinHizakura/S1wfy6nQO)

</br>

[System Call (系統呼叫) - 從零開始的開源地下城](https://hackmd.io/@combo-tw/Linux-%E8%AE%80%E6%9B%B8%E6%9C%83/%2F%40combo-tw%2FBJPoAcqQS)

</br>

[Linux 核心設計: Timer 及其管理機制](https://hackmd.io/@sysprog/linux-timer)

</br>

[CPU Pipeline](https://ithelp.ithome.com.tw/m/articles/10327694)

</br>