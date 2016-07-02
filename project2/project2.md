#第二次试验报告
##一、拓扑图
###project2_1<br>
参考：third  
    改变：wifi节点的移动模型；4个wifi节点都安装客户端
![project2_1拓扑](http://ww1.sinaimg.cn/mw690/87f91db3gw1f5fmeuugybj20g305n0tg.jpg "project2_2_拓扑")<br>
### project2_2<br>
参考：project2_1  
    改变：csma网络换为wifi2网络，服务器安装在最右的wifi节点上
![project2_2拓扑](http://ww3.sinaimg.cn/mw690/87f91db3gw1f5fmmajjv4j20hn064dh1.jpg "project2_2_拓扑")<br>
##二、通信效果截图
###project2_1<br>
![project2_1通信效果](http://ww1.sinaimg.cn/mw690/87f91db3gw1f5fn02yd7oj20hn07z0xz.jpg "project2_2_通信效果")<br>
### project2_2<br>
![project2_2通信效果](http://ww4.sinaimg.cn/mw690/87f91db3gw1f5fmyw59k0j20ho097grr.jpg "project2_2_通信效果")<br>
##三、STA移动模型部分代码截图
    project2_1 project2_2基本一样。
    以AP位置为初始位置，不同节点以不同数度(n/2.0+0.5)，朝相同方向(x轴正方向)，匀速远离AP。
![移动模型代码](http://ww2.sinaimg.cn/mw690/87f91db3gw1f5fn529hdmj20h709zn19.jpg "移动模型代码")<br>
