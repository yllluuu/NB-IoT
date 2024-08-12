# NB-IoT三层代码封装

# **开发目的**

跨平台技术允许开发者编写一次代码并在多个操作系统或设备上运行。这种方式显著减少了需要编写和维护的代码量，从而降低了开发和维护成本。

# 本Git介绍

**目录结构**

```
+---API
|   +---inc
|   |     +--- at-bc28.h
|   |     +--- at_cmd.h
|   |     +--- usart.h
|   +---src
|   |     +--- at-bc28.c
|   |     +--- at_cmd.c
|   |     +--- usart.c
+---BearKE1
|   +---inc
|   |     +--- gpio_led.h
|   |     +--- FreeRTOSConfig.h
|   |     +--- gpio_i2c.h
|   |     +--- main.h
|   |     +--- sht30.h
|   |     +--- tim.h
|   +---src
|   |   +--- freertos.c
|   |   +--- gpio_led.c
|   |   +--- gpio_i2c.c
|   |   +--- main.c
|   |   +--- sht30.c
|   |   +--- tim.c
+---Linux
|   +---inc
|   |   +--- nbiot.h
|   |   +--- leds.h
|   |   +--- sht20.h
|   |   +--- logger.h
|   +---src
|   |   +--- leds.c
|   |   +--- sht20.c
|   |   +--- logger.c
|   |   +--- nbiot.c
|   |   +--- makefile         #生成静态库
|   +---lib
|   |   +--- gpiod
|   |      +--- build.sh     #gpiod库的构建脚本
|   |      +--- makefile  
+--- makefile             #进行编译连接生成可执行文件
+---main.c                    #业务逻辑层代码
+--- README.md
```

**API：**

- src

  该文件下存放串口层、AT命令解析层以及bc28 模块AT命令层，通过对函数的封装与抽象实现了这三层代码能在Linux和单片机侧运行，提高了代码的兼容性。

  at-bc28.c:bc28联网所用的命令层。

  at_cmd.c：解析AT命令层。

  usart.c：串口收发层。

- inc

  该目录下存放了对应的头文件，实现函数的声明。

**Linux：**

- src

  该文件夹下保存了开发sht30温湿度采集等除跨平台代码外的其他项目实现的源文件。

  1. sht20.c：通过sht20传感器获取当前温湿度。
  2. leds.c：初始化绿灯引脚，实现灯的开关。
  3. logger.c: 日志文件，用来监测程序运行情况
  4. nbiot.c：通过多线程实现整个数据上报和接收下发指令的功能。
  5. makefile：生成自己代码所需的静态库

- inc

  该目录下存放了对应的头文件，实现函数的声明。
  
- lib/gpiod

  该目录存放build.sh用于gpiod库的构建脚本，以及makefile文件用来执行bulid.sh文件

- main.c文件式Linux侧的业务逻辑层

- makefile文件用来进入lib/gpiod文件夹下生成gpiod开源库以及进入src下生成代码所用静态库，并且生成可执行文件

**BearKE1：**

- src

  该文件夹下保存了开发sht30温湿度采集、AT命令解析、AT命令发送及整体项目实现的源文件。

  1. sht30.c：通过sht30传感器获取当前温湿度。
  2. gpio_i2c.c:利用gpio模拟i2c。
  3. gpio_led.c：初始化绿灯引脚，实现灯的开关。
  4. main.c：通过FreeRTOS系统管理任务的调度，实现整个数据上报和接收下发指令的功能。

- inc

  该目录下存放了对应的头文件，实现函数的声明。

  

# 使用

使用 `#ifdef` 指令可以在不同平台上选择性地编译特定代码段，从而实现跨平台支持。通过在代码中使用条件编译，你可以根据定义的宏来控制不同平台上的代码执行路径。Linux侧代码在Linux文件夹下，单片机侧代码在BearKE1文件夹下。

```
#ifdef CONFIG_OS_STM32
//code for stm32
#elif (defined CONFIG_OS_LINUX)
//code for linux
#endif
```

在使用时只需要操作以下步骤，通过条件编译选择想要的代码：

```
#define CONFIG_OS_STM32//在STM32环境下

#define CONFIG_OS_LINUX//在Linux环境下
```
