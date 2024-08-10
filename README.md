# NB-IoT三层代码封装

# **开发目的**

跨平台技术允许开发者编写一次代码并在多个操作系统或设备上运行。这种方式显著减少了需要编写和维护的代码量，从而降低了开发和维护成本。

# 本Git介绍

**目录结构**

```
+--- Inc
|   +--- at-bc28.h
|   +--- at_cmd.h
|   +--- usart.h
+--- README.md
+--- Src
|   +--- at-bc28.c
|   +--- at_cmd.c
|   +--- usart.c
```

- **Src**

  该目录下存放了串口层、AT命令解析层以及bc28 模块AT命令层，通过对函数的封装与抽象实现了这三层代码能在Linux和单片机侧运行，提高了代码的兼容性。

- **Inc**

  该目录下存放了对应的头文件，实现函数的声明。

# 使用

使用 `#ifdef` 指令可以在不同平台上选择性地编译特定代码段，从而实现跨平台支持。通过在代码中使用条件编译，你可以根据定义的宏来控制不同平台上的代码执行路径。

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

