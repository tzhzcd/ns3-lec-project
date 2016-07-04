#project3实验报告
#####说明：本次project以project2_2为基础，除记录部分没做任何改动。系统日志信息截图、PyViz截图见report2。
<br>
##Wireshark查看文件截图（能看到计算机用户名）
![wireshark] (http://ww1.sinaimg.cn/mw690/87f91db3gw1f5grwkfkr4j20l00gbgs8.jpg "wirshark")
##数据处理
以回写长度为自变量，回写时间为因变量，处理数据，并画点线图。<br>
数据： 
![data] (http://ww4.sinaimg.cn/mw690/87f91db3gw1f5hnrpmeaaj20w801eaat.jpg "数据")<br>
画点线图： <br> 
![plot] (http://ww4.sinaimg.cn/mw690/87f91db3gw1f5hnmrart8j20hv0dfwfd.jpg "折线图")<br>
##安装手册
* PyViz：
  * PyViz安装依赖包<br>
sudo apt-get install python-dev python-pygraphviz python-kiwi python-pygoocanvas python-gnome2 python-rsvg
  * 安装交互式python:<br>
sudo.apt-get.instal.ipython
  * PyViz使用前准备
在程序中包含：<br>
CommandLine cmd;
cmd.Prase (argc,argv);
  * PyViz使用<br>
在运行脚本的语句后面加入参数 –vis
* gnuplot:
  * 到安装路径，进入特权模式： su   然后 cd /usr/local/src
  * 下载gnuplot安装包：<br>
wget http://nchc.dl.sourceforge.net/project/gnuplot/gnuplot/4.4.0/gnuplot-4.4.0.tar.gz
  * 解压:tar -zxvf gnuplot-4.4.0.tar.gz
  * 配置:cd gnuplot-4.4.0   然后./configure
  * 编译:make
  * 安装:make install
