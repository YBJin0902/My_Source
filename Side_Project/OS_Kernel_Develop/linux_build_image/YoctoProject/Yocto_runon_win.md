# Yocto run on Windows

## 前言與注意事項

Yocot 主要是在 Linux 上建置的非不得已才在 Windows 上建置環境。

</br>

主要使用工具與作業系統環境：
* Host OS：Windows 11
* Docker \ WSL2 
* Yocto Project（4.0 演示為主）

</br>

---

</br>

# 環境安裝

### Docker 安裝

基本上網路上都可以找到資料，所以這裡不再贅述。

以下提供 Dockerfile，由於我使用 Yocto 4.0 所以選用 Ubuntu 20：

```dockerfile
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    tzdata locales dialog sudo \
    gawk wget git-core diffstat unzip texinfo gcc-multilib \
    build-essential chrpath socat cpio python3 python3-pip \
    python3-pexpect xz-utils debianutils iputils-ping curl file \
    python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev \
    pylint3 xterm zstd liblz4-tool rsync bc screen aria2 \
 && rm -rf /var/lib/apt/lists/*

# locale & timezone
RUN locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    ln -fs /usr/share/zoneinfo/Asia/Taipei /etc/localtime && \
    dpkg-reconfigure -f noninteractive tzdata

ENV LANG=en_US.UTF-8
ENV PATH="/root/bin:${PATH}"

# Install Google's repo tool (Yocto/Android常用)
RUN curl -fsSL https://storage.googleapis.com/git-repo-downloads/repo -o /usr/local/bin/repo && \
    chmod +x /usr/local/bin/repo

# 建 yocto 使用者（避免用 root 跑 bitbake）
RUN useradd -ms /bin/bash yocto && echo "yocto ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER yocto
WORKDIR /home/yocto
CMD ["/bin/bash"]
```

</br>

---

不同的 Yocto Project 會要求不同的 Linux 版本，但就算是最新的 Yocto 也不推薦用最新的 Ubuntu 因為後續在跑 bitbake 時會有很多權限問題，最推薦還是 Ubuntu 20 須注意！！！

---

</br>

### Docker 容器環境建置

撰寫完 Dockerfile 後我們需要根據需求建置容器

</br>

Step 1. 建置容器

```bash
docker build -t yocto-focal:20.04 -f Dockerfile .
```

---

</br>

Step 2. 建立 persistent volumes，這一步是為了 Yocto 編譯時的下載與快取，若是不設定該路徑後續會容易出現問題。

```bash
docker volume create yocto_downloads
docker volume create yocto_sstate
docker volume create yocto_build
```

---

</br>

Step 3. Run 容器

```bash
docker run --name yocto-dev -it   -v yocto_downloads:/home/yocto/downloads   -v yocto_sstate:/home/yocto/sstate-cache   -v yocto_build:/home/yocto/build   -v C:\path\to\your\yocto-src:/home/yocto/Project   yocto-focal:20.04
```

---

</br>

至此環境基本設定完成。

</br>

# Yocto Project

根據需求安裝，這裡以 STM32MP157F-DK2 所需要的環境為示範：

</br>

## Git clone

```bash
git clone -b kirkstone git://git.yoctoproject.org/poky.git
git clone -b kirkstone git://git.openembedded.org/meta-openembedded
git clone -b kirkstone https://github.com/STMicroelectronics/meta-st-stm32mp.git
```

繼續下一步以前先確定所有的 git source 都有正確下載！

</br>

## 建置 Yocto 所需環境

基本

```bash
cd /project
source poky/oe-init-build-env build
```

---

</br>

接下來，需要設定 Yocto 所需的 download 與快取資料夾設定：

設定 local.conf，把目錄指到 Linux 端的路徑

```bash
sed -i 's|^#DL_DIR ?=.*|DL_DIR ?= "/home/yocto/downloads"|' conf/local.conf
sed -i 's|^#SSTATE_DIR ?=.*|SSTATE_DIR ?= "/home/yocto/sstate-cache"|' conf/local.conf
```

</br>

在來是權限問題：

```bash
sudo mkdir -p ~/downloads ~/sstate-cache ~/build
sudo chown -R yocto:yocto ~/downloads ~/sstate-cache ~/build
sudo chmod -R u+rwX ~/downloads ~/sstate-cache ~/build
```

因為 Yocto 本身會鎖 root 權限操作，但是 windows 又需要有權限才可以操作，所以才會有這一步。

</br>

### 編譯

接下來就試著編譯試試看~

```bash
bitbake core-image-minimal
```

</br>

# 參考資料

[在 STM32MP1 上使用 Yocto 建置 Linux 系統](https://e61983.github.io/posts/stm32mp-with-yocto/)

</br>

# 額外 Bouns

其實我們可以使用 VS code 作為管理與編譯，但是不要太依靠，因為有時候 VS code 會害 Yocto 網路出錯導致編譯出錯。

唯一推薦是使用 Docker 的套件，Yocot Project 也有套件。

</br>

[使用 Visual Studio Code 遠端操作 docker 環境下的檔案](https://medium.com/%E5%A4%BE%E7%B8%AB%E4%B8%AD%E6%B1%82%E7%94%9F%E5%AD%98%E7%9A%84%E4%BA%BA%E9%A1%9E/%E4%BD%BF%E7%94%A8visual-studio-code-%E9%81%A0%E7%AB%AF%E6%93%8D%E4%BD%9Cdocker%E7%92%B0%E5%A2%83%E4%B8%8B%E7%9A%84%E6%AA%94%E6%A1%88-ebb35292a5b1)

[Yocto Project on VS code](https://blog.csdn.net/wpgddt/article/details/143796960)