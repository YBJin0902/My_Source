# RTOS

以 Free RTOS 的 open source 實作，搭配 STM32 與 HT32 系列開發版

</br>

# 簡介與基礎認知

### 前後台系統

前後台系統常建設在一般的程序上，簡單來說就是直接裸機操作 (直接套用while無限回圈內)。
須注意此系統並沒有嵌入式操作系統的改念

* 前台系統：中斷 (interrupt) 用於處理系統的異步事件 (callback)。

* 後台系統：程序是個死循環，在循環中不斷調用 API 函數完成所需的事件。

```
NOTE !! ：

前台系統為中斷級，後台為任務級
```

</br>

![Backend_system_digram](https://hackmd.io/_uploads/B1GfZ6k90.png)

</br>


### RTOS 系統

即為即時性作業系統，並強調即時性，又分為軟即時與硬即時。將需要實現的功能劃分為多個任務，並具有可剝奪性。

代表作：FreeRTOS、UCOS、RT-Thread、DJYOS

* 軟即時：系統能讓絕大多數任務在確定時間內完成。

* 硬即時：系統必須使任務在確定的時間內完成。

* 可剝奪性：CPU執行多個任務中優先權最高的那個任務，即使CPU正在執行某個低階任務，當高階任務準備好時，高階任務就會優先搶奪執行權。

</br>

![Front_desk_system_diagram](https://hackmd.io/_uploads/Hy7mZ6y9A.png)

</br>

# Free RTOS

## 簡介

FreeRTOS 是一個可剪裁、可剝奪型的多任務核心，而且沒有任務數量限制。

官網：https://www.freertos.org/index.html


## 原始碼

下載：https://freertos.org/a00104.html

* 核心程式碼：(Source)
    * tasks.c：掌管所有 task 的檔案
    * queue.c：管理 task 間的通訊
    * list.c：提供系統與應用實作會用到的 list 結構

* 硬體相關檔案：以 ARM Cortext-M3 為例，可在 Source/portable/GCC/ARM_CM3 中找到
    * portmacro.h：定義了與硬體相關的變數，如資料型態定義，以及與硬體相關的函式呼叫名稱定義(以 portXXXXX 命名)等，統一各平臺的函式呼叫
    * port.c：定義了包含與硬體相關的程式碼實作
    * FreeRTOSConfig.h：包含 clock speed, heap size, mutexes 等等都在此定義(需自行建立)

</br>

### 架構
FreeRTOS 的程式碼可以分為三個主要區塊：任務、通訊和硬體界面。

* 任務(Tasks)：FreeRTOS 的核心程式碼約有一半是用來處理多數作業系統首要關注的問題：任務，任務是擁有優先權的用戶所定義的 C 函數。task.c 和 task.h 負責所有關於建立、排程和維護任務的繁重工作。

* 通訊(Communication)：FreeRTOS 核心程式碼大約有 40% 是用來處理通訊的。queue.c 和 queue.h 負責處理 FreeRTOS 的通訊，任務和中斷(interrupt)使用佇列(佇列，queue)互相發送數據，並且使用 semaphore 和 mutex 來派發 critical section 的使用信號。(~~資料結構很重要~~)

* 硬體介面：同一份程式碼在不同硬體平台上的 FreeRTOS 都可以運行。大約有 6% 的 FreeRTOS 核心代碼，在與硬體無關的 FreeRTOS 核心和與硬體相關的程式碼間扮演著墊片(shim)的角色。

</br>
</br>

## 概念

在開始學習 RTOS 作業系統前，需要先了其解如何運作，如下圖所示為基本運作圖，展示了各 Task 如何交互與之間的關係

</br>

![FreeRTOS-TaskWork](https://hackmd.io/_uploads/ryZU-61c0.png)


1. 方格為狀態：
  * 運行：正在由 CPU 執行的狀態
  * 就緒：準備要執行的下一個狀態
  * 阻塞：在等待某個事件的狀態
  * 掛起：透過程式 API 要求退出之狀態

2. 方格之間的箭頭為需要透過地關係 (API)
  * Rusume：恢復狀態
  * Suspend：掛起狀態
  * Event (事件)：等待的事件
  * 阻塞函數：需要被等待的事件

</br>

經由第 2. 點可以發現，Rusume 與 Suspend 為相互的；Event 與 阻塞函數也為相互的。

</br>

接下來由流程的方式說明 RTOS 架構如何運作：

當一個任務被創建時 (視為黑點)，會處於就緒狀態。

任務被創建之後，使用者可以選擇要進行的動作，有兩種：運行、掛起；首先運行，就是直接運行該運務之內容；最後，掛起就是先將任務放置在一旁不進行運行，直到使用者需要時再透過 API 呼叫回復運行狀態。

其中較為特別的是阻塞，透過 API 的呼叫使任務被阻塞使得其他任務可以執行，這有關乎 RTOS 運作之關係， RTOS 之重點就在這，透過多個任務不斷互相阻塞與運行狀態達成類多執行序的效果。

</br>

---

</br>

以上說明可能較為難懂，以下由比喻的方式說明：

前情提要：
* 假設你的大腦是一個 MCU ，首先，你創建了三個任務分別是：動左腳、動右腳與躲避石頭。
* 當前情境為，你需要動左右腳進行走路，並且可能隨時會有人朝你丟石頭，所以你需要躲避。

</br>

各任務內容：
* 動左腳：抬起左腳 >> 放下左腳 >> 阻塞
* 動右腳：抬起右腳 >> 放下右腳 >> 阻塞
* 躲避石頭：躲避 >> 掛起
* 優先級：躲避石頭 > 動左腳 > 動右腳

</br>

開始執行：
* 創建各任務，其中躲避石頭創建完成後先掛起，因為躲避的動作不用時時刻刻進行；初始化完成，首先執行優先權級最大之任務，動左腳，當抬起左腳執行完成後，執行下一步驟放下左腳，此時進入阻塞狀態，系統開始尋找次優先權級之任務，動右腳，執行內部動作，執行完成後，再次尋找下一優先級，在這樣不斷得循環之中形成走路之動作。
  
* 突然有人朝你丟了石頭，你的大腦接收到了需要將躲避石頭恢復執行之動作，於是 Resume 了躲避石頭的任務，並將其執行，在執行完躲避的程序後，再將該任務掛起，因為並不需要一直處於躲避的狀態，直到下次需要應對時在恢復執行就好。

</br>

以上為基本的 RTOS 執行，實際上在撰寫與設計的過程中需要注意很多地方，例如：與中斷的交互、阻塞的等待事件、掛起與恢復執行的時間點...等等。

</br>

---

</br>

## 資料型態與命名規則

Free RTOS 不只是簡單的軟體流程而已，他還與硬體設備有關。

在不同的硬體設備上，通訊埠設定也會不同，其設定會在一個 portmarco.h 的標頭檔裡，有兩種特殊資料型態 portTickType 與 portBASE_TYPE。

* 資料型態：
  * portTickType：儲存 tick 計數值，可以用來判斷 block 次數。
  * portBASE_TYPE：定義為架構基礎的變數，隨各不同硬體來應用，如在 32Bit 架構上，其為 32Bit 型態，最常用以儲存極限值或布林數。

* 函式
  * 以回傳值型態與所在檔案名稱為開頭
    * vTaskPriority() 是 task.c 中回傳值型態為 **void** 的函式
    * xQueueReceive() 是 queue.c 中回傳值型態為 **portBASE_TYPE** 的函式
  * 只能在該檔案中使用的 (scope is limited in file) 函式，以 prv 為開頭 (private)

* 巨集
  * 巨集在 Free RTOS 裡皆為大寫字母定義，名稱前小寫字母為巨集定義的地方
    * portMAX_DELAY : portable.h
    * configUSE_PREEMPTION : FreeRTOSConfig.h

</br>

# Basic Knowledge

## 多任務處理

Kernel 是作業系統的核心架構，Linux、Windows 等通用的作業系統所使用的核心允許多個用戶看似同時訪問電腦的處理器，而用戶可以各自執行多個程序，並看起來像是同時發生。

使用多任務處理可以簡化原本複雜的應用程序：

* 作業系統的多任務處理和任務之間溝通的功能允許將複雜的功能劃分為更小且易於管理的一組任務
* 確保團隊分工明確
* 複雜的時序與排序細節由 RTOS 負責

</br>

常規的單核心處理器一次只能執行一個任務 (也就是所謂單執行序，如：MCU)，但多任務操作系統可以快速切換任務，使所有任務看起來像是同時執行。

用三個任務舉例，以下為同步執行
![image](https://hackmd.io/_uploads/By3sZSbH1e.png)

以下為多任務執行
![image](https://hackmd.io/_uploads/HkMpZSWrJe.png)

</br>

## How Does RTOS Work ?

在 RTOS 任務會被分成兩類：real-time 與 non-real-time 這兩種。

Real-Time 的任務會有一個期限，必須在該期限內完成任務內容以確保系統的正常運作。

Non-Real-Time 的任務則沒有嚴格的時序要求，可以在系統資源可用時執行。

</br>

# Kernel Developer

## 任務 (Task)

為 Free RTOS 中執行的基本單位，每一個任務都由 C 的函數組成。

當使用 RTOS 的即時應用程式可以結構化一組獨立的任務，也就是每創建一個任務都會是一個單獨的結構體。

在任何時間點，RTOS 中只能有一個任務在執行，而即時 RTOS 排程器負責決定這個任務應該是哪一個。

因此，當應用程式執行時，RTOS 排程器可能會重複啟動和停止每項任務 (將每項任務交換進來和交換出去)。

由於任務並不知道 RTOS 排程器如何運作，因此即時 RTOS 排程器有責任確保在任務交換進來時的處理器上下文（暫存器值、堆疊內容等）與交換出來時的處理器上下文完全相同。為了達到這個目的，每個任務都有自己的堆疊。當任務被換出時，執行上下文會被儲存在該任務的堆疊中，因此當同樣的任務被換回時也可以完全還原。**簡單來說，當進行到一半的任務的執行權需要交出給下一個任務時 RTOS 會確保被中斷的點再下次該任務換回來時可以持續執行。**

</br>

### States

如上面所提到的可以分為：
* 運行 (Running)
* 就緒 (Ready)
* 阻塞 (Blocked)
* 掛起 (Suspend)

以下詳細解說

</br>

**Running**

當任務實際正在執行時，稱為 Running 狀態。
它目前正在使用處理器。如果執行 RTOS 的處理器只有一個核心，那麼任何時候都只能有一個任務處於 Running 狀態。

---

</br>

**Ready**

就緒任務是指可以**即將**執行的任務 (它們未處於 Blocked 或 Suspend 狀態)，但目前尚未執行，因為優先順序相同或更高的其他任務已處於執行狀態。

也就是執行權目前被其他任務佔有中。

---

</br>

**Blocked**

如果任務目前正在等待時間事件 (與 os 有關的 delay) 或外部事件，則稱該任務處於阻塞狀態。

例如，如果任務呼叫 vTaskDelay()，它會阻塞，直到延遲期間結束為止，這是一種時間事件。

任務也可以阻塞來等待佇列、訊號觸發、事件群組、通知。

阻塞狀態中的任務通常會有「 timeout 」，timeout 後，即使任務等待的事件尚未發生，任務也會超時，並解除阻塞。

**處於阻塞狀態的任務不使用任何處理時間，且無法選擇進入執行狀態。**

---

</br>

**Suspend**

與處於 Blocked 狀態的任務一樣，處於 Suspended 狀態的任務也無法選擇進入 Running 狀態，但是處於 Suspended 狀態的任務沒有 timeout。相反，任務只有在分別透過 vTaskSuspend() 和 xTaskResume() ...等 API 呼叫**明確命令**進入或離開 Suspended 狀態時，才會進入或離開 Suspended 狀態。

---

</br>

#### How to make a TASK

需要先定義一個 C 的函數，然後再用 xTaskCreate() 這個 API 來建立一個 task，此 C 函數有幾個特點，它的返回值必須是 void，其中通常會有一個無限迴圈，所有關於這個 task 的工作都會在迴圈中進行，且函數不會有 return，FreeRTOS 不允許 task 自行結束 ( 使用 return 或執行到函數的最後一行 )。


**建立一個 Task 的任務內容為何：**

```c
void vATaskFunction(void *pvParameters)
{
    for( ;; )
    {
        /* Task application code here. */
    }

    /* Tasks must not attempt to return from their implementing
       function or otherwise exit. In newer FreeRTOS port
       attempting to do so will result in an configASSERT() being
       called if it is defined. If it is necessary for a task to
       exit then have the task call vTaskDelete( NULL ) to ensure
       its exit is clean. */
}
```

</br>

**創建一個 Task：**

```c
xTaskCreate(TaskFunction_t pvTaskCode,
            const char * const pcName,
            const configSTACK_DEPTH_TYPE uxStackDepth,
            void *pvParameters,
            UBaseType_t uxPriority,
            TaskHandle_t *pxCreatedTask);
```

* pvTaskCode：就是我們定義好用來建立 task 的 C 函數
* pcName：任意給定的 task name，這個名稱只被用來作識別，不會在 task 管理中被採用
* usStackDepth：堆疊的大小 (以 Byte 計算)
* pvParameters：要傳給 task 的參數陣列，也就是我們在 C 函數宣告的參數，可以為 NULL
* uxPriority：定義這個任務的優先權，在 FreeRTOS 中，0 最低，(configMAX_PRIORITIES – 1) 最高
* pxCreatedTask：handle，是一個被建立出來的 task 可以用到的識別符號

</br>

## 優先權 (Priority)

在 Free RTOS 中定義最高優先權的變數為 **configMAX_PRIORITIES** ， 位於 FreeRTOSConfig.h

每個任務都會有各自的優先權級，從 0 到 configMAX_PRIORITIES ，根據不同的優先權依序執行 Ready 中的 Task，其中，低優先順序數字表示低優先順序工作，閒置任務的優先順序為零

Free RTOS 的調度程序會確保處於 Ready 或 Running 狀態的任務，總會優先得到處理器 (CPU) 的時間，而不是優先順序較低但也處於就緒狀態的任務。換句話說，置於 Running 狀態的任務永遠是能夠執行的最高優先順序任務。

任何數量的任務都可以共用相同的優先順序。如果未定義 configUSE_TIME_SLICING，或者 configUSE_TIME_SLICING 設定為 1，則具有相同優先順序的就緒狀態任務將使用時間切片輪流排程方案來分享可用的處理時間。

</br>

## 排程 (Scheduling)

Free RTOS 的排程演算法涉及到核心數量，不同數量的核心會牽扯到多種不同記憶體位置、任務調度、互斥量 ... 等問題。

Free RTOS 排程演算法適用於單核心，非對稱多核心 (AMP) 和對稱多核心 (SMP) 的設定。

排程演算法是決定哪個 RTOS 任務應處於 Running 狀態的軟體例行程序。在任何特定時間，每個處理器核心只能有一個任務處於 Running 狀態。AMP 是指每個處理器核心都執行自己的 FreeRTOS 範例。SMP 是指有一個 FreeRTOS 範例在多個核心之間排程 RTOS 任務。

</br>

### 單核心

預設情況下，FreeRTOS 使用固定優先順序的優先排程，並對等優先順序 (equal priority tasks) 的任務進行循環時間切分。

Equal priority tasks：

* Fixed priority：固定優先順序，表示排程器不會永久改變任務的優先順序，雖然它可能會因為優先順序繼承而暫時提升任務的優先順序。

* Preemptive：搶佔執行，是指調度器永遠執行能夠執行的最高優先順序 RTOS 任務，不論任務何時能夠執行。 例如，如果中斷程序 (ISR) 改變了能夠執行的最高優先順序任務，則調度器會停止目前執行的較低優先順序任務，並啟動較高優先順序的任務 - **即使這發生在時間片內**。 在這種情況下，較低優先順序的工作稱為被較高優先順序的工作「搶先執行」。

* Round-robin：循環，是指共享優先順序的任務輪流進入「運行」狀態。

* Time sliced：時間片，表示調度器會在每次 tick 中斷時切換優先順序相同的任務。(tick 中斷是 RTOS 用來測量時間的週期性中斷）。

</br>

```
Using a prioritised preemptive scheduler - avoiding task starvation
```

永遠執行最高優先順序工作的後果是，高優先順序工作若從未進入 Blocked 或 Suspended 狀態，將永遠剝奪所有較低優先順序工作的執行時間。 

例如：如果高優先順序任務正在等待事件，它就不應該在循環中等待事件，因為輪詢讓它一直在執行，所以永遠不會處於阻塞或暫停狀態；相反，該任務應該進入 Blocked 狀態來等待事件，事件可以使用 FreeRTOS 多種任務間通訊和同步 function 之一傳送給任務。 

接收事件會自動將優先順序較高的任務從 Blocked 狀態移除，較低優先順序的任務會在較高優先順序的任務處於 Blocked 狀態時執行。

</br>

#### 設定 RTOS 排程 ：

* configUSE_PREEMPTION
  * 如果 configUSE_PREEMPTION 為 0，則會關閉搶佔權，只有當執行中的任務進入 Blocked 或 Suspend 狀態時才會發生上下文切換，執行中的任務會呼叫 taskYIELD() 或中斷程序 (ISR) 手動要求上下文切換時，才會發生上下文切換。

* configUSE_TIME_SLICING
  * 如果 configUSE_TIME_SLICING 為 0，則時間分割關閉，因此排程器不會在每次 tick 中斷時切換優先順序相同的工作。

</br>

### 非對稱多核心(AMP)

使用 FreeRTOS 的非對稱多核心 (AMP) 是指多核心裝置的每個核心都執行自己獨立的 Free RTOS。 

這些核心不需要擁有相同的架構，但如果 Free RTOS 需要彼此通訊，則必須共用一些記憶體。

每個核心都執行自己的 FreeRTOS，因此任何指定核心上的排程演算法都與上述單核心系統的排程演算法完全相同。 

可以使用串流或訊息緩衝區作為核心間的通訊，這樣一個核心上的任務就可以進入 Blocked 狀態，以等待從不同核心傳送來的資料或事件。

</br>

### 對稱多核心 (SMP)

使用 FreeRTOS 的對稱多核心 (SMP) 是指一個 FreeRTOS 範例在多個處理器核心之間調度 RTOS 裡的任務。由於只有一個 FreeRTOS 範例在執行，所以同一時間只能使用一個 FreeRTOS port，因此每個核心必須有相同的處理器架構，並共享相同的記憶體空間。

SMP 會導致在任何特定時間都有超過一個任務處於 Running 狀態 (每個核心有一個 Running 狀態的任務)。 這表示只有在沒有較高優先順序的任務可以執行時，較低優先順序的任務才會執行的假設不再成立。 

SMP 排程器如何選擇要在雙核微控制器上執行的任務，當一開始有一個高優先順序任務和兩個中優先順序任務都處於就緒狀態時，排程器需要選擇兩個任務，每個核心一個。

首先，高優先級任務是能夠執行的最高優先級任務，因此它會被選取用於第一個核心。剩下的兩個中優先級任務是能夠執行的最高優先級任務，因此會為第二個核心選擇一個。結果是高優先級和中優先級工作同時執行。

</br>

## Co-Routine

共常式（英語：coroutine）是電腦程式的一類組件，推廣了**協同運作式多工**的次常式，允許執行被掛起與被恢復。相對次常式而言，共常式更為一般和靈活，但在實踐中使用沒有次常式那樣廣泛。共常式更適合於用來實現彼此熟悉的程式組件，如協同運作式多工、例外處理、事件迴圈、迭代器、無限列表和管道。 - from Wiki

* 此處將 "Co-Routine" 翻譯為：協同程序

</br>

協同程序僅適用於具有嚴格 RAM 限制的小型處理器，通常不會在 32 位元微控制器上使用。協同程序可以存在於下列狀態之一：

* Running
* Ready
* Blocked

</br>

Free RTOS 中，Co-Routine 相當於無法搶佔的任務，其優先順序均低於任務。

因為其非搶佔性，因此其所需的系統資源較少，不需要獨立的堆疊空間。

</br>

## Idle Task (空閒任務)

當 RTOS 調度程序開始後，會自動建立空閒任務，以確保始終只有一個任務可以執行。

盡可能低的優先權創建，以確保在有高優先權的任務處於 Ready 時，不會占用任何 CPU 資源。

空閒任務負責釋放 RTOS 分配給已刪除任務的記憶體。

```
在使用 vTaskDelete() 時，確保空閒任務不會缺乏處理時間非常重要 !!
```

**空閒任務沒有其他活動功能，因此在所有其他條件下可以合理地缺乏微控制器時間。**

</br>

### HOOK

空閒任務 HOOK 是在空閒任務的每個週期期間呼叫的函數。

* 在空閒任務　HOOK 中實現此功能：
    必須始終至少有一項任務準備好運行。
    因此，HOOK　函數必須不呼叫任何可能導致空閒任務阻塞的 API 函數（例如 vTaskDelay()，或具有阻塞時間的佇列或信號量函數）。
    
* 建立一個空閒優先權任務來實現該功能：
    更靈活的解決方案，但需要的 RAM 更高

```c
void vApplicationIdleHook( void );
```

</br>

## Thread Local Storage Pointers

* Thread > 執行緒

### 介紹

Thread Local Storage (執行緒本地儲存或 TLS) 允許應用程式編寫者將值儲存在任務的控制區塊內，使該值特定於任務本身（本地），並允許每個任務擁有自己的唯一值。

TLS 最常用於儲存**單執行緒程式**本來儲存在全域變數中的值。例如，許多程市都包含一個名為 X 的全域變數；如果函式庫函數向呼叫函數傳回錯誤條件，則呼叫函數可以檢查 X 值以確定錯誤是什麼。在單執行緒中，將 X 宣告為全域變數就足夠了，但在多執行緒中，每個執行緒（任務）必須有自己唯一的 X 值；否則一個任務可能會讀取用於另一任務的 X 值。

</br>

# Queue

"佇列" 與資料結構相同概念，有 FIFO 的特性。

在 Free RTOS 中，Queue 當作個任務之間傳遞訊息的媒介，若是不透過 Queue 則很有可能造成任務互斥而卡死。

</br>

* Queue 可以保存有限數量且固定大小的資料。

* 最大容量在 Free RTOS 中稱為 "length"。

* 每個資料的長度與大小都在創建 Queue 時設定。

</br>

### 介紹

假設創建一個 length 為 5 的 Queue ，當資料寫入時會從最後的位置 (尾部) 開始寫入，並從 Queue 的最前頭刪除。也可以使用相反的方式寫入，從最頭寫入，最尾刪除。

![image](https://hackmd.io/_uploads/HkBGddZcR.png)

如圖所示為 Free RTOS 官網所提供之範例。

當資料從 A 任務寫入後再由 B 任務拿出，其中資料不斷跌帶進入 Queue。

```
NOTE !! ：

在設計上通常以 多寫一讀 為主；不太會使用 多讀一寫
```

</br>

### 設計

**讀取：**

當任務嘗試從 Queue 中讀取資料時，可以設定 Block Timeout。

當另一個任務或中斷將資料放入 Queue 時，處於 Block 狀態等待 Queue 中的資料可用的任務會自動轉至就緒狀態，同時，如果在資料可用之前指定的阻塞時間到期，任務也會自動從阻塞狀態轉移到就緒狀態。

**寫入：**

當任務嘗試從 Queue 中寫入資料時，可以設定 Block Timeout，在這種情況下，Block Timeout 是任務保持阻塞狀態以等待空間的最長時間。

Queue 可以有多個寫入器，因此一個 Queue 可能有多個任務被 Block !

UnBlock 的任務始終是等待空間的最高優先順序任務，如果兩個或多個被 Block 的任務具有相同的優先順序，則 UnBlock 的任務是等待時間最長的任務。

#### How to make Queue

**創建 Queue：**

```c
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                           UBaseType_t uxItemSize);
```

* uxQueueLength：Queue 在任一時間可以容納的最大項目數，在同一時間允許有多少個職可以進入
* uxItemSize：每個項目所需要的大小 (以 byte 為單位)

```c
// Ex.

xQueueCreate(5, sizeof(int32_t));

// 建立最多一次可容納 5 個值，每個值都是足夠大以容納 int32_t(32 Bits) 類型的變數。
```

</br>

* 返回值：若建立失敗回傳 NULL

</br>

**將當前 Queue 返回原始狀態**

```c
xQueueReset();
```

</br>

**寫入 Queue：**

有多種方式可以寫入，這邊介紹兩個

從最頭開始寫入：

```c
BaseType_t xQueueSendToFront(QueueHandle_t    xQueue,
                             const void *     pvItemToQueue,
                             TickType_t       xTicksToWait); 
```

* xQueue：使用的 Queue
* pvItemToQueue：欲放入的值
* xTicksToWait：如果 Queue 已滿，任務應進入 Block ，等待 Queue 上的空間變為可用的最長時間。如果設定為 0，則呼叫將立即傳回。

</br>

從最後開始寫入：

```c
BaseType_t xQueueSendToBack(QueueHandle_t    xQueue,
                            const void *     pvItemToQueue,
                            TickType_t       xTicksToWait); 
```

* xQueue：使用的 Queue
* pvItemToQueue：欲放入的值
* xTicksToWait：如果 Queue 已滿，任務應進入 Block ，等待 Queue 上的空間變為可用的最長時間。如果設定為 0，則呼叫將立即傳回。

</br>

**讀取 Queue：**

```c
BaseType_t xQueueReceive(QueueHandle_t     xQueue,
                         void              *pvBuffer,
                         TickType_t        xTicksToWait);
```

* xQueue：使用的 Queue
* *pvBuffer：指向緩衝區的指標，接收到的項目將複製到該 buffer
* xTicksToWait：如果呼叫時 Queue 為空，任務應 Block 等待接收項目的最長時間

</br>

**查詢當前 Queue 項目數：**

兩種查詢方式

查詢當前存在的項目數：

```c
UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue); 
```

</br>

查詢當前存在的空閒空間數：

```c
UBaseType_t uxQueueSpacesAvailable(QueueHandle_t xQueue); 
```

</br>

# Software Timer Management

### 簡介

軟體定時器，用於安排函數在未來的某個設定時間執行，或定期執行。計時器啟動到其回呼函數執行之間的時間稱為計時器的週期。

簡單來說，定時器的 Callback Function 在定時器的週期到期時執行。

此計時器與硬體沒有關系，都是靠 Free RTOS 進行軟體運算。

該功能位於 FreeRTOS/Source/timers.c

相關函數，皆位於 FreeRTOSConfig.h：

* configUSE_TIMERS
    
* configTIMER_TASK_PRIORITY
    設定定時器之優先權，需介於 0 ~ (configMAX_PRIORITIES - 1)

* configTIMER_QUEUE_LENGTH
    設定一個定時器使用的 Queue ，其設定值用於容納未處理指令的最大數量

* configTIMER_TASK_STACK_DEPTH
    設定一個給定時器 Task 的大小
    
</br>

### 效率考量

* 簡易功能實現起來很容易，但高效實現起來卻很困難。

* FreeRTOS 實作不會從中斷程序執行定時器的 Callback，不會消耗任何處理時間。

* 除非定時器實際上已超時，不會向晶片本身計時中斷添加任何處理開銷。

* 此計時器利用現有的 Free RTOS 功能，允許將計時器功能新增至 API 中。

</br>

**Timer Callback Function**

軟體計時器 Callback function

* 軟體定時器需要從頭到尾都處於 Running 狀態，不能進入 Blocked

```c
void ATimerCallback( TimerHandle_t xTimer );
```

</br>

## 軟體計時器的屬性與狀態

### Timer 週期

* 此處的周期指的是計時器啟動和計時器 Callback 之間的時間

#### One-shot and Auto-reload Timers

* 一次性和自動重新加載計時器

* 一次性：
    * 只要一啟動，計時器僅執行 Callback Function
    * 可手動重新啟動，但不會重新啟動

* 自動重新加載：
    * 只要一啟動，計時器就會週期性的執行 Callback Function

![image](https://hackmd.io/_uploads/SJDpLQS9A.png)

</br>

### Timer States

* Dormant (休眠)：
    * 計時器存在
    * 未運行
    * Callback 不會執行
    * 可以透過 API 喚回

* Running (執行)：
    * 正在運行的計時器，將在等於其週期的時間後執行其 Callback
    * 計時器進入運行狀態，或者自上次重置計時器以來

</br>

* 下圖為 Auto-reload

![image](https://hackmd.io/_uploads/H1xRPQrcR.png)

</br>
</br>

* 下圖為 One-shot

![image](https://hackmd.io/_uploads/HJyzOXBcC.png)

</br>

**Timer Delete**

刪除軟體計時器

* xTicksToWait：即將被刪除的任務需要等待的阻塞時間，如果在開始啟動前就調用 xTimerDelete ，則 xTicksToWait 會被忽略

```c
BaseType_t xTimerDelete( TimerHandle_t xTimer, TickType_t xTicksToWait );
```

* 回傳：
    * pdPASS
    * pdFAIL

</br>

## 撰寫 Software Timer

將其有最簡單的話來說就是：用軟體的方式做計數，並加入中斷計數器程序（Interrupt）的概念。

### RTOS 守護程序任務

專用的「Tmr Svc」（定時器服務或守護程序）任務，它維護軟體定時器的有序列表（Timer List）

</br>

![image](https://hackmd.io/_uploads/ByseMd890.png)

</br>

每當 Timer List 中的定時器到期時，任務就會從 Timer List 中知道他要喚醒的時間。當計時器到期時，計時器的任務將呼叫其 Callback Function。

</br>

RTOS Daemon (Timer Service) Task，在以前叫做 Timer Service Task ，最一開始只用來執行軟體的 Callback Function

Daemon Task 在調度程序開始時自動創建
* 優先級大小由 configTIMER_TASK_PRIORITY 與 configTIMER_TASK_STACK_DEPTH 設定，位於 FreeRTOSConfig.h

Callback Function 中不得調用 Free RTOS API ，否則任務會進入 Blocked 狀態

</br>

### Timer Command Queue

定時器 API 會將命令從調用任務發送到 Timer Command Queue 的 Queue ，如下圖所示

</br>

![image](https://hackmd.io/_uploads/SyxAyuU5R.png)

</br>

```
NOTE !! ：

Timer 的資源相當龐大 !!

若不需要請不要使用!!
```

</br>

#### How to make Timer

**創建 Timer**

* Free RTOS 中包括了 xTimerCreateStatic() API 用於分配靜態時脈所需的記憶體，若使用靜態記憶體則必須在使用前明確建立。

```c
TimerHandle_t xTimerCreate(const char * const pcTimerName,
                           const TickType_t xTimerPeriodInTicks,
                           const BaseType_t xAutoReload,
                           void * const pvTimerID,
                           TimerCallbackFunction_t pxCallbackFunction);
```

* pcTimerName：給予計時器的名子，純粹是作為輔助用途
* xTimerPeriodInTicks：計時器的週期，以毫秒為單位，不能為 0
* xAutoReload：設定 pdTURE 為 Auto-Reload；pdFALSE 則相反
* pvTimerID：給予該函數一個 ID 值，提供定時器特定的儲存
* pxCallbackFunction：一個 C 函數，撰寫 Callback Function 內容

</br>

**Timer 開始調用**

* 用於啟動處於休眠狀態的軟體計時器，或重置（重新啟動）軟體計時器

* 可以在調度程序啟動之前調用；但是當調用完成後，軟體定時器將不會實際開始，直到調度程序啟動的時候才會。

```c
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
```

</br>

**Timer ID**

每個計時器都有 ID，ID 為 void 指標，可以直接儲存一個整數值，或是用作函數指標。

* vTimerSetTimerID()：更新 Timer ID 

* pvTimerGetTimerID()：查詢用

</br>

```c
void vTimerSetTimerID( const TimerHandle_t xTimer, void *pvNewID );
```

* xTimer：Timer Handle
* pvNewID：ID 設定值

</br>

```c
void *pvTimerGetTimerID( const TimerHandle_t xTimer );
```

* 回傳：正在查詢的軟體定時器的 ID

</br>

**更改 Timer 週期**

* 使用 xTimerChangePeriod() 函數更改軟體計時器的週期。

* 不能在中斷程序中呼叫該 API。

</br>

```c
BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,
                              TickType_t    xNewPeriod,
                              TickType_t    xTicksToWait);
```

* xTimer：對象 Timer Handle
* xNewPeriod：新週期

</br>

**重製 Timer**

* 使用 xTimerReset() API 函數重設計時器。

* 也可以用在啟動 Timer。

* 不能在中斷程序中呼叫該 API。

</br>

```c
BaseType_t xTimerReset( TimerHandle_t xTimer, TickType_t xTicksToWait );
```

</br>

# 中斷管理

在 Free RTOS 中允許中斷處理程序，本篇章主要說明如何合理規劃任務與中斷、優先級...等。

首先，先複習一下 Free RTOS 中的 Task，任務使跟硬體無關的純軟體程序；但是，"中斷" (Interrupt) 是與硬體有關的，硬體控制哪個中斷程序將運行以及何時運行。

Free RTOS 中任務只會在沒有 ISR 正在執行中運行；因此，最低有先權的中斷事件也會中斷最高優先權的任務，而且任務無法搶佔 ISR。

### 簡介

許多的 Free RTOS API 無法在中斷中使用，若要使用相關 API 請選擇 API 後方加上 FromISR 的函數。

