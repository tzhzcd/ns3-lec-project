#project4实验报告
##说明
* 参考程序：projec2_2.cc  example/wireless/mixed-wireless.cc(ad-hoc骨干)
* 更改：
   * 1.移动模型，和初始位置 ok
	* 2.传播模型改为　ConstantSpeedPropagationDelayModel  ok 
	* 3.Mac类型为：NqosWifiMacHelper（发现一样，不需要修改） ok
	* 4.packet size为512byte，包间隔为100ms ok 
	* 5.wifi1、wifi2之间用ad-hoc连接　fail
* 说明：wifi1、wifi2之间改用ad-hoc连接失败了，现象是客户端发送正常，可是服务器回写失　败，无报错。猜想信道配置出错，由于时间问题，本project仍用p2p信道连接AP。出错程序见project4_WRONG.cc

##仿真拓扑
![拓扑] (http://ww1.sinaimg.cn/mw690/87f91db3gw1f5i3atd0poj20ez05caan.jpg "仿真拓扑")
##PyViz截图（能看到计算机用户名）
![PyViz] (http://ww2.sinaimg.cn/mw690/87f91db3gw1f5i3fbq5p8j20hx0crgn4.jpg "PyViz")
##数据处理
以回写长度为自变量，回写时间为因变量，处理数据，并画点线图。<br>
数据： 
![data] (http://ww4.sinaimg.cn/mw690/87f91db3gw1f5hnrpmeaaj20w801eaat.jpg "数据")<br>
画点线图： <br> 
![plot] (http://ww4.sinaimg.cn/mw690/87f91db3gw1f5hnmrart8j20hv0dfwfd.jpg "折线图")<br>
