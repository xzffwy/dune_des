__dune_enter
   1、推入栈，扩展栈的空间112，rsp=rsp-112；
   2、调用SAVE_REGS宏保存rdi~r11,rcx,rax的值
   3、调用SAVE_REST宏保存其余的寄存器的值
   4、给现在的rsp栈顶指针保存，其中的汇编操作有点不懂,但是调用了函数ioctl,其中ioctl为DUNE_ENTER,进入了设备io控制，在这之前的操作都是配置和设置，可以说没怎么懂,主要是参数是怎么传递过来的。
   5、没有看。。。
