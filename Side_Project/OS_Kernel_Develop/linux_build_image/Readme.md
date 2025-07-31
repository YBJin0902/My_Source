# Linux DEV

# Linux Kernel 開發



# systemd

一套現代 Linux 系統中主要用來啟動系統與管理服務（services）的 init 系統。

systemd 是目前大多數主流 Linux 發行版所採用的初始化系統（init system），它的主要功能包括：
* 系統啟動與初始化（取代 SysVinit）
* 管理 system service（daemon）
* 管理系統資源（如 cgroups）
* 日誌紀錄（journald）
* 定時執行（systemd-timers，取代 cron 的某些功能）

</br>

### Systemd 基本架構與單位（Unit）

systemd 使用「unit」來描述服務或資源的各種元素。

| 類型      | 副檔名        | 說明                      |
| :------- | :---------- | :----------------------- |
| service | `.service` | 定義一個服務                  |
| socket  | `.socket`  | 定義一個 socket             |
| target  | `.target`  | 一組 unit 的集合（如 runlevel） |
| device  | `.device`  | 對應一個裝置                  |
| mount   | `.mount`   | 定義一個掛載點                 |
| timer   | `.timer`   | 定時器，可取代 `cron`          |
| path    | `.path`    | 監控某個路徑                  |


</br>

### CMD

查詢 systemd 操作日誌：

```bash
journalctl -u can0-setup.service
```

---

</br>

## service 系統服務控制

.service 檔是 systemd 單元檔（unit file）的一種，用於描述一個「服務」（service），也就是長時間執行的程式，如：web server、資料庫、背景監聽程式等。

service 的結構會需要：unit、service、install

```ini
[Unit]
Description=My Example Service
After=network.target

[Service]
Type=simple
ExecStart=/usr/bin/my_program
Restart=on-failure
User=nobody

[Install]
WantedBy=multi-user.target
```

* [Unit]：定義服務的基本資訊與依賴關係

| 指令                      | 說明                           |
| :----------------------- | :---------------------------- |
| `Description=`          | 為此服務提供一段簡短的描述                |
| `Documentation=`        | 指向說明文件的 URL 或檔案              |
| `Requires=`             | 指定「需要的服務」。若這個服務失敗，當前服務也會失敗   |
| `Wants=`                | 類似 `Requires`，但弱依賴，失敗不會影響本服務 |
| `Before=`               | 指定本單元要比誰「早」啟動                |
| `After=`                | 指定本單元要比誰「晚」啟動                |
| `Conflicts=`            | 與哪些單元衝突，不能同時執行               |
| `ConditionPathExists=`  | 僅當指定路徑存在時，才啟動該服務             |
| `ConditionPathExists=!` | 僅當指定路徑不存在時，才啟動該服務            |

</br>

* [Service]：定義服務本體

| 指令                                   | 說明                        |
| :------------------------------------ | :------------------- |
| `Type=`                              | 定義啟動型態，常見值如下：<br> - `simple`：預設，執行 `ExecStart`，不 fork<br> - `forking`：若服務會 background fork，需要此選項<br> - `oneshot`：只執行一次的腳本<br> - `notify`：程序會主動告知 systemd 啟動完成<br> - `idle`：延後執行直到系統空閒 |
| `ExecStart=`                         | 啟動服務的主命令路徑                                                                                                                                                                            |
| `ExecStartPre=`                      | 在主命令執行前，先執行的命令                                                                                                                                                                        |
| `ExecStartPost=`                     | 主命令成功後執行的命令                                                                                                                                                                           |
| `ExecStop=`                          | 停止服務時執行的命令                                                                                                                                                                            |
| `ExecReload=`                        | 重載服務時執行的命令（如 `nginx -s reload`）                                                                                                                                                       |
| `Restart=`                           | 程序失敗時是否重啟：<br> - `no`（預設）<br> - `on-success`<br> - `on-failure`<br> - `always`                                                                                                        |
| `RestartSec=`                        | 重啟間隔秒數                                                                                                                                                                                |
| `User=`                              | 指定執行該服務的使用者                                                                                                                                                                           |
| `Group=`                             | 指定群組                                                                                                                                                                                  |
| `WorkingDirectory=`                  | 設定工作目錄                                                                                                                                                                                |
| `Environment=`                       | 設定環境變數（格式：`VAR=value`）                                                                                                                                                                |
| `EnvironmentFile=`                   | 指定包含環境變數設定的檔案                                                                                                                                                                         |
| `StandardOutput=` / `StandardError=` | 控制輸出：`journal`, `syslog`, `null`, `tty`, `file:/path`                                                                                                                                 |
| `TimeoutStartSec=`                   | 執行 `ExecStart` 的逾時秒數                                                                                                                                                                  |
| `TimeoutStopSec=`                    | 停止服務時的逾時時間                                                                                                                                                                            |
| `KillSignal=`                        | 停止服務時使用的 signal（預設 SIGTERM）                                                                                                                                                           |


</br>

* [Install]：定義開機自動啟動的連接方式

| 指令                 | 說明  |
| :------------------ | :---------------------- |
| `WantedBy=`        | 指定此服務所附加的 target，例如：`multi-user.target`（CLI 模式開機後） |
| `RequiredBy=`      | 被哪個 target 所需，和 `WantedBy` 類似，但為強依賴                |
| `Alias=`           | 建立此服務的別名（如提供多個服務名）                                 |
| `Also=`            | 啟用/禁用此單元時，同時啟用/禁用的其他單元                             |
| `DefaultInstance=` | 用於 template unit，指定預設的 instance 名稱                 |


</br>

</br>

# Socket

Socket 是一套用於不同主機之間的通訊 API，工作在<font color = red>傳輸層</font>上。它是一種點對點的連線。

在 Linux 開發中可以利用 Socket 對 Device Controller 連線，並進一步做到對 Peripheral I/O 的控制。

需要注意：不是用 Socket 直接對周邊進行控制，而是透過 Socket 作為 IPC（進程間通訊） 的工具，間接控制周邊。

進程間通訊（Inter-Process Communication，簡稱IPC）是一種技術或方法，用於在至少兩個行程或執行緒之間傳輸資料或訊號。每個行程都有自己獨立的系統資源，彼此隔離。為了實現不同行程之間的資源訪問和協調工作，需要使用進程間通訊。

</br>

## 架構

![alt text](image.png)

</br>

```pgsql
+---------------------+
|   User Application  | <--- 控制邏輯（透過 socket 傳指令）
+---------------------+
          |
          |  (socket IPC)
          v
+---------------------+
|  Device Controller  | <--- 獨立程式（收指令後操作 I2C/SPI/UART 等周邊）
+---------------------+
          |
          |  (libgpiod / ioctl / write to /dev)
          v
+---------------------+
|    Peripheral I/O   |
+---------------------+
```

</br>

## SocketCAN

SocketCAN 子系統是在 Linux 下 CAN 協定（Controller Area Network）實作的實作方法。 

Linux 下最早使用 CAN 的方法是基於字元設備來實現的，與之不同的是 SocketCAN 使用伯克利的 Socket 介面和 Linux 網路協定棧，這種方法使得 CAN 設備驅動可以透過網路介面來呼叫。 

SocketCAN 的介面被設計的盡量接近 TCP/IP 的協議，讓那些熟悉網路程式設計的程式設計師能夠比較容易的學習和使用。

SocketCAN 把 CAN 裝置看作「網路介面」，並使用 Socket API（如 socket(), bind(), recv(), send()） 進行通訊。

</br>

### SocketCAN 架構組成
1. Kernel CAN Stack
* 內建於 Linux kernel，例如：
  * can 模組（協定族 PF_CAN（Protocol Family））
  * can_raw、can_bcm、can_isotp 等通訊協定模組

2. CAN 驅動程式
* 針對實體 CAN 控制器

3. Socket API
使用標準 POSIX socket 程式設計風格操作 CAN：

```c
int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
```

</br>

```pgsql
+--------------------------+
|   User Application       |
|   (C / Python / etc.)    |
+-----------+--------------+
            |
        Socket API
            |
+-----------v--------------+
|      PF_CAN Stack        | <--- CAN_RAW, CAN_BCM, etc.
+-----------+--------------+
            |
        CAN Driver
            |
+-----------v--------------+
|     CAN Hardware Device  | <--- MCP2515, USB-CAN, etc.
+--------------------------+
```

</br>

### Queue 佇列接收

![alt text](image-1.png)

</br>

允許多個應用程式同時存取網路導致了新的問題出現，那就是不同的應用程式可能會在同一個 CAN 網路介面上對具有相同 CAN-ID 的幀感興趣。

SocketCAN 的核心部分實現了SocketCAN 的協定族，透過高效率的接收佇列解決了這個問題。

例如一個用戶空間的程式開啟了一個原始 CAN 套接字，原始協定模組將向CAN套接字的核心模組申請用戶空間所需的一系列 CAN-ID。

SocketCAN 的核心提供 CAN 協定模組預約與解約 CAN-ID 的介面 --can_rx_(un)register()，無論這個 CAN-ID 是針對一個特定的 CAN 介面或是所有已知的CAN介面。

為了最佳化 CPU 的運作效率，每個裝置都對應一個接收佇列，這樣比較容易實作各種封包過濾規則。

</br>

### How to use

就像 TCP/IP 協定一樣，在使用 CAN 網路之前你首先需要打開一個套接字。

CAN 的套接字使用到了一個新的協定族，所以在呼叫 socket(2) 這個系統函數的時候需要將 PF_CAN 當作第一個參數。

目前有兩個 CAN 的協定可以選擇：
* 一個是原始套接字協定（ raw socket protocol）
* 另一個是廣播管理協定 BCM（broadcast manager

```c
s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
```

```c
s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);
```

</br>

在成功創建套接字之後，通常需要使用 bind(2) 函數將套接字綁定在某個 CAN 介面上。

在綁定（CAN_RAW）或連接（CAN_BCM）套接字之後，你可以在套接字上使用 read(2)/write(2)，也可以使用 send(2)/sendto(2)/sendmsg(2) 和對應的 recv* 操作。

當然也會有 CAN 特有的套接字選項，以下會說明。

基本的CAN幀結構體和套接字位址結構體定義在include/linux/can.h：

```h
struct can_frame {
            canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
            __u8    can_dlc; /* frame payload length in byte (0 .. 8) */
            __u8    __pad;   /* padding */
            __u8    __res0;  /* reserved / padding */
            __u8    __res1;  /* reserved / padding */
            __u8    data[8] __attribute__((aligned(8)));
    };
```

結構體的有效資料在 data[] 陣列中，它的位元組對齊是 64bit 的，所以使用者可以比較方便的在 data[] 中傳輸自己定義的結構體和共用體。

CAN 總線中沒有預設的位元組序。在 CAN_RAW 套接字上呼叫 read(2)，傳回給使用者空間的資料是一個 struct can_frame 結構體。

</br>

像 PF_PACKET 套接字一樣，sockaddr_can 結構體也有介面的索引，這個索引綁定了特定介面：

```h
struct sockaddr_can { 
      sa_family_t can_family;
      int          can_ifindex;
      union { 
                /* transport protocol class address info (eg ISOTP) */
                struct { canid_t rx_id, tx_id; } tp;
                /* reserved for future CAN protocols address information */ 
      } can_addr; 
};
```

指定介面索引需要呼叫 ioctl()（例如對於沒有錯誤檢查 CAN_RAW 套接字）:

```c
int s;
struct sockaddr_can addr;
struct ifreq ifr;

s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

strcpy(ifr.ifr_name, "can0" );
ioctl (s, SIOCGIFINDEX, &ifr);

addr.can_family = AF_CAN;
addr.can_ifindex = ifr.ifr_ifindex;

bind(s, (struct sockaddr *)&addr, sizeof(addr));
```

</br>

為了將套接字和所有的 CAN 介面綁定，介面索引必須是 0。

這樣套接字便可以從所有使能的 CAN 介面接收 CAN 幀。

recvfrom(2) 可以指定從哪個介面接收。在一個已經和所有 CAN 介面綁定的套接字上，sendto(2) 可以指定從哪個介面發送。

從一個 CAN_RAW 套接字上讀取 CAN 幀也就是讀取 struct can_frame 結構體：

```c
struct can_frame frame;

nbytes = read(s, &frame, sizeof(struct can_frame));

if (nbytes < 0) {
        perror("can raw socket read");
        return 1;
}

/* paranoid check ... */
if (nbytes < sizeof(struct can_frame)) {
        fprintf(stderr, "read: incomplete CAN frame\n");
        return 1;
}
```

寫 CAN 幀也是類似的，需要用到 write(2) 函數：

```c
nbytes = write(s, &frame, sizeof(struct can_frame)); 
```

#### 參考資料：

* [CSDN Linux 核心 Socket CAN 中文文檔](https://blog.csdn.net/yuanlulu/article/details/7220060)

</br>

* [SocketCAN - Controller Area Network](https://docs.kernel.org/networking/can.html)

</br>

# UNIX Domain Socket

UNIX Domain Socket（UDS）是一種在同一台機器上的程序之間通訊的 Socket 技術。

它與一般常見的 TCP/IP socket 不同，不走網路協定，而是透過檔案系統的 socket 檔（通常在 /tmp 或 /run）進行資料交換。

特性：
* 運作只在本地機器，不經網路。
* 使用 檔案路徑 做為通訊端點（例如 /tmp/mysock）。



</br>