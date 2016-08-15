# KinectV2-for-openFrameworks
KinectV2のColor, Body, DepthをopenFrameworksに移植したやつ  

### 環境  
  Windows 10  
  openFrameworks v0.9.3  
  VisualStudio Community 2015  

### VSの設定  
  追加のインクルードディレクトリを設定する  
  ```
  C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc 
  ```
  追加のライブラリディレクトリを設定する  
  ```
  C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\Lib\x86 
  ```
  追加の依存ファイルに Kinect20.lib を設定する  
  NuGetパッケージの管理から opencv3.1 をインストール  

### 参考サイト  
<http://izmiz.hateblo.jp/entry/2015/02/08/222217>  
<http://www.buildinsider.net/small/kinectv2cpp/05>  
<http://unanancyowen.com/?p=1665>
