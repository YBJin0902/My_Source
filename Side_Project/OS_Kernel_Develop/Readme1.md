# OS Kernel Develop

### 介紹

主旨為開發與研究 Operating System（OS、作業系統）

目前針對對象：Linux 發行版

#### 這份筆記的讀法

我會切分成很多不同的檔案說明較為專業的項目，像是 Linux Kernel 中的各項、OS 的執行續 ... 等。

先讓我們有個基礎的認知與了解 ~

若有實作的部分請一定要在 Linux 的 OS 上運行，最好也把自己的作業系統換成 Linux。

</br>

#### 基本認知

請先了解 Bare-Metal 之後再來繼續深入研究。

先讓我們從自己的主機中開始了解，一個主機的組成我們會需要 CPU(SoC)、RAM、Disk、網路、主機板、滑鼠以及螢幕等周邊組成。

那當我們一開機自己的主機時，通常都會直接進入我們所裝設好的作業系統，那問題來了，從我按下開機鍵到螢幕顯示中間經過了甚麼，甚至在我操作作業系統得期間發生了甚麼，都是我們可以去學習了。

那讀資訊工程的各位也一定都知道，一個作業系統通常會被打包成 image 或是 iso，那如何製作這個映像檔也是我們可以自己製作的。

</br>

這裡步講述任何的程式，除了 build image 之外！！

---

</br>

# Linux 簡介

作為一個我們平常可能會去使用的作業系統，現在要去學習的是：
1. 如何製作 image/iso
2. 硬體與整體架構（軟體與韌體）
3. Kernel 中的功能應用與開發
4. OS 中的任務運作

</br>

那先讓我們來聊聊整體 Linux 的架構。

</br>

# Linux 架構

![linux arch](images/linux%20arch.png)

</br>

直接看圖可能會很抽象，先讓我以我們常用的 Linux OS 主機為例，一個主機會有 SoC、滑鼠、鍵盤...等硬體相關，再來就是作業系統部分，假設我們裝的是 Ubuntu 作業系統（現行Linux LTS），我們就會有（先不看 bootloader）：

* User space：APP，這些使用者應用程式可以是預設也可以是後來安裝的。
  
* Kernel space：操作系統核心運行的空間，擁有高等級的控制權限，用於直接管理硬體資源和系統調用。
  * 核心空間包括核心（Kernel）本身和核心模組（Kernel Modules），負責處理進程調度、記憶體管理、 Driver 程序操作、網路堆疊等關鍵功能。

* Hardware：Linux系統的物理基礎，包括但不限於以下幾個關鍵部分：
  * CPU: 負責處理指令和控制其他硬體元件。
  * 記憶體: 存儲程序和數據，供CPU快速訪問。
  * I/O 裝置: 包括硬碟、網路介面卡等，負責數據的輸入輸出操作。
  * 外部連接介面: 如USB、串列埠等，用於連接外部裝置。

</br>

詳細說明這三部分：

* 用戶空間 (User Space)

  * 用戶空間是應用程序運行的地方。

  * Linux 系統中執行所有用戶程序和應用程序的區域。與核心空間相比，用戶空間具有一定的限制和保護機制。

  * 程序執行與硬體訪問
    * 隔離與保護：用戶空間中的程序不能直接訪問記憶體和硬體資源。這種隔離提供了保護，防止用戶程序對系統的整體穩定性造成影響。
    * 通過核心空間訪問硬體：當用戶空間的程序需要與硬體互動時，它們必須通過核心空間來實現。這意味著所有硬體訪問請求都需要經過核心的管理和控制。

  * 記憶體訪問
    * 系統呼叫限制: 用戶空間的程序或程序只能通過系統呼叫來訪問一部分記憶體。這種設計確保了記憶體訪問的安全性和有效性。
    * 保護機制: 由於有完整的保護機制，用戶模式下的崩潰通常是可恢復的，不會影響到系統的其他部分。

</br>

* 核心空間 (Kernel Space)

  * 負責管理硬體資源和系統調用

  * 硬體獨立性：Kernel 與硬體無關，對於 Linux 支持的所有硬體處理器（如 Intel、ARM、Atmel 等）都是通用的。
    * 資源管理：Kernel 在 Kernel Space 內充當資源管理員，執行程序管理、檔案管理、記憶體管理、中斷處理、程序調度等任務。
    * 強大的結構: Kernel 是一個強大的結構，能夠處理各種操作，從基本的檔案讀寫到複雜的網路通訊和硬體控制。

</br>

* 硬體平台就不多贅述了，就是平常見的那些~

</br>

</br>

### 補充 Bootloader

Bootloader，簡單來說就是開機時我們所執行的第一個程式，主要功用是用來載入作業系統的核心，在後續的 Kernel 會詳細說明。

</br>

</br>

# Linux 組成

萬事起頭難，先讓我們好好深入了解 Linux 的架構與組成。

首先，在我們已經對架構有基本認知之後，來了解一下 Linux 的四個大部分：
1. Linux Kernel
   * Linux Kernel 是作業系統的「核心」，負責硬體資源管理與提供系統呼叫給使用者空間的程式使用。
   
2. GUN tool
   * GNU 提供 Linux 上的使用者空間工具與編譯系統，使 Kernel 成為一個完整作業系統。
  
3. 圖形化桌面環境
   * 在 Linux 上，桌面環境是「使用者與電腦互動的圖形介面層」，不屬於 Linux 核心，而是運行於其上的應用程式集合。

4. 應用軟體
   *  Linux 作業系統上的各式軟體，包括 CLI 工具、GUI 應用程式與 Server/Daemon。

</br>

</br>

## Linux System

下圖為 Linux 系統架構的分層圖，Linux 系統中各個元件的角色與層次關係說明。

![linux 系統組成](images/linux_sys_build.png)

</br>

須注意這裡是在說明 Kernel，Kernel 本身也有層次這裡初步作說明：

</br>

#### 最底層：loadable kernel modules

* 可載入的核心模組（Loadable Kernel Modules, LKM）

* 是動態加入 Linux Kernel 的模組，不需重新編譯整個 kernel

* 常見模組：裝置驅動（如 USB、網卡）、檔案系統模組（如 ext4、vfat）

#### 中層：Linux Kernel

* Linux 系統的核心，管理所有硬體資源與提供系統呼叫介面給應用程式

* 包含 CPU 行程管理、記憶體管理、檔案系統、網路協定、系統呼叫機制等

#### 上層：system shared lib

* 系統共享函式庫，如：glibc, libm, libpthread, libstdc++ 等

* 用來讓使用者空間應用程式呼叫系統功能，提供抽象化 API

* 範例：printf(), malloc(), fopen() 等都是透過 shared lib 實作


#### 最上層：使用者空間程式

| 分類                           | 說明                                                             |
| ---------------------------- | -------------------------------------------------------------- |
| `system-management programs` | 系統管理工具，如 `systemd`, `cron`, `login`, `sshd`, `network-manager` |
| `user-processes`             | 使用者自己執行的程式，如 `firefox`, `bash`, `python`                       |
| `user-utility programs`      | CLI 工具，如 `ls`, `cp`, `top`, `grep`, `find`，屬於 coreutils        |
| `compilers`                  | 開發工具如 `gcc`, `clang`, `make`, `ld`, `objdump` 等，負責程式編譯與連結      |

</br>

#### 關係圖

文字敘述：

Loadable Kernel Modules 提供了可以在系統執行期間動態載入與卸除的核心功能，例如裝置驅動程式。Linux Kernel 層負責統一管理這些核心模組、硬體資源以及系統呼叫介面。往上，System Shared Libraries（如 glibc）則作為使用者空間與核心空間的橋梁，封裝複雜的系統呼叫，提供開發者熟悉的 API（如 printf()、malloc() 等）。最上層的使用者程式（如系統管理工具、CLI 工具、應用軟體與編譯器）透過這些共享函式庫來與核心互動，實現應用程式功能。

</br>

![總結架構關係圖](images/linux總結架構關係圖.png)

</br>

Linux System 內由三個主要的程式碼主體組成：

</br>

* 核心 (Kernel)：
  * Linux Kernel 是 Linux 作業系統的核心部分，負責管理系統的所有硬體和軟件交互。它是一個大型的、複雜的程式，負責處理 CPU、記憶體管理、檔案系統、裝置控制、網路功能等。

  * Linux Kernel 是一個單一的、整體的二進制檔案 monolithic binary（為了最佳性能），在處理器的特權模式（又稱核心模式）下運行，實現了作為操作系統所需的所有功能。核心可以在運行時動態地加載（和卸載）模組。

  * Kernel 中不包含用戶代碼。任何不需要在核心模式下運行的操作系統支持代碼都放在系統庫 (system libraries) 中，並在用戶模式下運行。用戶模式只能訪問系統資源的受控子集 (controlled subset of system resources)。

  * Kernel 是用 C 語言和少量的匯編語言編寫的程式碼。

  * 這些程式碼被編譯成一個二進制映像（例如 vmlinux 或壓縮後的 bzImage），在系統啟動時由  Bootloader 加載到記憶體並執行。

</br>

* 核心模組（Kernel Module）：
  * Kernel Module 是 Linux 系統中的一種特殊程式，它們可以在系統運行時動態地加載或卸載到 Kernel 中。這些模組通常用於添加硬體驅動程式、檔案系統支援或其他擴展功能，而無需重新啟動系統或重新編譯  Kernel。

  * 驅動程式（Driver）通常是 Kernel Module 的一種特殊類型，主要負責硬體裝置的功能控制。它們是系統與硬體之間的接口，負責翻譯 Kernel 的通用指令為特定硬體裝置的控制信號，並將硬體的狀態信息回傳給 Kernel。驅動程式是 Kernel Module 的重要組成部分，但並非所有 Kernel Module 都是驅動程式。Kernel Module 還可能包括其他類型的擴展，例如檔案系統、網路協議支援等。

  * Kernel Module 可以被視為 Kernel 的子集。它們允許系統在運行時動態地擴展功能。這包括但不限於動態加載和卸載硬體驅動程式、檔案系統支援等。Kernel Module 提供了一種機制，讓系統能夠在不重啟或重新編譯 Kernel 的情況下添加或移除這些功能。

</br>

* 系統庫（System Library）：
  * System Library 通過定義一組標準函數，使應用程序能夠與核心進行交互。這些函數實現了大部分無需完整特權的核心代碼的作業系統功能。最重要的系統庫是 libc（C庫），它提供了標準 C 庫並實現了 Linux 系統調用接口的用戶模式部分，以及其他關鍵的系統級接口。

  * System libraries 允許應用程序向 Linux 核心發起系統呼叫（system call）。進行系統呼叫涉及從非特權的用戶（unprivileged user）模式轉移到特權的核心模式（privileged kernel mode）。這些庫還提供了一些不對應於系統呼叫的常用功能，如排序算法、數學函數和字串操作常用功能。

</br>

三者之間的關係：

```sql
┌────────────────────────────┐
│       User Applications    │ ← 用戶程式，透過系統庫使用核心服務
├────────────────────────────┤
│      System Libraries      │ ← libc, glibc：負責呼叫 system call 並包裝功能
├────────────────────────────┤
│        Linux Kernel        │ ← 管理 CPU / RAM / FS / Net / IPC 等核心功能
├────────────────────────────┤
│   Loadable Kernel Modules  │ ← 可動態加入的驅動、檔案系統、協定模組等
└────────────────────────────┘
```

</br>

# 深入學習



</br>