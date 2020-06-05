### Reactor关键模型



Eventloop 事件循环；

![ReactorModel](https://gitee.com/lalalilia/NetWordCode/raw/master/untitled1/images/ReactorModel.jpg)



EventLoop 核心工作步骤：

**1.建立过程：**

**首先**建立一个EventLoop类，在类中调用updateChannel()来更新poller中的事件表，到此一个事件就加入了EventLoop工作的流程中；

**2.执行过程：**

在Eventloop类中loop()函数中调用Poller类中的poll()来获取活跃事件，并总的记录在一个活跃事件表activeChannels中，之后遍历这个活跃事件表，逐个调用活跃事件Channel中的handleEvent()来回调事件函数。

在此中，EventLoop就是领导，Poller是书记员，Channel是打工仔，领导日常询问书记员（调用poll()），书记员找到有工作请求的打工仔（poll()中寻找活动的fd然后更新到活跃事件activeChannel中），执行官指导后就嘱咐打工仔干他对应的活（activeChannel->handleEvent()）

**3.runInLoop/queueInLoop：**	

走后门的runInLoop，在EventLoop中，领导人EventLoop专门设置了一个doPendingFunctor()环节，来额外接受不在poller记录本里面的事件，但还存在一个pendingFunctors_，专门记录走后门名单，领导人EventLoop平常都会阻塞在poll()函数中但没事做，如果这是有人走后门（queueInLoop）了，就会weakup()直接通知到poll()函数中，让EventLoop可以跳出poll()的阻塞，然后来doPendingFunctor()。