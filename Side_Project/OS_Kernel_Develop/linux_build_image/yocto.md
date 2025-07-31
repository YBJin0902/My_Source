# Yocto Learn

### 甚麼是 Yocto Project

Yocto Project 是一套用來「建立自訂的 Linux 發行版」的工具平台，特別針對嵌入式裝置（如開發板、車機、網通設備、工業電腦）設計。

</br>

### Yocto Project 在做甚麼

Yocto 的核心任務是幫你：

1. 從原始碼建構一個完整的 Linux 系統（kernel、rootfs、toolchain）

2. 自動處理交叉編譯與相依性（針對 ARM、RISC-V、x86）

3. 根據自定義的配置，產生最小化或客製化映像檔

</br>

### Yocto 組成的工具

| 元件                  | 功能 |
| ------------------- | ------- |
| **Poky**            |  Yocto 預設的 reference distribution（整合 BitBake + 基礎 recipes） |
| **BitBake**         |  編譯工具核心（像是 make）負責執行建構流程                                   |
| **Layers**          |  模組化的功能集合，如 `meta-oe`, `meta-qt5`, `meta-st`               |
| **Recipes (.bb)**   |  指定怎麼下載、編譯、安裝某個套件                                          |
| **Images**          |  組合多個套件/功能產生完整 rootfs 映像                                   |
| **SDK / Toolchain** |  為目標平台自動產生交叉編譯工具鏈                                          |

</br>

用更白話文的方式說明：

| Yocto 元件                          | 比喻說明                                               |
| --------------------------------- | -------------------------------------------------- |
| **BitBake**                       | 🍳 廚房裡的「總主廚自動機器人」：接到指令後會自動照食譜下指令、調度廚師、組裝整份套餐       |
| **Recipes (.bb 檔案)**              | 📜 一道道菜的「食譜」：說明要去哪裡拿食材、怎麼煮、要放哪裡、要附上什麼醬料（安裝哪個路徑）    |
| **Layers**                        | 🍱 不同的菜單區域：中餐、日式、甜點，各自收錄自己的食譜集合（Layer = 模組化菜單）     |
| **Images**                        | 🍽 一整份套餐：你最後要上桌的完整 Linux rootfs，是由多道「recipe」料理組合而成 |
| **SRC\_URI**                      | 🛒 食材來源：告訴你去哪裡買原料（Git、http、local 檔案）               |
| **do\_compile() / do\_install()** | 🔪 烹飪流程：像是切菜、炒鍋、擺盤，對應程式的編譯與安裝過程                    |
| **sstate-cache**                  | 🧊 冷凍庫：之前做好的料理可以直接拿出來加熱，不需要每次重煮（加速建構）              |


</br>

Yocto 系統（透過 BitBake）會自動完成一連串的流程，從下載原始碼到產生一個完整的 rootfs 映像（例如 .ext4, .wic, .tar, .sdimg 等）

</br>

#### Step 1. 分析與解析相依性
   * 根據你指定的 image（例如 core-image-minimal），BitBake 會去讀取這個配方（.bb）中需要安裝哪些套件（IMAGE_INSTALL）。

   * 再去追蹤每個套件的配方及其相依性。

#### Step 2. 下載原始碼（do_fetch）
   * 對每個 .bb 所指定的 SRC_URI，進行原始碼下載。

   * 支援 git://, http://, ftp://, file:// 等協定。

   * 檔案會放到 downloads/ 資料夾。

#### Step 3. 解壓與套用 patch（do_unpack, do_patch）
   * 把原始碼解壓縮到 tmp/work/。

   * 套用 .bb 中指定的 patch 檔案或附加修正。

#### Step 4. 建構與編譯（do_configure, do_compile）
   * do_configure: 執行 ./configure 或 CMake 等建構設定。

   * do_compile: 執行 make 或其他工具編譯原始碼。

#### Step 5. 安裝檔案（do_install）
   * 將編譯出來的執行檔、庫、腳本安裝到 image/ 模擬 rootfs 的目錄結構中（如 /usr/bin, /lib 等）。

#### Step 6.  打包與映像建構（do_rootfs, do_image）
   * 將上面安裝的內容打包成 root filesystem。

   * 預設格式包括：
     * core-image-xxx.ext4 → rootfs image（給板子使用）
     * core-image-xxx.tar.gz → 原始根目錄 tarball
     * core-image-xxx.manifest → 映像中包含哪些套件


</br>

---

</br>

# How to use 

## Compile（Bitbake）

</br>

## 新增自己的 Project

當我想要在 Yocto 專案中新增屬於自己的 package 時，我該怎麼做：

步驟總覽
1. 建立 Layer（元資料層）
2. 撰寫 Recipe（配方）給你的專案
3. 將你的專案原始碼加進來（或透過 Git）
4. 修改 bblayers.conf 加入你的 Layer
5. 在映像中安裝你的 package
6. bitbake your-image 自動整合到映像中

</br>

其中第 3 步，我們可以先將設計好的專案使用 git submodule 的方式匯入。

</br>

---

</br>

# Documentation research

Yocto 專案是一個專注於嵌入式 Linux 開發人員的開源協作專案。此外，Yocto 專案使用基於 Poky 專案的 OpenEmbedded 建置系統來建立完整的 Linux image。

</br>

Force on Rocko version（2.4）

</br>

## System Requirements

```
sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat

sudo apt-get install libsdl1.2-dev xterm

sudo apt-get install autoconf automake libtool libglib2.0-dev libarchive-dev

sudo apt-get install python-git
```

</br>

## Required Git, tar, and Python Versions

* Git 1.7.8 or greater

* tar 1.24 or greater

* Python 2.7.3 or greater not including Python 3.x, which is not supported.

</br>

# 甚麼是 Yocto

## 簡介

Yocto Project是一個開源項目，旨在幫助開發者創建嵌入式Linux系統。</br>
它提供了一個框架和工具集，可以使開發者輕鬆地建立客製化的Linux發行版，以滿足各種嵌入式設備的需求。

以下是一些關於Yocto Project的基本知識：

#### 1. 核心元件：
* OpenEmbedded Core (OE-Core)：Yocto Project 的核心元件之一，提供了一組工具和類別，用於建構和管理嵌入式Linux系統的軟體包。
* BitBake：一個用 Python 編寫的建置工具，用於定義和執行建置任務。
* Metadata：元資料包括用於建構 Linux 發行版的各種設定檔、腳本和類別。


#### 2. 主要概念：
* Layer：Yocto Project 使用層的概念來組織和擴展功能。每個層都包含一組相關的配置、軟體包和元資料。
* Recipe：一個 Recipe 是一個用於構建軟體包的腳本，其中包含了構建該軟體包所需的所有信息，如下載源代碼、配置選項、編譯參數等。
* Image：一個 Image 是一個嵌入式 Linux 系統的鏡像，包含了核心、檔案系統和所需的應用程式等元件。


#### 3. 工作流程：
* 配置：透過編輯設定檔和層來配置 Yocto Project 以滿足特定的需求。
* 建置：使用 BitBake 工具執行建置任務，BitBake 將根據配置和 Recipe 建置所需的軟體包和鏡像。
* 調試和自訂：透過調試工具和自訂 Recipe 來調試和自訂產生的 Linux 系統。

</br>

## Poky

在 Yocto 專案中，Poky 是一個包含 OpenEmbedded-Core (OE-Core) 層以及其他關鍵層和元件的參考發行版。

它提供了一個完整的軟體包集合，用於建立嵌入式 Linux 系統。</br>
Poky 提供了一個基礎框架，使開發者可以快速開始建立客製化的 Linux 發行版。
具體來說，Poky 包括以下主要元件：

* OpenEmbedded-Core (OE-Core)：這是 Yocto 專案的核心層，提供了建置和管理嵌入式 Linux 系統所需的基礎工具和類別。

* BitBake：是一個用 Python 編寫的建置工具，用於定義和執行建置任務。 
  * BitBake 是建構和打包軟體包的核心引擎。

* Meta層：Poky 包含了一系列的 meta 層，其中包括了用於建構嵌入式系統的一些常用設定、軟體包和工具。
  * 範例設定檔和軟體包：Poky 包含了一些範例設定檔和預先設定的軟體包，使開發者能夠快速開始建立自己的嵌入式系統，並作為參考。
  * 總的來說，Poky 提供了一個完整的開發環境，使開發者能夠基於 Yocto 專案快速建置、自訂和部署嵌入式 Linux 系統。

</br>

## bb

在Yocto專案中，BB 腳本是 BitBake 的一種設定檔格式。 

BitBake 是 Yocto 專案中用於建置和管理軟體包的工具，而 BB 腳本是 BitBake 使用的主要設定檔之一。 BB 腳本通常用於定義軟體包的建置過程，包括下載原始程式碼、設定編譯選項、指定依賴關係等。

BB 腳本通常包含以下部分：

1. 元資料資訊：指定軟體包的名稱、版本、許可證等元資料資訊。

2. 依賴關係：指定軟體包所依賴的其他軟體包，以確保建置時能正確解析依賴關係。
3. 原始碼取得：指定如何取得軟體包的原始碼，可以是從網路下載、從本機檔案系統取得或使用版本控制系統（如Git、SVN等）。
4. 編譯配置：指定編譯時的設定選項，如編譯器、最佳化選項等。
5. 建置步驟：定義軟體包的建置過程，包括編譯、安裝等步驟。
6. 安裝路徑：指定軟體包安裝到目標系統的路徑。
7. 補丁：包含對軟體包進行補丁修復或修改的資訊。</br>
 BB 腳本的檔案副檔名通常是 .bb，例如 example.bb。 BB 腳本是 Yocto 專案中最基本的建置單元之一，透過編寫 BB 腳本，開發者可以客製化地建立軟體包，以滿足特定嵌入式系統的需求

</br>

### BB 語法

BB 檔案是 BitBake 工具用來建立軟體包的設定文件，通常以.bb為副檔名。以下是 BB 檔的基本語法和常見元素：

```
# metadate 訊息：指定軟體包的名稱、版本、 licence 等
SUMMARY = "Short description of the package"
LICENSE = "GPLv2"
SRC_URI = "http://example.com/archive.tar.gz"

# 依賴關西：指定個軟體之間的相依關西，確保建置時能正確解析依賴關西
DEPENDS = "libfoo"

# Source code 獲取：指定如何獲取軟體包
SRC_URI = "http://example.com/archive.tar.gz"

# 編譯配置：指定編譯時的配置選項
EXTRA_OEMAKE = "-j4"

# 建構步驟：定義軟體包的建置過程，包括編譯、安裝步驟等
do_compile() {
    make
}

# 安裝路徑：指定軟體包安裝到指定路徑
DESTDIR = "${D}"

# 補丁 patch：包含軟體包進行補丁修復或是修改
SRC_URI_append = " file://example.patch"

# 變數
MY_VAR = "value"

# 條件式
ifeq (${MACHINE}, "raspberrypi")
    # do something specific for Raspberry Pi
endif
```

## Build

### 概述

在開發環境中，每當變更硬體、新增或變更系統程式庫或新增或變更具有相依性的服務時，都需要建置映像。

![yocto_build_overview](yocto_build_overview.png)

</br>

在使用 Yocot project 時需要先設置 OpenEmbedded 的環境

</br>

### Yocto 項目的廚師 bitbake

bitbake 是 OpenEmbedded 建置系統的引擎，透過解析一系列設定檔（主要為 recipes，即 bb/bbappend 檔案）來建立任務列表，並根據依賴關係依序執行。

透過 bitbake -c listtasks xxx（模組名或映像名）指令可以查看編譯一個模組或整個映像所需任務名稱。

編譯一個模組/映像的主要執行過程如下（核心除外）：

![alt text](image.png)

</br>

下面按四個階段來講解 bitbake 的執行過程及涉及的變量，其中第四階段（do_rootfs、do_image）不是編譯每個模組都有的。

### 一、原始碼獲取及處理

這個階段包含三個任務，分別為 do_fetch、do_unpack、do_patch。

原文件來源

Yocto 編譯一個模組所需的原始碼或開源元件從哪裡取得？

![alt text](image-1.png)

如上圖所示，原始碼可以從上游開源專案（如busybox-1.28.3.tar.bz2）、本機專案（本機原始碼）、軟體組態管理（如git）中取得。

</br>

---

#### do_fetch：根據設定檔中 SRC_URI 變數所指定的方式取得原始碼。

---

#### do_unpack：如果來源檔案需要解碼，則該任務會將來源碼包解壓縮到指定目錄下。

![alt text](image-2.png)

---

#### do_patch：如果對開源專案進行了修改，則這個任務可以為解壓縮後的原始碼打補丁，例如對 linux 開源核心原始碼進行了修改。

![alt text](image-3.png)

---

</br>

這個階段涉及的配置變數有以下：

#### SRC_URI

來源檔案清單變量，位於配方（recipe）檔案中。每個 recipe 必須有一個指向來源的 SRC_URI 變數。

```
SRC_URI = "git://github.com/openbmc/linux;protocol=git;branch=${KBRANCH}" 
# 表示從 github 上下載

SRC_URI += "file://defconfig" 
# 表示從本地目錄獲取內核默認配置，路徑由 FILESPATH 指定
```

列舉常用的取得方式：

- file:// ：從本地機器獲取文件，通常是元數據附帶的文件（例如補丁、內核配置、uboot配置文件），路徑是相對於 FILESPATH 變量的。</br>
建置系統預設在配方同目錄下名為 ``` “${BP}” 、 "${BPN}" 、 "files" ``` 的目錄下尋找指定文件，如需增加額外路徑，便通過 FILESPATH 並 FILESEXTRAPATHS 指定。

- git:// ：從 Git 版本控制儲存庫中取得檔案。</br>
如果是從本地倉庫下載，則先設定本地 Git倉庫。 Yocto 依賴與 git 的版本控制，簡單來說就是 Yocto 每次編譯時候，會去檢查軟體包的源文件是否修改，只有修改過才會讓軟體包重新編譯，那麼對於 git 倉庫就是對比上一次與當次版本差異。

- http:// ：使用http。
- https:// ：使用https。

---

</br>

#### DL_DIR

用於指定開源元件包（tar、git等）下載的存放路徑，位於編譯目標層的 conf/local.conf.sample 檔案中（如有需要可在此檔案中修改），這個檔案將被解析到 build/conf/local.conf 檔案中，預設情況下該變數指定的目錄為 build/downloads/，如果多使用者在一台編譯機器上使用，可以指定一個公用目錄，避免重複下載，例如 DL_DIR ?= "/opt/downloads"。

通常一個元件包下載完畢後，在存放目錄下會產生一個包名加".done"的檔案表示該包下載完成。

小技巧：某個開源元件一直下載失敗，那就從其他地方下載同版本元件拷貝到存放目錄下，並複製一個*.done檔案改為對應名字即可。

---

</br>

#### FILESPATH

建置系統搜尋本機檔案（本機原始碼、修補程式、設定檔等）的目錄集合，位於配方（recipe）檔案中。

在建置過程中，bitbake 尋找 SRC_URI 變數的 ```file://``` 語句指定的本機檔案時，會依序搜尋 FILESPATH 變數指定的目錄集合，該變數的預設值在 meta/classes/base.bbclass 中定義：

```
FILESPATH = "${@base_set_filespath(["${FILE_DIRNAME}/${BP}", \
            "${FILE_DIRNAME}/${BPN}", "${FILE_DIRNAME}/files"], d)}"
```

預設情況，建置系統會將 ```file://``` 指定帶有 ".diff" 或 ".patch" 的補丁檔案套用到 ${S} 目錄，如果不想使用這個補丁，可以顯示指定不套用。

</br>

---

</br>

<font color = red> 注意：很多時候我們會在配方檔案(.bb)中看見以下類似程式碼。 </font>

```
FILESPATH := "${THISDIR}/../../sources/${PN}:"
```

這種強制更改 FILESPATH 變數預設值的方法是不正確的，正確做法是採用 FILESEXTRAPATHS 變數來擴展搜尋目錄，例如下面做法：

```
FILESEXTRAPATHS_prepend := " ${THISDIR}/../../sources/XXX:${THISDIR}/../../sources/YYY:" 
# 同時擴展兩個路徑

FILESEXTRAPATHS_append := " ${THISDIR}/../EEE:" 
# 尾部的分號需要存在
```

在 .bbappend 檔案中只能使用 FILESEXTRAPATHS 變數。

關於 "_prepend" 和 "_append" 操作符說明將在後續講解。

---

</br>

#### THISDIR

bb 或 bbappend 檔案所在目錄，位於配方（recipe）檔案中。

例如某個配方檔案位於如下路徑：``` /work/open_source/openbmc/meta-aspeed/recipes-kernel/linux/linux-aspeed_git.bb ```

那麼在 linux-aspeed_%.bbappend 配方檔案中的 ```${THISDIR}``` 變數值為也包含 ```/work/open_source/openbmc/meta-aspeed/recipes-kernel/linux/```。

注意，```${THISDIR}``` 變數值是配方檔案所在目錄，也就是說在 bbappend 檔案中使用 ```${THISDIR}``` 變量，即包含 bb 檔案所在目錄，也包含 bbappend 檔案所在目錄，例如上面和下面兩個目錄都包含。

```
/work/open_source/openbmc/meta-ibm/meta-romulus/recipes-kernel/linux/linux-aspeed_%.bbappend
```

---

</br>

#### TMPDIR

此變數是 OpenEmbedded 建置系統用於所有建置輸出和中間檔案（共用狀態快取除外）的基本目錄，位於編譯目標層的 conf/local.conf.sample 檔案中（如有需要可在此檔案中修改），這個檔案將會被解析到 build/conf/local.conf 檔案中，預設情況下該變數指定的目錄為 build/tmp/。

---

</br>

#### PACKAGE_ARCH

套件架構名，位於配方（recipe）檔案中（一般不會在配方中自行指定）。

查看範例：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^PACKAGE_ARCH
PACKAGE_ARCH="4u_x201"
PACKAGE_ARCHS="all any noarch arm armv4 armv4t armv5 armv5t armv5e armv5te armv6 armv6t 4u_x201"
```

其中obmc-phosphor-image為映像名（可使用套件名稱），最終映像就會產生於build\tmp\deploy\images\4u-x201目錄中。

---

</br>

#### TARGET_OS

指定目標的作業系統。對於基於 glibc 的系統（GNU C庫），該變數可以設定為 “linux”，對於 musl libc 可以設定為 “linux-musl”。

對於 ARM/EABI 目標，存在 “linux-gnueabi” 和 “linux-musleabi” 可能的值。查看範例：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^TARGET_OS
TARGET_OS="linux-gnueabi"
```

---

</br>

#### PN

用於建立套件的配方名稱或套件的名稱，該變數位於配方檔案中（如有需要可在此檔案中修改，一般會自動擷取配方名稱）。

例如，如果配方名為 expat_2.0.1.bb，則 PN 預設值為 “expat” 。

這裡要注意一下，包名或配方名稱中不能使用下劃線_，在 Yocto 下劃線為版本分隔符號。

查看範例：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^TARGET_OS
TARGET_OS="linux-gnueabi"
```

---

</br>

#### PV

配方版本，該變數位於配方文件中（如有需要可在此文件中修改，一般會自動捕獲配方版本）。

例如，如果配方名為 expat_2.0.1.bb，則 PV 的預設值將為「2.0.1」。

查看範例：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^PV
PV="1.0"
```

---

</br>

#### PR

配方的修訂版本，該變數位於配方文件中（如有需要可在此文件中修改，一般需要手動指定）。

此變數的預設值為 “r0”，配方的後續修訂通常具有值 “r1”、“r2” 等。

當 PV 增加時，PR 通常重置為 “r0”。

查看範例：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^PR
PR="r0"
```

---

</br>

#### BP

變數的值基本配方名稱和版本，但沒有任何特殊配方名稱後綴（即-native、lib64- 等）。

BP 由組成為 ```${BPN}-${PV}```， 其檢視範例為：

```
~/work/bmc/build$ bitbake -e obmc-phosphor-image | grep ^BP
BP="obmc-phosphor-image-1.0"
```

---

</br>

#### WORKDIR

OpenEmbedded 建置系統在其中建立配方的工作目錄的路徑名。

該目錄位於 TMPDIR 目錄下，實際路徑基於正在建置的配方和正在建置的系統，預設定義如下：

```
${TMPDIR}/work/${MULTIMACH_TARGET_SYS}/${PN}/${EXTENDPE}${PV}-${PR}
```

- TMPDIR：頂層建置輸出目錄

- MULTIMACH_TARGET_SYS：目標系統標識符，預設值為 ```${PACKAGE_ARCH} ${TARGET_VENDOR} - ${TARGET_OS}``` ，範例值為 4u_x201-openbmc-linux-gnueabi 或 armv6-openbmc-linux-gnueabi。
- PN：配方名稱
- EXTENDPE：擴展前綴，如果PE未指定，EXTENDPE`則為空白，大多數食譜通常都是這種情況
- PV：配方版本
- PR：配方修改

---

</br>

#### S

解壓縮後的配方原始程式碼所在路徑，該變數位於配方檔案中（如有需要可在此檔案中修改）。

預設情況下，此目錄為 ```${WORKDIR}/${BPN}-${PV}```，如果來源壓縮套件將程式碼提取到名為 ```${BPN}-${PV}``` 以外的任何目錄，或者如果原始程式碼是從 SCM（例如 Git 或 Subversion）取得的，則必須在配方中設置 S，以便 OpenEmbedded 建置系統知道在哪裡可以找到解壓縮的原始程式碼。

假設 bb 檔案中指定來源碼來自 git，則在 do_fetch 期間，原始碼將被複製到 ```${WORKDIR}/git``` 目錄中。

由於此路徑與 S 的預設值不同，所以必須專門設置，才能定位到來源：

```
SRC_URI = "git://path/to/repo.git"
S = "${WORKDIR}/git"
```

---

</br>

總結：原始碼取得及處理階段就是根據 ```${SRC_URI}``` 變數指定方式來取得原始碼存放到 ```${WORKDIR}``` 路徑，若是壓縮包則解壓縮到 S 路徑下，若有修補程式檔案則套用到 S 目錄。

---

</br>

### 二、原始碼配置、原始碼編譯及成果物安裝

原始碼打好補丁（patch）後，bitbake 執行配置和編譯原始碼的任務。

編譯完成後，成果物檔案將複製到保存區域（暫存）以準備打包：

![alt text](image-4.png)

</br>

這個階段主要包含四個任務，分別為do_prepare_recipe_sysroot、do_configure、do_compile和do_install。

---

</br>

#### do_prepare_recipe_sysroot：

網路上大多資料都簡單帶過這個任務，未能講明白這個任務是做什麼的，但想要了解 Yocto 的配方文件共享機制就必須弄清楚這個任務是做什麼的！

do_prepare_recipe_sysroot 與 do_populate_sysroot 是 staging.bbclass 類別中關鍵任務，用於共享配方之間成果物！

拋出一個問題思考一下，如果一個配方 B 需要使用配方 A 的成果物怎麼辦（例如頭檔、動態/靜態連結庫、設定檔）？ 

Yocto 為了解決這個問題，提供了一套配方成果物共享機制，該機制分為兩階段：

1. 第一階段在 A 配方建構時完成。 </br> A 配方在建置時，需要在 do_install 任務中將需要共享的檔案安裝至 ```${D}``` 目錄，後續執行的 do_populate_sysroot 任務將自動拷貝 ```${D}``` 目錄下部分子目錄到 ```${SYSROOT_DESTDIR}```，而 ```${SYSROOT_DESTDIR}``` 目錄最終會放置到共享區（預設為build/tmp/sysroots-components）暫存，其他配方建置時就可以從共用區拷貝。 </br> </br>
那麼，```${D}``` 目錄下哪些子目錄會被自動拷貝呢？</br> 自動拷貝的目錄由三個變數指定，分別為 SYSROOT_DIRS （目標裝置需要儲存的子目錄）、SYSROOT_DIRS_BLACKLIST（目標裝置不需要儲存的子目錄）、SYSROOT_DIRS_NATIVE（本機裝置需要儲存的目錄），以 SYSROOT_DIRS 變數為例，其預設值為：

```
SYSROOT_DIRS = " \
         ${includedir} \
         ${libdir} \
         ${base_libdir} \
         ${nonarch_base_libdir} \
         ${datadir} \
     "
```

如果需要新增其他額外儲存的目錄，可以在配方檔案中增加 SYSROOT_DIRS += “YYY”。


2. 第二階段在 B 配方建置時完成。 </br> B 配方中添加 DEPENDS += "A"，便可使用 A 配方的成果物了。</br> bitbake 執行建造任務時會保證 B 配方的 do_prepare_recipe_sysroot 任務執行前，A 配方的成果物已位於 build/tmp/sysroots-components 中。 </br> </br>
任務 do_prepare_recipe_sysroot 會在 ```${WORKDIR}``` 目錄中建立兩個 sysroot 目錄並填充（所有依賴拷貝到其中），這兩個目錄名分別為 "recipe-sysroot" 和 "recipe-sysroot-native"（本機），其中 "recipe-sysroot" 給目標設備使用，A 配方產生的成果物就在裡面，另一個 "recipe-sysroot-native" 是給本機設備使用的。 </br> </br>
不知道目標設備與本機設備的差異？簡單講解一下：假設要為 arm 平台編譯 flash 固件，編譯主機是 x86 平台，那麼 <font color = red> 目標設備就是 arm 設備 </font>，本機設備就是 x86 編譯主機。 </br>
我們知道為 arm 設備編譯程式碼需要使用交叉編譯鏈（如 arm-linux-gcc），編譯鏈需要使用根檔案系統下的各種函式庫（或其他配方產生的頭檔及函式庫），因此便設定 "recipe-sysroot" 為編譯器使用的檔案系統。</br> </br>
注意了，原始碼編譯 do_compile 任務只是 bitbake 眾多任務當中的一個，那其他任務也需要使用函式庫或工具（如製作檔案系統工具、壓縮工具、cmake 工具）怎麼辦？所以 Yocto 將本機執行其他任務所需庫或工具都放置在 "recipe-sysroot-native"！

---

</br>

#### do_configure：

此任務用於完成編譯原始碼前的配置，配置可以來自配方本身，也可以來自繼承的類，一般情況我們都會使用autotools (配方中使用inherit autotools)、cmake類(配方中使用inherit cmake)或預設的make（不需要額外配置）。該任務運行時將當前工作目錄設為${B}（一般與${S}相同），該任務有個預設行為，即如果找到一個makefile( makefile, makefile，或GNUmakefile)並且CLEANBROKEN沒有設為“1”，則執行oe_runmake clean。

簡單說明一下該任務怎麼用：

* 軟體包編譯是基於 autotools 的，則可以使用 EXTRA_OECONF 或 PACKAGECONFIG_CONFARGS 變數添加其他設定選項，例如在配方檔案中添加如下：

```
EXTRA_OECONF += "--with-mib-modules="mib" \
                 --with-openssl=openssl \
                 --with-default-snmp-version="3" \
                 --with-logfile="/var/log/snmpd.log" \
                 --with-persistent-directory="/etc" \
                 --enable-privacy \
                 --enable-md5 \
                 --enable-des \
                 --prefiex=/xxx/yyy/ \
 "
```

則 do_configure 任務就如同手動執行 ```./configure ${EXTRA_OECONF} ${PACKAGECONFIG_CONFARGS}``` 一樣。

* 軟體包編譯是基於 cmake 的，則可以使用 EXTRA_OECMAKE 變數添加其他設定選項，例如在軟體包的配方檔案中添加如下：

```
EXTRA_OECMAKE = " \
     -DBMCWEB_INSECURE_ENABLE_REDFISH_FW_TFTP_UPDATE=ON  -DBMCWEB_INSECURE_DISABLE_SSL=ON  \
     -DBMCWEB_ENABLE_DEBUG=ON -DBMCW   EB_ENABLE_LOGGING=ON \
"
```

則 do_configure 任務就如同手動執行 ```mkdir build/ && cd build/ && cmake ${EXTRA_OECMAKE}``` 一樣。

* 軟體包編譯是基於 make 的，則可以使用 EXTRA_OEMAKE 變數添加其他設定選項，例如在軟體包的配方檔案中添加如下：

```
EXTRA_OEMAKE = "INSTALL_PREFIX=${D} OTHERLDFLAGS='${LDFLAGS}' HOST_CPPFLAGS='${BUILD_CPPFLAGS}'"
```

則 do_configure 任務相當於只做預設行為（EXTRA_OEMAKE 執行 make 時才傳入）。

---

</br>

#### do_compile：

編譯原始碼。

此任務運行時將目前工作目錄設為 ```${B}```（一般與 ```${S}``` 相同），該任務有個預設行為，即如果找到一個 makefile( makefile, makefile，或 GNUmakefile)，則運行 oe_runmake，若未找到此類檔案將不執行任何操作。

如果在執行 oe_runmake 時需要傳入額外編譯選項或連結庫，則可以使用在配方中下列變數：

```
CFLAGS += "-I${WORKDIR}/recipe-sysroot/usr/include/xxx  -DBMCW=ON" 
# gcc 的編譯選項，增加額外標頭檔路徑，定義 BMCW 巨集

CXXFLAGS = " -fPIC" 
# g++ 的編譯選項，告訴編譯器產生位置無關 code

LDFLAGS += "-L${WORKDIR}/recipe-sysroot/usr/lib -yyy" 
# 編譯器連接選項 Linker
```

---

</br>

#### do_install :

文件或成果物的安裝任務。

此任務會將編譯目錄 ```${B}``` 中需要打包的檔案（放到目標裝置中去的及其他配方所依賴的）複製到保存區 ```${D}``` 中。

注意：安裝檔案時不要把擁有者和群組 ID 設定錯誤，特別是使用 cp 指令時會保留原始檔案的 UID 和 GID，以下是建議的安全方法：

```
使用 install 命令
使用 cp 命令時加上 "--no-preserve=ownership" 選項
使用 tar 命令時加上 "--no-same-owner" 選項
```

這個任務就是把其他配方所需依賴安裝到 ```${D}``` 目錄，然後 do_populate_sysroot 任務才能去 ```${D}``` 目中拷貝。 假設 A 模組執行 do_install 的一個簡單範例：

```
do_install() {
    oe_runmake DESTDIR=${D}${libdir} install 
    # 執行 Makefile 中安裝任務（安裝 .so），傳入安裝目錄
    install -m 0644 -d ${D}${includedir}/api 
    # 建立標頭檔目錄
    install -m 0644 ${S}/api_common.h  ${D}${includedir}/api 
    # 安裝標頭檔
    install -m 0644 ${S}/api_xxx.h  ${D}${includedir}/api 
    # 安裝標頭檔
}
```

之後 B 模組在配方中加入如下：

```
DEPENDS += " A"
CFLAGS += " -I${WORKDIR}/recipe-sysroot/usr/include/api"
```

這樣B模組就可以使用A模組編譯的動態函式庫。

---

</br>

此階段涉及但未解釋的變數有以下：

#### B

套件建置的編譯目錄，一般情況 ```${B}``` 與 ```${S}``` 相同，即為 ```${WORKDIR}/${BPN}-${PV}```。

---

</br>

#### D

套件建構成果物的安裝目錄，也稱為目標目錄。 </br>
預設這個目錄為 ```${WORKDIR}/image```。

---

</br>

#### SYSROOT_DESTDIR

指向套件建置工作目錄下的暫存目錄，其預設值為 ```${WORKDIR}/sysroot-destdir```。

---

</br>

### 三、包的拆分處理

配置、編譯和安裝完成後，建置系統分析結果並將套件拆分處理。

例如將檔案 stripped 後放入 packages -split 目錄：

![alt text](image-5.png)

</br>

此階段分為三個任務，分別為 do_package、do_packagedata、do_populate_sysroot。

---

</br>

#### do_package、do_packagedata

do_package 和 do_packagedata 任務組合起來分析在 ```${D}``` 目錄中找到的文件，並根據可用的套件和文件將它們分成子集。

分析處理過程包括以下內容：去除偵錯符號，查看套件之間的共享庫依賴關係，以及查看套件之間的關係。

任務 do_packagedata 根據分析建立包元資料放置 Package Feeds（即 PKGDATA_DIR 指定目錄）中，這樣建置系統就可以從拿到套件產生最終的 image。

---

</br>

#### do_populate_sysroot

任務 do_populate_sysroot 在之前已經介紹過。

此任務將自動拷貝 ```${D}``` 目錄下部分子目錄到 ```${SYSROOT_DESTDIR}```，並將 ```${SYSROOT_DESTDIR}``` 目錄內容暫存至共享區（預設為build/tmp/sysroots-components）。

自動拷貝的子目錄由三個變數指定，分別為 SYSROOT_DIRS（目標設備需要保存的子目錄）、SYSROOT_DIRS_BLACKLIST（目標設備不需要保存的子目錄）、SYSROOT_DIRS_NATIVE（本機設備需要保存的目錄），

---

</br>

該階段會涉及以下變數：

---

</br>

#### PACKAGE_CLASSES

用於指定建置系統在打包檔案時使用何種套件管理器，該變數位於編譯目標層的 conf/local.conf.sample 檔案中（如有需要可在此檔案中修改），這個檔案將被解析到 build/conf/local.conf 檔案中，例如設定其值為 PACKAGE_CLASSES ?= "package_rpm package_tar"。

---

</br>

#### PKGD

在將包拆分為單獨的包之前，包的目標目錄。

---

</br>

#### PKGDESTWORK

任務 do_package 用來保存包元資料的臨時工作區 (即 pkgdata)。

---

</br>

#### PKGDEST

拆分後的套件的父目錄 (即 packages-split)。

---

</br>

#### PKGDATA_DIR

一個共享的全域狀態目錄，其中包含打包過程中產生的打包元資料。

打包過程將元資料從 PKGDESTWORK 複製到 PKGDATA_DIR 區域，在那裡元資料成為全域可用的。

---

</br>

#### STAGING_DIR_HOST

要執行元件的系統的系統根路徑 (即 recipe-sysroot)，也就是目前配方原始碼編譯時的根檔案系統，裡麵包含配方所需依賴及交叉編譯器所需依賴。

---

</br>

#### STAGING_DIR_NATIVE

為建置主機建置元件時所使用的系統根路徑 (即 recipe-sysroot-native)。

---

</br>

#### STAGING_DIR_TARGET

建立在系統上執行的元件並為另一台機器產生程式碼（例如 cross-canadian recipes）時使用的 sysroot 路徑，一般與 STAGING_DIR_HOST 一樣。

---

</br>

#### FILES

用於指定套件（模組）安裝在 ```${D}``` 目錄中哪些成果物要打包，該變數位於配方檔案中（如有需要可在此檔案中修改）。

簡單來說就是在 do_install 任務中安裝在 ```${D}``` 目錄下的檔案不會都打包，以 rsa 模組為例該變數的預設值為：

```
FILES_rsa="/usr/bin/* /usr/sbin/* /usr/libexec/* /usr/lib/lib*.so.*             /etc /com /var             /bin/* /sbin/*             /lib/*.so.*             /lib/udev /usr/lib/udev             /lib/udev /usr/lib/udev             /usr/share/hikrsa /usr/lib/hikrsa/*             /usr/share/pixmaps /usr/share/applications             /usr/share/idl /usr/share/omf /usr/share/sounds             /usr/lib/bonobo/servers"
```

也就是說，只要成果物安裝在上面這些目錄下的都會參與打包，當然如果不放心或需要額外增加文件，可以在配方文件中顯性指定：

```
FILES_${PN} += " \
        ${sbindir}/rsaverify \
" 
#${sbindir} 莫認為 /usr/sbin/
```

---

</br>

總結：包拆分處理階段是根據 conf 配置將 ```${D}``` 目錄中成果物打包放置於 Package Feeds 區域，同時產生包元數據，最後將其他配方可能用到的文件放置於文件共享區（為其他配方提供依賴）。

---

</br>

### 四、image 生成

這個階段在一般套件（模組）編譯過程中不存在！ ！ ！

可透過 bitbake -c listtasks XXX（包名或固件名）指令查看編譯任務清單。

一旦軟體包被拆分並儲存在Package Feeds 區域中，建置系統將使用bitbake 產生根檔案系統映像（image）：

![alt text](image-6.png)

</br>

此階段涉及兩個任務，分別為 do_rootfs、do_image。

---

</br>

#### do_rootfs

任務將創建目標設備的根檔案系統（將需要打包至目標設備的程式、庫、檔案等都放置到根檔案系統中），這個根檔案系統最終會打包到image中。 

do_rootfs任務會透過來最佳 ROOTFS_POSTPROCESS_COMMAND 化檔案大小（如 mklibs 流程優化了庫的大小，同時 prelink 優化了共享庫的動態連結以減少執行檔的啟動時間），ROOTFS_POSTPROCESS_COMMAND 如下：

```
ROOTFS_POSTPROCESS_COMMAND() {
    write_package_manifest; 
    license_create_manifest;   
    ssh_allow_empty_password;  
    ssh_allow_root_login;  
    postinst_enable_logging;
    rootfs_update_timestamp ;   
    write_image_test_data ;  
    set_systemd_default_target; 
    systemd_create_users; 
    empty_var_volatile;
    remove_etc_version ;  
    set_user_group; 
    sort_passwd; 
    rootfs_reproducible;
}
```

建立的檔案系統所在位置由 IMAGE_ROOTFS 變數指定，檢視範例：

```
~/work/bmc$ bitbake -e obmc-phosphor-image | grep ^IMAGE_ROOTFS
IMAGE_ROOTFS="~/work/bmc/build/tmp/work/4u_x201-openbmc-linux-gnueabi/obmc-phosphor-image/1.0-r0/rootfs"
```

如果我們修改了某個程序，但又不想重新燒寫整個固件，那就去這個目錄下找到程序，再通過 TFTP 方式（或 NFS 直接掛載）下載到目標是設備調試即可。

---

</br>

#### do_image

任務 do_image 會透過 IMAGE_PREPROCESS_COMMAND 對 image 進行預處理，主要是優化 image 大小， IMAGE_PREPROCESS_COMMAND 如下：

```
IMAGE_PREPROCESS_COMMAND() {
 mklibs_optimize_image;  prelink_setup; prelink_image;  reproducible_final_image_task;
}
```

建置系統 do_image 根據需要動態產生支援的 do_image_* 任務，產生的任務類型取決於 IMAGE_FSTYPES 變數。

do_image_* 任務將所有內容轉換為一個 image 文件或一組 image 文件，並且可以壓縮根文件系統 image 大小，以減少最終燒寫到目標設備的 image 整體大小。用於根檔案系統的格式取決於 IMAGE_FSTYPES 變量，壓縮取決於格式是否支援壓縮。

生成 image 完成後執行最後一個任務 do_image_complete，該任務將透過 IMAGE_POSTPROCESS_COMMAND 完成 image 的後續處理，預設 IMAGE_POSTPROCESS_COMMAND 為空，檢視範例：

```
~/work/bmc$ bitbake -e obmc-phosphor-image | grep ^IMAGE_POSTPROCESS_COMMAND
IMAGE_POSTPROCESS_COMMAND=""
```

---

</br>

此階段涉及變數如下：

---

</br>

#### IMAGE_INSTALL

此變數指明 Package Feeds 區域安裝的基本軟體包集中，哪些包（模組）最終要打包到 image，該變數一般位於編譯目標層的 conf/local.conf.sample 文件中（如有需要可在此文件中修改），這個文件將被解析到 build/conf/layer.conf文件中。

注意與 FILES 差異，FILES 是指明軟體包內部哪些文件需要參與打包，IMAGE_INSTALL 而是指明哪個軟體包需要參與打包。

一個簡單範例：

```
IMAGE_INSTALL_append += ”rsa"
```

---

</br>

#### PACKAGE_EXCLUDE

指定不應安裝到 image 中的套件。

---

</br>

#### IMAGE_FEATURES

指定要包含在影像中的特徵，大多數這些功能會對應到其他安裝套件（未能弄清楚具體作用）。

---

</br>

#### IMAGE_LINGUAS

決定安裝附加語言支援包的語言，該變數一般位於編譯目標層的 conf/local.conf.sample 檔案中（如有需要可在此檔案中修改），這個檔案將會被解析到 build/conf/layer.conf 檔案中。預設為：

```
~/work/bmc$ bitbake -e rsa | grep ^IMAGE_LINGUAS
IMAGE_LINGUAS="en-us en-gb"
```

---

</br>

#### PACKAGE_INSTALL

傳遞給套件管理器以安裝到映像中的套件的最終清單。

---

</br>

#### DEPLOY_DIR

最終 image 和 SDK 輸出的目錄，預設值為 build/tmp/deploy/。

---

</br>

總結：image 生成生成階段就是創建目標設備的根檔案系統，並將需要打包至目標設備的程式、庫、檔案等都放置到根檔案系統中，然後對檔案和整個檔案系統進行優化壓縮，最終生成 image。

---

</br>

![alt text](image-7.png)

---

</br>

# Yocto 任務與語法

Recipes 在 yocto 用 .bb 檔案表示. 而 .bbappend 用於修改 .bb 的設定。

什麼是 bbappend 檔案？
* 副檔名為 .bbappend 的檔案稱為 Bitbake 附加檔案。此文件用於修改配方文件。
* Yocto 專案是一個開源項目，如果您想修改配方文件，建議建立您的圖層並建立 .bbappend 檔案來修改配方。
* bbappend 檔案名稱應與配方包名稱相同。
* 例如，如果配方檔案名稱為 busybox_1.32.0.bb，並且您想要修改 BusyBox 軟體包，則需要建立一個 busybox_%.bbappend 檔案。



---

</br>

# 添加程序與腳本

## 添加自己的 Layers（利用 bitbake 工具建立 layer）

### 事前準備工作

在先前的使用中可以知道，當我們使用 ```bitbake [image]``` 時會根據 image 的 conf 檔進行相關的 recipes 操作，此時可以利用 bitbake-layers 建立自己的 layer 於 build 目錄底下。再透過 bitbake-layers 把 layer 加入到 bblayers.conf 設定檔。

</br>

---

Step 1. bitbake-layers create-layer [my-layer]
* my-layer 填入自己的 layer 名稱
* 此 CMD 操作為藉由 bitbake 工具先創立一個自己的 meta layer

```
bitbake-layers create-layer my-meta-layer
```

Step 2. bitbake-layers add-layer [my-layer]
* my-layer 填入剛剛所加入的 layer 名稱
* 此 CMD 操作為藉由 bitbake 工具先創立 meta layer 的資料夾於工作區域
* 工作區域會位於 ```[build]``` 裡

```
bitbake-layers add-layer my-meta-layer
```

</br>

![add_recipes_use_bitbake_layers_tools_CMD](add_recipes_use_bitbake_layers_tools_CMD.png)

---

### 新增 Recipes

以 devtool 與 recipetool 個別示範如何產生 recipe。

經由工具產生 recipe 的時候，若是 source code 來源（e.g. git, ftp, tar, etc.）以及編譯方式（cmake, make, etc.）是可以被解析的。

經工具所產生的 recipe 幾乎都不需要多做修改，而以下說明兩個範例各自的情境：
* devtool 範例：使用 GIFLIB 套件。由於 GIFLIB 基於 autotool, configure 與 Makefile。
   * devtool 可以分析並幫助我們產生無須修改的 recipe。
* recipetool 範例：這邊以自身撰寫程式的經驗。
   * 程式只會有 header file 與 source file。
   * 並搭配 Makefile 來編譯的專案。所以 recipetool 產生的 recipe 需要修改.

</br>

#### devtool

devtool 產生 recipe 時候，會先建立一個 workspace layer，並在目錄底下產生 giflib recipe。

<font color=red>由於 source code 格式符合 yocto。產生出來的 recipe 並不需要額外修改。</font>



---

</br>