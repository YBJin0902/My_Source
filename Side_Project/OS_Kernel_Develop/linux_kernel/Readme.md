# Linux Kernel 

# 簡介 

在正式開始之前我們先來了解有關 Linux Kernel 的概念

</br>

## 工具

在開始之前先讓我們建構一個工具環境：User-Mode Linux（UML）。

User-Mode Linux (以下簡稱 UML) 顧名思義是將 Linux 核心移植到 user-space，如此一來，就可將這個修改的核心當作一般的 Linux process 來執行，有以下應用：
* 對與硬體架構無關的一般性 Linux 程式作偵錯與快速測試
* 檢驗 (客製化) 檔案系統的完整性與正確性，特別是 init scripts 相關的部份
* 在單機建構虛擬網路環境，以多個網路單元進行模擬操作
* 搭配 gdb 來追蹤 Linux 核心主體流程，快速測試新的演算法或引入改進
* 易於部署的 Linux 教學環境

</br>

UML 所使用的檔案系統對 Host Linux 來說也不過只是單純的檔案，經由適當配置，我們大可放心對虛擬機器作任何更動，而不必擔憂損害到真實的硬體與系統。

相當重要的觀念是：UML 本身就是全功能的核心，具備專屬的虛擬環境，對硬體的支援僅仰賴於宿主 Linux 系統。

![UNL 架構](../images/UNL_arch.png#pic_center=100x150)

</br>

### 建構 User-Mode Linux 和搭配的檔案系統

```note
若沒有特別說明，本文在開發端 (即編輯修改原始程式碼、編譯和準備相關工具等等動作) 的工作目錄皆位於上方解壓縮後的核心原始程式碼目錄。

為避免目錄切換導致的錯誤，可用環境變數保存: (此處 WS 指 "workspace"，命名沒有特別意思)

export WS=`pwd`
```

</br>

#### 基本設定：

<font color = red>皆為在 Ubuntu 中安裝</font>。

1. 安裝套件

```bash
$ sudo apt install build-essential libncurses-dev flex bison
$ sudo apt install xz-utils wget ca-certificates bc
```

</br>

2. 取得 Linux 核心原始程式碼（v5.12.0 為例）

```bash
$ wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.12.tar.xz
$ tar xvf linux-5.12.tar.xz
```

3.  Linux 核心原始程式碼

```bash
$ cd linux-5.12
```

</br>

4. 設定核心組態

```bash
$ make mrproper
$ make defconfig ARCH=um SUBARCH=x86_64
$ make linux ARCH=um SUBARCH=x86_64
```

編譯完成後會出現 linux 的執行檔。由於目前只是一班的執行檔所以我們需要準備一個 rootfs（root file system）。後續會說明 [rootfs](#root-file-system)。

</br>

5. 安裝 rootfs 套件

```bash
$ sudo apt install fakeroot
```

</br>

6. 套件管理系統 APK 建立 rootfs

```bash
$ export REPO=http://dl-cdn.alpinelinux.org/alpine/v3.13/main
$ mkdir -p rootfs
$ curl $REPO/x86_64/APKINDEX.tar.gz | tar -xz -C /tmp/
$ export APK_TOOL=`grep -A1 apk-tools-static /tmp/APKINDEX | cut -c3- | xargs printf "%s-%s.apk"`
$ curl $REPO/x86_64/$APK_TOOL | fakeroot tar -xz -C rootfs
$ fakeroot rootfs/sbin/apk.static \
    --repository $REPO --update-cache \
    --allow-untrusted \
    --root $PWD/rootfs --initdb add alpine-base
$ echo $REPO > rootfs/etc/apk/repositories
$ echo "LABEL=ALPINE_ROOT / auto defaults 1 1" >> rootfs/etc/fstab
```

逐行輸入即可。

</br>

7. 建立方便測試的 .sh 腳本，命名為 UML.sh

```bash
#!/bin/sh
./linux umid=uml0 \
        root=/dev/root rootfstype=hostfs hostfs=./rootfs \
        rw mem=64M init=/bin/sh quiet

stty sane ; echo
```

</br>

8. 啟動 UML

```bash
$ chmod +x UML.sh
$ ./UML.sh
```

<br>

稍早準備的檔案系統，已有 busybox，不過相關的 symbolic link 還未設定，我們需要執行以下: (只要做一次)

```bash
UML $ /bin/busybox --install
```

</br>


由於目前沒有特別去撰寫 init scripts，像是 procfs 沒預先掛載，需要手動執行以下命令：

```bash
UML $ mount -t proc none /proc
```

</br>

我們先前在編譯 Linux 核心程式碼時指定 ARCH=um，這對於 UML 環境的影響是什麼呢？執行下列命令：

```bash
UML $ cat /proc/cpuinfo
```

Output

```bash
processor	: 0
vendor_id	: User Mode Linux
model name	: UML
mode		: skas
host		: Linux node1 4.15.0-72-generic #81-Ubuntu SMP Tue Nov 26 12:20:02 UTC 2019 x86_64
bogomips	: 7722.59
```

</br>

目前工具的安裝就到一段落，接下來可以做一些客製化的安裝（可選）

tini：

```bash
$ wget -O rootfs/sbin/tini https://github.com/krallin/tini/releases/download/v0.19.0/tini-static
$ chmod +x rootfs/sbin/tini
```

建立 rootfs/init.sh 檔案，記得要變更檔案權限：

```sh
#!/bin/sh

mount -t proc proc /proc
mount -t sysfs sys /sys

#更改 UML 環境中命令提示訊息字樣與顏色
export PS1='UML:\w\ $ '
export PS1='\[\033[01;32mUML:\w\033[00m \$ '

exec /sbin/tini /bin/sh +m
```

```bash
$ chmod +x rootfs/init.sh
```

修改 UML.sh 中的 init 選項，改為指定 init.sh：

```sh
#!/bin/sh
./linux umid=uml0 \
        root=/dev/root rootfstype=hostfs hostfs=./rootfs \
        rw mem=64M init=/init.sh quiet
stty sane ; echo
```

---

</br>

#### 準備核心模組：

1. 編譯核心模組

```bash
$ make ARCH=um SUBARCH=x86_64 modules
```

</br>

2. 安裝核心模組到 rootfs 所在的目錄

```bash
$ make modules_install MODLIB=`pwd`/rootfs/lib/modules/VER ARCH=um
```

</br>

測試：

```bash
UML $ cd /lib/modules
UML $ ls # 這裡會發現有剛剛掛載的資料夾 VER
UML $ depmod -ae VER
UML $ modprobe isofs
UML $ lsmod
```

Output

```bash
Module                  Size  Used by    Tainted: G  
isofs                  25330  0 
```

</br>

### GDB 工具（GNU Degbugr）

除錯器（debugger），可以在一個精準受控的環境下執行另一個程式。例如：單步執行程式，跟蹤程式，查看變數內容，記憶體地址，以及程式中每一條指令指行完畢後CPU暫存器的變化情況，檢視程式呼叫堆疊等等。

GDB，全名為 gnu debugger，是在 GNU 軟體系統中的標準除錯器，介面為互動式的 shell，許多類 Unix，如 :FreeBSD，Linux 等作業系統中都能夠使用，支援許多語言，包括 C, C++ 等。

</br>

1. 建構 GDB script

```bash
$ echo "CONFIG_GDB_SCRIPTS=y" > .config-fragment
$ ARCH=um scripts/kconfig/merge_config.sh .config .config-fragment
$ make ARCH=um scripts_gdb
```

</br>

2. 用下行命令來啟動 GDB，指定載入 Linux 核心提供的 GDB script

```bash
$ gdb -ex "add-auto-load-safe-path scripts/gdb/vmlinux-gdb.py" \
      -ex "file vmlinux" \
      -ex "lx-version" -q
```

</br>

3. 準備 gdbinit 檔案（檔案名稱直接 gdbinit 即可）

```sh
python gdb.COMPLETE_EXPRESSION = gdb.COMPLETE_SYMBOL
add-auto-load-safe-path scripts/gdb/vmlinux-gdb.py
file vmlinux
lx-version
set args umid=uml0 root=/dev/root rootfstype=hostfs rootflags=FULLPATH/rootfs rw mem=64M init=/init.sh quiet
handle SIGSEGV nostop noprint
handle SIGUSR1 nopass stop print
```

</br>

執行：

```bash
$ sed -i 's|FULLPATH|'"$PWD"'|' gdbinit 
```

```bash
$ gdb -q -x gdbinit
```

Output

```bash
(gdb) run
```

</br>

一些 GDB CMD：

```bash
(gdb) lx-mounts 
(gdb) lx-cmdline
(gdb) lx-ps
(gdb) lx-dmesg
(gdb) lx-lsmod
```

</br>

以上就是可以幫助我們工具的安裝。

---

### 小結

UML（User Mode Linux）是學習 Linux Kernel 的最佳起點之一，因為它讓你不用燒機、不用模擬器、不用 root 權限，就能直接在 Linux 上執行與除錯你所編譯的 Kernel。

**核心啟動流程（Boot process）**

* 觀察 `start_kernel()`、`init/main.c` 的流程
* 分析系統初始化：記憶體子系統、排程器、裝置初始化
* ✅ 你可以對 `start_kernel` 下斷點，逐行 trace

</br>

**System Call 系統呼叫**

* UML 會透過 user-space syscall 模擬 Linux 真實系統呼叫
* 可觀察 `sys_read()`, `sys_write()`, `sys_execve()` 的進入與返回
* ✅ 在 GDB 下斷點於 `do_sys_open`, `sys_clone`, `sys_exit`

</br>

**行程管理（Process / Task Scheduling）**

* trace `schedule()`、`wake_up()`、`do_fork()` 等
* 分析 Task struct（`struct task_struct`）內容
* ✅ 可以學會 Linux 如何維護執行緒與上下文切換

</br>

**虛擬記憶體管理**

* 分析 kernel 如何建立 page table
* 熟悉 `kmalloc()`、`vmalloc()`、`brk()`、`mmap()` 的機制
* ✅ 查看 slab/slub allocator 的行為，甚至觀察 page fault 處理流程

</br>

**核心模組（Kernel Modules）**

* 在 UML 中可以載入 `.ko` 模組，如：

  ```bash
  insmod hello.ko
  ```
* 可用 GDB trace `init_module()`、模組初始化流程
* ✅ 模組開發學習、模擬 driver 開發流程無風險又快速

</br>

**檔案系統 VFS 與 syscall interaction**

* UML 支援擬態的磁碟 image（ubd），你可以在其中執行 `open()`, `read()`, `write()` 並 trace kernel 的 VFS 層
* ✅ 學會 `do_filp_open()`、`vfs_read()` 的內部機制

</br>

**Signal / IPC / clone / exec 系統行為**

* UML 可以真實模擬行程之間的 IPC、signal 傳遞、execve 換程式
* ✅ 適合深入學 `clone()` vs `fork()`，以及 user-space interaction 機制

</br>

那工具備齊了，讓我們開始吧 ～

</br>

---

</br>

## OS Kernel duty and subsystem

「Linux Kernel 的所有功能」，實際上是在說明一個操作系統核心（Kernel）所承擔的所有責任與子系統。Linux Kernel 是一個模組化、功能完整的單體核心，提供了從資源管理到系統呼叫的全部功能。

</br>

### 核心基礎設施（Core Infrastructure）

| 功能                       | 說明                            |
| ------------------------ | ----------------------------- |
| 初始化子系統（boot/init）        | 啟動流程，執行 `start_kernel()` 等    |
| 記錄與除錯（printk, tracing）   | `printk`, ftrace, debugfs 等   |
| 錯誤處理（panic, oops）        | 當系統發生不可恢復錯誤時提供報告與保護機制         |
| 核心模組支援（Loadable Modules） | `insmod`, `rmmod`, `modprobe` |


</br>

### 程序與排程子系統（Process / Scheduler）

| 功能                   | 說明                                  |
| -------------------- | ----------------------------------- |
| 行程控制（task\_struct）   | 行程資訊資料結構                            |
| 行程排程器（CFS）           | 完整排程策略與切換實作                         |
| 行程建立與管理（fork/clone）  | 系統呼叫如 `fork()`, `clone()`           |
| 行程優先權與時間片            | 動態調整行程 priority、nice、real-time      |
| 工作佇列與 kernel threads | 工作推遲與背景執行支援（`workqueue`, `kthread`） |

</br>

### 記憶體管理（Memory Management, MM）

| 功能                            | 說明                       |
| ----------------------------- | ------------------------ |
| 虛擬記憶體（VMM）                    | 每個 process 擁有獨立虛擬空間      |
| 實體頁管理（Page Allocator）         | 頁面分配與釋放（Buddy system）    |
| 記憶體區塊配置（SLAB/SLUB）            | 針對 kernel object 的快取分配策略 |
| mmap/munmap 支援                | 使用者空間記憶體映射               |
| Demand Paging / Copy-on-Write | 延遲載入與複製優化                |
| Swap（交換空間）支援                  | 不足時移至磁碟                  |

</br>

### 檔案系統（VFS + FS）

| 功能               | 說明                                            |
| ---------------- | --------------------------------------------- |
| 虛擬檔案系統（VFS）      | 抽象層，支援 ext4, FAT, tmpfs, etc.                 |
| 磁碟快取（page cache） | 檔案存取加速                                        |
| 路徑解析與權限管理        | `open()`, `chmod()`, `chown()`                |
| 檔案系統類型支援         | ext2/3/4, XFS, Btrfs, squashfs, ISO9660, etc. |
| 特殊檔案系統           | procfs、sysfs、debugfs、tmpfs、devtmpfs           |

</br>

### 網路子系統（Networking Stack）

| 功能                              | 說明                                           |
| ------------------------------- | -------------------------------------------- |
| 協定支援                            | IPv4, IPv6, TCP, UDP, SCTP, ICMP, RAW socket |
| 網卡驅動支援                          | eth, wlan, loopback, tap/tun 等               |
| Netfilter / iptables / nftables | 封包過濾、防火牆功能                                   |
| socket 介面與 syscalls             | `socket()`, `bind()`, `send()`, `recv()`     |
| 網路 namespace / 虛擬網路設備           | 用於容器、虛擬化環境                                   |

</br>

### 裝置與驅動支援（Device & Drivers）

| 類別                     | 說明                                |
| ---------------------- | --------------------------------- |
| 字元裝置（Character Device） | /dev/tty, /dev/random, 自定義驅動      |
| 區塊裝置（Block Device）     | 磁碟裝置：SATA, NVMe, USB mass storage |
| 網路裝置（Network Device）   | eth0, wlan0 等介面驅動                 |
| I2C / SPI / UART 子系統   | 嵌入式外設驅動介面                         |
| GPIO / PWM / ADC       | 嵌入式輸出入控制                          |
| ALSA / Input / DRM     | 聲音、鍵盤滑鼠、圖形輸出支援                    |
| PCI / USB / ACPI       | 外部匯流排與熱插拔支援                       |

</br>

### 安全性與存取控制（Security & Permissions）

| 功能                         | 說明              |
| -------------------------- | --------------- |
| 權限控制（UID/GID）              | 使用者與群組識別與檢查     |
| 能力（Capability）             | 精細權限劃分          |
| SELinux / AppArmor / Smack | 安全模組與 MAC 框架支援  |
| 安全系統呼叫限制（seccomp）          | 控制哪些 syscall 可用 |

</br>

### 系統呼叫介面（Syscall Interface）

| 功能                     | 說明                                        |
| ---------------------- | ----------------------------------------- |
| 提供給使用者空間的 API          | `read()`, `write()`, `mmap()`, `fork()` 等 |
| 系統呼叫表                  | 每個架構對應自己的 syscall table                   |
| ptrace, audit, tracing | 系統呼叫監控與除錯支援                               |

</br>

### 時間與時鐘（Timers & Clocks）

| 功能                     | 說明            |
| ---------------------- | ------------- |
| 時間中斷（tick）與排程          | 排程器依據 tick 運作 |
| High-resolution timers | 精密時間控制        |
| RTC / TSC / HPET 支援    | 不同硬體時間來源      |
| NTP 同步                 | 網路時間同步機制      |

</br>

### 中斷處理（Interrupts）與同步機制

| 功能                         | 說明                            |
| -------------------------- | ----------------------------- |
| 中斷控制器支援                    | APIC、GIC、etc.                 |
| 中斷服務例程（ISR）                | `request_irq()`, `free_irq()` |
| 工作排程延遲（bottom halves）      | tasklet, workqueue            |
| Spinlocks / Mutex / RWLock | 核心同步                          |
| 原子操作與記憶體障壁                 | `atomic_t`, `smp_mb()` 等      |

</br>

### 容器與命名空間（Namespaces / Cgroups）

| 功能                                      | 說明                             |
| --------------------------------------- | ------------------------------ |
| Cgroups（資源隔離）                           | 限制 CPU、記憶體、IO 使用量              |
| Namespace（命名空間）                         | PID, mount, net, user, IPC 等隔離 |

</br>

### 虛擬化支援（KVM / UML / Xen / VirtIO）

| 功能                   | 說明            |
| -------------------- | ------------- |
| KVM 虛擬機支援            | Linux 原生虛擬化平台 |
| VirtIO 裝置            | 用於高效虛擬化 I/O   |
| UML（User Mode Linux） | 核心跑在使用者空間     |
| Xen PV/HVM guest     | 支援 Xen 架構虛擬化  |

</br>

以上的大項皆為一個作業系統的 Kernel 會有的部分，表格內的功能皆為大概舉例。

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

---

</br>

# Bootloader






# root file system