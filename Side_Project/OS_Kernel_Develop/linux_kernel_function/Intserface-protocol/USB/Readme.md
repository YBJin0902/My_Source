# USB

這裡主要介紹 linux 下的 USB 開發與設計，請先詳讀上篇的 USB 文章後再來讀這篇。

</br>

# linux USB

在 linux 的作業系統上有一個好處，當我們在開發 USB Driver 時我們並不用實際去撰寫底層，因為其實都幫我們寫好了，如下圖所示我們實際需要撰寫的其實只有 USB Device Driver。

</br>

先從簡單的 User Space 下手開始，相信各位在使用 ubuntu 時在找 USB 設備都會用到 `lsusb`，在 CMD 中就會出現現在所插著的設備，那這個 `lsusb` 的功用其實就是用一個軟體事件去 call linux core 中的 USB core 去換出目前辨識到的 USB 設備。

除了基本的 `lsusb` 之外我們也可以使用 `lsusb -v -d` 去查看更加詳細的 USB Device 訊息。

</br>

那我們要做的事情其實已經很容易倒出了：<font color=red>寫一個 USB Driver 程式去使用這些 linux 為我們準備好的 API</font>

</br>

## Linux USB 架構



</br>

![linux usb 架構](./images/linux-usb-架構.png#pic_center=100x150)