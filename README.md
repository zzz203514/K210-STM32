# K210-STM32-
k210.py中为串口通信中K210发送信息的代码，用于传输识别出的二维码和颜色等信息
使用重定义让K210的引脚可以用于通信，注意与stm32端的RX与TX的连法

注意：K210中的传输数据前需要对数据进行打包，其中传输数据为16位的，而stm32端接收是8位的，即K210的一个数据会变为两个数据在STM32端，
因此要进行数据验证

uart是STM32端的接收源代码，使用的是HAL库，搭配STM32CubeMX使用
在stm32端要想看到收到的信息要使用另外一个串口与上位机通信，同时要重定义printf函数，建议放到usart.c文件里面

注意：当使用串口中断通信时，必须先在串口初始化之后开启中断。若接收长度为1，则相当于阻塞传输，可以直接在回调函数中再次开启中断，
从而使不断进行通信。若指定长度则需要把再次开启中断部分的代码放入IQHeader函数中