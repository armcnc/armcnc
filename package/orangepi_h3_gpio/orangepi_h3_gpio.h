// Copyright 2024 ARMCNC, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ARMCNC_GPIO__
#define __ARMCNC_GPIO__

// 包含实时系统API相关头文件
#include "rtapi.h"
// 包含实时系统应用程序相关头文件
#include "rtapi_app.h"
// 实时数学函数
#include "rtapi_math.h"
// 硬件抽象层
#include "hal.h"
// 标准工具函数
#include <stdlib.h>
// 包含POSIX标准定义的符号常量和类型
#include <unistd.h>
// 包含固定宽度整数类型定义
#include <stdint.h>
// 包含通用工具函数和常量定义
#include <stdlib.h>
// 包含字符串操作相关函数
#include <string.h>
// 包含文件控制选项定义
#include <fcntl.h>
// 包含内存管理声明
#include <sys/mman.h>
// 包含文件系统用户ID处理函数
#include <sys/fsuid.h>
// 包含时间相关函数和类型定义
#include <time.h>

// ARISC CPU频率定义(450MHz)
#define ARISC_CPU_FREQ          450000000 // Hz
// ARISC固件在内存中的基地址(对ARM CPU来说是0x00040000)
#define ARISC_FW_BASE           (0x00040000)
// ARISC固件大小定义(48KB = (8+8+32)*1024)
#define ARISC_FW_SIZE           ((8+8+32)*1024)
// 共享内存大小定义(4KB)
#define ARISC_SHM_SIZE          (4096)
// 共享内存基地址(位于固件末尾)
#define ARISC_SHM_BASE          (ARISC_FW_BASE + ARISC_FW_SIZE - ARISC_SHM_SIZE)

// GPIO寄存器基地址
#define GPIO_BASE               0x01c20800
// GPIO-R组寄存器基地址(通常用于电源管理)
#define GPIO_R_BASE             0x01f02c00
// 每个GPIO端口组(bank)的寄存器大小
#define GPIO_BANK_SIZE          0x24

// 最大GPIO端口组数量
#define GPIO_PORTS_MAX_CNT      8
// 每个端口组最大引脚数量
#define GPIO_PINS_MAX_CNT       24

// GPIO功能模式的枚举定义
enum
{
    GPIO_FUNC_IN,        // 配置为输入模式
    GPIO_FUNC_OUT,       // 配置为输出模式
    GPIO_FUNC_2,         // 配置为复用功能2
    GPIO_FUNC_3,         // 配置为复用功能3
    GPIO_FUNC_RESERVED4, // 保留功能4
    GPIO_FUNC_RESERVED5, // 保留功能5
    GPIO_FUNC_EINT,      // 配置为外部中断功能
    GPIO_FUNC_DISABLE,   // 禁用此引脚
    GPIO_FUNC_CNT        // 功能模式总数
};

// GPIO多驱动强度等级枚举定义
enum
{
    GPIO_MULTI_DRIVE_LEVEL0,    // 驱动强度等级0(最弱)
    GPIO_MULTI_DRIVE_LEVEL1,    // 驱动强度等级1
    GPIO_MULTI_DRIVE_LEVEL2,    // 驱动强度等级2
    GPIO_MULTI_DRIVE_LEVEL3,    // 驱动强度等级3(最强)
    GPIO_MULTI_DRIVE_LEVEL_CNT  // 驱动强度等级总数
};

// GPIO上下拉配置枚举定义
enum
{
    GPIO_PULL_DISABLE,    // 禁用上下拉
    GPIO_PULL_UP,         // 使能上拉
    GPIO_PULL_DOWN,       // 使能下拉
    GPIO_PULL_RESERVED3,  // 保留设置
    GPIO_PULL_CNT         // 上下拉设置总数
};

// GPIO端口组枚举定义
enum { PA, PB, PC, PD, PE, PF, PG, PL };  // 定义所有可用的GPIO端口组

// GPIO电平状态枚举定义
enum { LOW, HIGH };  // 定义GPIO的低电平和高电平状态

// 设置GPIO引脚输出高电平的宏
#define GPIO_PIN_SET(PORT,PIN_MASK) \
    _GPIO[PORT]->data |= PIN_MASK

// 设置GPIO引脚输出低电平的宏
#define GPIO_PIN_CLR(PORT,PIN_MASK_NOT) \
    _GPIO[PORT]->data &= PIN_MASK_NOT

// 获取GPIO引脚输入状态的宏
#define GPIO_PIN_GET(PORT,PIN_MASK) \
    (_GPIO[PORT]->data & PIN_MASK)

// GPIO端口寄存器结构体定义
typedef struct
{
    uint32_t config[4];    // 引脚功能配置寄存器(每个引脚3位配置位)
    uint32_t data;         // 数据寄存器
    uint32_t drive[2];     // 驱动能力配置寄存器
    uint32_t pull[2];      // 上下拉配置寄存器
} _GPIO_PORT_REG_t;

// PWM最大通道数定义
#define PWM_CH_MAX_CNT 16
// PWM计算中浪费的ARISC时钟周期数(160/2)
#define PWM_WASTED_TICKS (160/2)

// PWM通道数据索引枚举
enum
{
    PWM_CH_POS,          // 通道位置
    PWM_CH_TICK,         // 时钟计数
    PWM_CH_TIMEOUT,      // 超时值
    PWM_CH_STATE,        // 通道状态
    PWM_CH_WATCHDOG,     // 看门狗设置

    PWM_CH_P_PORT,       // PWM输出端口
    PWM_CH_P_PIN_MSK,    // PWM引脚掩码
    PWM_CH_P_PIN_MSKN,   // PWM引脚掩码(取反)
    PWM_CH_P_INV,        // PWM信号是否反相
    PWM_CH_P_T0,         // PWM周期T0时间
    PWM_CH_P_T1,         // PWM周期T1时间
    PWM_CH_P_STOP,       // PWM停止标志
    PWM_CH_P_TICK,       // PWM时钟计数

    PWM_CH_D_PORT,       // 方向控制端口
    PWM_CH_D_PIN_MSK,    // 方向控制引脚掩码
    PWM_CH_D_PIN_MSKN,   // 方向控制引脚掩码(取反)
    PWM_CH_D,            // 方向值
    PWM_CH_D_INV,        // 方向信号是否反相
    PWM_CH_D_T0,         // 方向切换延时T0
    PWM_CH_D_T1,         // 方向切换延时T1
    PWM_CH_D_CHANGE,     // 方向改变标志

    PWM_CH_DATA_CNT      // PWM通道数据总数
};

// PWM模块数据索引枚举
enum
{
    PWM_TIMER_TICK,      // 定时器计数值
    PWM_ARM_LOCK,        // ARM锁定标志
    PWM_ARISC_LOCK,      // ARISC锁定标志
    PWM_CH_CNT,          // 通道计数
    PWM_DATA_CNT         // 数据总数
};

// PWM通道状态枚举
enum
{
    PWM_CH_STATE_IDLE,   // 空闲状态
    PWM_CH_STATE_P0,     // PWM周期P0状态
    PWM_CH_STATE_P1,     // PWM周期P1状态
    PWM_CH_STATE_D0,     // 方向切换D0状态
    PWM_CH_STATE_D1      // 方向切换D1状态
};

// PWM共享内存地址定义
#define PWM_SHM_BASE         (ARISC_SHM_BASE)                    // PWM共享内存基地址
#define PWM_SHM_DATA_BASE    (PWM_SHM_BASE)                      // PWM数据区基地址
#define PWM_SHM_CH_DATA_BASE (PWM_SHM_DATA_BASE + PWM_DATA_CNT*4)// PWM通道数据区基地址
#define PWM_SHM_SIZE         (PWM_SHM_CH_DATA_BASE + PWM_CH_MAX_CNT*PWM_CH_DATA_CNT*4) // PWM共享内存总大小

// 编码器模块使能标志
#define ENC_MODULE_ENABLED 1

// 如果编码器模块被使能
#if ENC_MODULE_ENABLED
// 定义最大编码器通道数
#define ENC_CH_MAX_CNT 8

// 编码器通道数据索引枚举
enum
{
    ENC_CH_BUSY,         // 通道忙状态
    ENC_CH_POS,          // 位置计数值
    ENC_CH_AB_STATE,     // A/B相状态

    ENC_CH_A_PORT,       // A相端口
    ENC_CH_A_PIN_MSK,    // A相引脚掩码
    ENC_CH_A_INV,        // A相信号是否反相
    ENC_CH_A_ALL,        // A相所有边沿触发
    ENC_CH_A_STATE,      // A相当前状态

    ENC_CH_B_USE,        // B相使用标志
    ENC_CH_B_PORT,       // B相端口
    ENC_CH_B_PIN_MSK,    // B相引脚掩码
    ENC_CH_B_STATE,      // B相当前状态

    ENC_CH_Z_USE,        // Z相使用标志
    ENC_CH_Z_PORT,       // Z相端口
    ENC_CH_Z_PIN_MSK,    // Z相引脚掩码
    ENC_CH_Z_INV,        // Z相信号是否反相
    ENC_CH_Z_ALL,        // Z相所有边沿触发
    ENC_CH_Z_STATE,      // Z相当前状态

    ENC_CH_PER_TICKS,    // 每周期的时钟计数
    ENC_CH_LAST_TICK,    // 上一次时钟计数

    ENC_CH_DATA_CNT      // 编码器通道数据总数
};

// 编码器模块数据索引枚举
enum
{
    ENC_ARM_LOCK,        // ARM锁定标志
    ENC_ARISC_LOCK,      // ARISC锁定标志
    ENC_CH_CNT,          // 通道计数
    ENC_DATA_CNT         // 数据总数
};

// 编码器共享内存地址定义
#define ENC_SHM_BASE         (PWM_SHM_BASE + PWM_SHM_SIZE)  // 编码器共享内存基地址
#define ENC_SHM_DATA_BASE    (ENC_SHM_BASE)                 // 编码器数据区基地址
#define ENC_SHM_CH_DATA_BASE (ENC_SHM_DATA_BASE + ENC_DATA_CNT*4)  // 编码器通道数据区基地址
#define ENC_SHM_SIZE         (ENC_SHM_CH_DATA_BASE + ENC_CH_MAX_CNT*ENC_CH_DATA_CNT*4 - ENC_SHM_BASE) // 编码器共享内存总大小
#endif

// 错误消息打印宏定义
#define PRINT_ERROR(MSG) \
    rtapi_print_msg(RTAPI_MSG_ERR, "%s: "MSG"\n", comp_name)

// 打印错误消息并返回的宏定义
#define PRINT_ERROR_AND_RETURN(MSG,RETVAL) \
    { PRINT_ERROR(MSG); return RETVAL; }

// 全局变量声明：虚拟地址指针
static uint32_t *_shm_vrt_addr, *_gpio_vrt_addr, *_r_gpio_vrt_addr;

// GPIO端口寄存器数组，用于访问所有GPIO端口
volatile _GPIO_PORT_REG_t *_GPIO[GPIO_PORTS_MAX_CNT] = {0};
// GPIO数据缓冲区
static uint32_t _gpio_buf[GPIO_PORTS_MAX_CNT] = {0};

// PWM通道数据数组，第一维是通道号，第二维是数据类型
volatile uint32_t * _pwmc[PWM_CH_MAX_CNT][PWM_CH_DATA_CNT] = {0};
// PWM模块数据数组
volatile uint32_t * _pwmd[PWM_DATA_CNT] = {0};

#if ENC_MODULE_ENABLED
// 编码器通道数据数组，第一维是通道号，第二维是数据类型
volatile uint32_t * _encc[ENC_CH_MAX_CNT][ENC_CH_DATA_CNT] = {0};
// 编码器模块数据数组
volatile uint32_t * _encd[ENC_DATA_CNT] = {0};
#endif

// PWM自旋锁获取内联函数
static inline
void _pwm_spin_lock()
{
    // 设置ARM锁定标志
    *_pwmd[PWM_ARM_LOCK] = 1;
    // 如果没有活动通道，直接返回
    if ( ! *_pwmd[PWM_CH_CNT] ) return;
    // 等待直到ARISC释放锁
    while ( *_pwmd[PWM_ARISC_LOCK] );
}

// PWM自旋锁释放内联函数
static inline
void _pwm_spin_unlock()
{
    // 清除ARM锁定标志
    *_pwmd[PWM_ARM_LOCK] = 0;
}

#if ENC_MODULE_ENABLED
// 编码器自旋锁获取内联函数
static inline
void _enc_spin_lock()
{
    // 设置ARM锁定标志
    *_encd[ENC_ARM_LOCK] = 1;
    // 如果没有活动通道，直接返回
    if ( ! *_encd[ENC_CH_CNT] ) return;
    // 等待直到ARISC释放锁
    while ( *_encd[ENC_ARISC_LOCK] );
}

// 编码器自旋锁释放内联函数
static inline
void _enc_spin_unlock()
{
    // 清除ARM锁定标志
    *_encd[ENC_ARM_LOCK] = 0;
}
#endif

// 设置GPIO引脚上下拉状态的内联函数
static inline
int32_t gpio_pin_pull_set(uint32_t port, uint32_t pin, uint32_t level, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
        // 检查上下拉级别是否有效
        if ( level >= GPIO_PULL_CNT ) return -3;
    }
    // 计算寄存器槽位和位置偏移
    uint32_t slot = pin/16, pos = pin%16*2;
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 清除原有上下拉设置
    _GPIO[port]->pull[slot] &= ~(0b11 << pos);
    // 设置新的上下拉值
    _GPIO[port]->pull[slot] |= (level << pos);
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取GPIO引脚上下拉状态的内联函数
static inline
uint32_t gpio_pin_pull_get(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
    }
    // 计算寄存器槽位和位置偏移
    uint32_t slot = pin/16, pos = pin%16*2;
    // 返回上下拉状态值
    return (_GPIO[port]->pull[slot] >> pos) & 0b11;
}

// 设置GPIO引脚驱动强度的内联函数
static inline
int32_t gpio_pin_multi_drive_set(uint32_t port, uint32_t pin, uint32_t level, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
        // 检查驱动强度级别是否有效
        if ( level >= GPIO_MULTI_DRIVE_LEVEL_CNT ) return -3;
    }
    // 计算寄存器槽位和位置偏移
    uint32_t slot = pin/16, pos = pin%16*2;
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 清除原有驱动强度设置
    _GPIO[port]->drive[slot] &= ~(0b11 << pos);
    // 设置新的驱动强度值
    _GPIO[port]->drive[slot] |= (level << pos);
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取GPIO引脚驱动强度的内联函数
static inline
uint32_t gpio_pin_multi_drive_get(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
    }
    // 计算寄存器槽位和位置偏移
    uint32_t slot = pin/16, pos = pin%16*2;
    // 返回驱动强度值
    return (_GPIO[port]->drive[slot] >> pos) & 0b11;
}

// 设置GPIO引脚功能的内联函数
static inline
int32_t gpio_pin_func_set(uint32_t port, uint32_t pin, uint32_t func, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
        // 检查功能选择是否有效
        if ( func >= GPIO_FUNC_CNT ) return -3;
    }
    // 计算寄存器槽位和位置偏移（每个引脚占用4位配置）
    uint32_t slot = pin/8, pos = pin%8*4;
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 清除原有功能配置
    _GPIO[port]->config[slot] &= ~(0b0111 << pos);
    // 设置新的功能配置
    _GPIO[port]->config[slot] |=    (func << pos);
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取GPIO引脚功能的内联函数
static inline
uint32_t gpio_pin_func_get(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效
        if ( port >= GPIO_PORTS_MAX_CNT ) return -1;
        // 检查引脚号是否有效
        if ( pin >= GPIO_PINS_MAX_CNT ) return -2;
    }
    // 计算寄存器槽位和位置偏移
    uint32_t slot = pin/8, pos = pin%8*4;
    // 返回功能配置值
    return (_GPIO[port]->config[slot] >> pos) & 0b0111;
}

// 获取GPIO引脚输入状态的内联函数
static inline
uint32_t gpio_pin_get(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
        // 检查引脚号是否有效，无效返回0
        if ( pin >= GPIO_PINS_MAX_CNT ) return 0;
    }
    // 读取数据寄存器并返回对应引脚的状态(HIGH或LOW)
    return _GPIO[port]->data & (1UL << pin) ? HIGH : LOW;
}

// 设置GPIO引脚输出高电平的内联函数
static inline
int32_t gpio_pin_set(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
        // 检查引脚号是否有效，无效返回0
        if ( pin >= GPIO_PINS_MAX_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 设置对应位为1，输出高电平
    _GPIO[port]->data |= (1UL << pin);
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 设置GPIO引脚输出低电平的内联函数
static inline
int32_t gpio_pin_clr(uint32_t port, uint32_t pin, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
        // 检查引脚号是否有效，无效返回0
        if ( pin >= GPIO_PINS_MAX_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 清除对应位，输出低电平
    _GPIO[port]->data &= ~(1UL << pin);
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取指定GPIO端口的所有引脚状态的内联函数
static inline
uint32_t gpio_port_get(uint32_t port, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
    }
    // 返回整个端口的数据寄存器值
    return _GPIO[port]->data;
}

// 设置指定GPIO端口多个引脚为高电平的内联函数
static inline
int32_t gpio_port_set(uint32_t port, uint32_t mask, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe )
    {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 将mask指定的位设置为1
    _GPIO[port]->data |= mask;
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 设置指定GPIO端口多个引脚为低电平的内联函数
static inline
int32_t gpio_port_clr(uint32_t port, uint32_t mask, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe )
    {
        // 检查端口号是否有效，无效返回0
        if ( port >= GPIO_PORTS_MAX_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 将mask指定的位清零
    _GPIO[port]->data &= ~mask;
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取所有GPIO端口状态的内联函数
static inline
uint32_t* gpio_all_get(uint32_t safe)
{
    uint32_t port;
    // 遍历所有端口，保存其状态到缓冲区
    for ( port = GPIO_PORTS_MAX_CNT; port--; ) _gpio_buf[port] = _GPIO[port]->data;
    // 返回缓冲区指针
    return (uint32_t*) &_gpio_buf[0];
}

// 设置所有GPIO端口指定引脚为高电平的内联函数
static inline
int32_t gpio_all_set(uint32_t* mask, uint32_t safe)
{
    uint32_t port;
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 遍历所有端口，将mask指定的位设置为1
    for ( port = GPIO_PORTS_MAX_CNT; port--; ) _GPIO[port]->data |= mask[port];
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 设置所有GPIO端口指定引脚为低电平的内联函数
static inline
int32_t gpio_all_clr(uint32_t* mask, uint32_t safe)
{
    uint32_t port;
    // 获取PWM锁
    _pwm_spin_lock();
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，获取编码器锁
    _enc_spin_lock();
#endif
    // 遍历所有端口，将mask指定的位清零
    for ( port = GPIO_PORTS_MAX_CNT; port--; )
    {
        _GPIO[port]->data &= ~mask[port];
    }
#if ENC_MODULE_ENABLED
    // 释放编码器锁
    _enc_spin_unlock();
#endif
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 清理PWM配置的内联函数
static inline
int32_t pwm_cleanup(uint32_t safe)
{
    uint32_t c, d;

    // 如果启用安全检查，获取PWM锁
    if ( safe ) _pwm_spin_lock();

    // 遍历所有PWM通道
    for ( c = PWM_CH_MAX_CNT; c--; ) {
        // 重置已配置的引脚状态
        if ( *_pwmc[c][PWM_CH_P_PORT] < GPIO_PORTS_MAX_CNT && *_pwmc[c][PWM_CH_P_PIN_MSK] ) {
            // 根据反相设置处理PWM引脚
            if ( *_pwmc[c][PWM_CH_P_INV] ) {
                _GPIO[ *_pwmc[c][PWM_CH_P_PORT] ]->data |= *_pwmc[c][PWM_CH_P_PIN_MSK];
            } else {
                _GPIO[ *_pwmc[c][PWM_CH_P_PORT] ]->data &= *_pwmc[c][PWM_CH_P_PIN_MSKN];
            }
        }
        // 处理方向控制引脚
        if ( *_pwmc[c][PWM_CH_D_PORT] < GPIO_PORTS_MAX_CNT && *_pwmc[c][PWM_CH_D_PIN_MSK] ) {
            // 根据反相设置处理方向引脚
            if ( *_pwmc[c][PWM_CH_D_INV] ) {
                _GPIO[ *_pwmc[c][PWM_CH_D_PORT] ]->data |= *_pwmc[c][PWM_CH_D_PIN_MSK];
            } else {
                _GPIO[ *_pwmc[c][PWM_CH_D_PORT] ]->data &= *_pwmc[c][PWM_CH_D_PIN_MSKN];
            }
        }
        // 重置通道的所有数据
        for ( d = PWM_CH_DATA_CNT; d--; ) *_pwmc[c][d] = 0;
    }

    // 重置模块的所有数据
    for ( d = PWM_DATA_CNT; d--; ) *_pwmd[d] = 0;

    // 如果启用安全检查，释放PWM锁
    if ( safe ) _pwm_spin_unlock();

    return 0;
}

// 设置PWM模块数据的内联函数
static inline
int32_t pwm_data_set(uint32_t name, uint32_t value, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查数据名称是否有效
        if ( name >= PWM_DATA_CNT ) return -1;
        // 如果是设置通道数，检查通道数是否超出最大值
        if ( name == PWM_CH_CNT && value >= PWM_CH_MAX_CNT ) return -2;
    }
    // 获取PWM锁
    _pwm_spin_lock();
    // 设置指定数据
    *_pwmd[name] = value;
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取PWM模块数据的内联函数
static inline
uint32_t pwm_data_get(uint32_t name, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查数据名称是否有效，无效返回0
        if ( name >= PWM_DATA_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
    // 读取指定数据
    uint32_t value = *_pwmd[name];
    // 释放PWM锁
    _pwm_spin_unlock();
    return value;
}

// 设置PWM通道数据的内联函数
static inline
int32_t pwm_ch_data_set(uint32_t c, uint32_t name, uint32_t value, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= PWM_CH_MAX_CNT ) return -1;
        // 检查数据名称是否有效
        if ( name >= PWM_CH_DATA_CNT ) return -2;
    }
    // 获取PWM锁
    _pwm_spin_lock();
    // 设置指定通道的指定数据
    *_pwmc[c][name] = value;
    // 释放PWM锁
    _pwm_spin_unlock();
    return 0;
}

// 获取PWM通道数据的内联函数
static inline
uint32_t pwm_ch_data_get(uint32_t c, uint32_t name, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效，无效返回0
        if ( c >= PWM_CH_MAX_CNT ) return 0;
        // 检查数据名称是否有效，无效返回0
        if ( name >= PWM_CH_DATA_CNT ) return 0;
    }
    // 获取PWM锁
    _pwm_spin_lock();
    // 读取指定通道的指定数据
    uint32_t value = *_pwmc[c][name];
    // 释放PWM锁
    _pwm_spin_unlock();
    return value;
}

// 设置PWM通道引脚配置的内联函数
static inline
int32_t pwm_ch_pins_setup (
    uint32_t c,                  // 通道号
    uint32_t p_port,            // PWM信号端口
    uint32_t p_pin,             // PWM信号引脚
    uint32_t p_inv,             // PWM信号是否反相
    uint32_t d_port,            // 方向控制端口
    uint32_t d_pin,             // 方向控制引脚
    uint32_t d_inv,             // 方向信号是否反相
    uint32_t safe               // 是否启用安全检查
) {
    // 如果启用安全检查
    if ( safe ) {
        // 检查参数有效性
        if ( c >= PWM_CH_MAX_CNT ) return -1;
        if ( p_port >= GPIO_PORTS_MAX_CNT ) return -1;
        if ( p_pin >= GPIO_PINS_MAX_CNT ) return -1;
        if ( d_port >= GPIO_PORTS_MAX_CNT ) return -1;
        if ( d_pin >= GPIO_PINS_MAX_CNT ) return -1;
    }

    // 配置PWM信号引脚
    gpio_pin_func_set(p_port, p_pin, GPIO_FUNC_OUT, safe);    // 设置为输出模式
    gpio_pin_pull_set(p_port, p_pin, GPIO_PULL_DISABLE, safe);// 禁用上下拉
    if ( p_inv ) gpio_pin_set(p_port, p_pin, safe);           // 如果反相，设置为高电平
    else         gpio_pin_clr(p_port, p_pin, safe);           // 否则设置为低电平

    // 配置方向控制引脚
    gpio_pin_func_set(d_port, d_pin, GPIO_FUNC_OUT, safe);    // 设置为输出模式
    gpio_pin_pull_set(d_port, d_pin, GPIO_PULL_DISABLE, safe);// 禁用上下拉
    if ( d_inv ) gpio_pin_set(d_port, d_pin, safe);           // 如果反相，设置为高电平
    else         gpio_pin_clr(d_port, d_pin, safe);           // 否则设置为低电平

    // 获取PWM锁
    _pwm_spin_lock();

    // 保存PWM信号引脚配置
    *_pwmc[c][PWM_CH_P_PORT] = p_port;                         // 保存端口号
    *_pwmc[c][PWM_CH_P_PIN_MSK] = 1UL << p_pin;               // 保存引脚掩码
    *_pwmc[c][PWM_CH_P_PIN_MSKN] = ~(1UL << p_pin);           // 保存引脚掩码的反码
    *_pwmc[c][PWM_CH_P_INV] = p_inv;                          // 保存反相设置

    // 保存方向控制引脚配置
    *_pwmc[c][PWM_CH_D_PORT] = d_port;                         // 保存端口号
    *_pwmc[c][PWM_CH_D_PIN_MSK] = 1UL << d_pin;               // 保存引脚掩码
    *_pwmc[c][PWM_CH_D_PIN_MSKN] = ~(1UL << d_pin);           // 保存引脚掩码的反码
    *_pwmc[c][PWM_CH_D_INV] = d_inv;                          // 保存反相设置
    *_pwmc[c][PWM_CH_D] = 0;                                   // 初始化方向值为0

    // 释放PWM锁
    _pwm_spin_unlock();

    return 0;
}

// 设置PWM通道定时参数的内联函数
static inline
int32_t pwm_ch_times_setup (
    uint32_t c,                  // 通道号
    int32_t p_freq_mHz,         // PWM频率(毫赫兹)
    int32_t p_duty_s32,         // 占空比(32位有符号整数格式)
    uint32_t p_duty_max_time_ns, // 最大占空时间(纳秒)
    uint32_t d_hold_ns,         // 方向信号保持时间(纳秒)
    uint32_t d_setup_ns,        // 方向信号建立时间(纳秒)
    uint32_t safe               // 是否启用安全检查
) {
    uint32_t p_t0, p_t1, p_t1_max, p_period, d_t0, d_t1;
    int32_t d;

    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= PWM_CH_MAX_CNT ) return -1;

        // 如果未指定方向信号时间，设置默认值
        if ( !d_hold_ns ) d_hold_ns = 50000;
        if ( !d_setup_ns ) d_setup_ns = 50000;

        // 检查通道引脚是否正确配置
        if (  *_pwmc[c][PWM_CH_P_PORT] >= GPIO_PORTS_MAX_CNT ||
              *_pwmc[c][PWM_CH_D_PORT] >= GPIO_PORTS_MAX_CNT ||
            !(*_pwmc[c][PWM_CH_P_PIN_MSK]) ||
            !(*_pwmc[c][PWM_CH_D_PIN_MSK])
        ) {
            // 如果引脚配置无效，禁用通道
            _pwm_spin_lock();
            *_pwmc[c][PWM_CH_STATE] = PWM_CH_STATE_IDLE;
            _pwm_spin_unlock();
            return 0;
        }
    }

    // 如果频率或占空比为0，停止通道
    if ( !p_freq_mHz || !p_duty_s32 ) {
        _pwm_spin_lock();
        if ( *_pwmc[c][PWM_CH_STATE] ) *_pwmc[c][PWM_CH_P_STOP] = 1;
        _pwm_spin_unlock();
        return 0;
    }

    // 计算方向
    d = (p_freq_mHz < 0 ? -1 : 1) * (p_duty_s32 < 0 ? -1 : 1);

    // 取频率和占空比的绝对值进行计算
    p_duty_s32 = p_duty_s32 < 0 ? -p_duty_s32 : p_duty_s32;
    p_freq_mHz = p_freq_mHz < 0 ? -p_freq_mHz : p_freq_mHz;

    // 计算PWM周期(考虑浪费的时钟周期)
    p_period = (uint32_t) ( ((uint64_t)ARISC_CPU_FREQ) * ((uint64_t)1000) / ((uint64_t)p_freq_mHz) );
    p_period = p_period < (2*PWM_WASTED_TICKS) ? 0 : p_period - (2*PWM_WASTED_TICKS);

    // 根据占空比计算高电平时间
    p_t1 = (uint32_t) ( ((uint64_t)p_period) * ((uint64_t)p_duty_s32) / ((uint64_t)INT32_MAX) );

    // 如果指定了最大占空时间，限制高电平时间
    if ( p_duty_max_time_ns ) {
        p_t1_max = ((uint64_t)ARISC_CPU_FREQ) * ((uint64_t)p_duty_max_time_ns) / ((uint64_t)1000000000);
        if ( p_t1 > p_t1_max ) p_t1 = p_t1_max;
    }

    // 计算低电平时间
    p_t0 = p_period - p_t1;

    // 计算方向信号的时序参数
    d_t0 = ARISC_CPU_FREQ / (1000000000 / d_hold_ns);
    d_t0 = d_t0 < PWM_WASTED_TICKS ? 0 : d_t0 - PWM_WASTED_TICKS;
    d_t1 = ARISC_CPU_FREQ / (1000000000 / d_setup_ns);
    d_t1 = d_t1 < PWM_WASTED_TICKS ? 0 : d_t1 - PWM_WASTED_TICKS;

    // 获取PWM锁
    _pwm_spin_lock();

    // 确定是否需要改变方向
    *_pwmc[c][PWM_CH_D_CHANGE] = (d > 0 &&  (*_pwmc[c][PWM_CH_D])) ||
                                 (d < 0 && !(*_pwmc[c][PWM_CH_D])) ? 1 : 0;

    // 根据当前通道状态更新超时值
    switch ( *_pwmc[c][PWM_CH_STATE] ) {
        case PWM_CH_STATE_IDLE: { *_pwmc[c][PWM_CH_STATE] = PWM_CH_STATE_P0; break; }  // 空闲状态转为P0状态
        case PWM_CH_STATE_P0: { *_pwmc[c][PWM_CH_TIMEOUT] = p_t0; break; }             // P0状态设置低电平时间
        case PWM_CH_STATE_P1: { *_pwmc[c][PWM_CH_TIMEOUT] = p_t1; break; }             // P1状态设置高电平时间
        case PWM_CH_STATE_D0: { *_pwmc[c][PWM_CH_TIMEOUT] = d_t0; break; }             // D0状态设置方向保持时间
        case PWM_CH_STATE_D1: { *_pwmc[c][PWM_CH_TIMEOUT] = d_t1; break; }             // D1状态设置方向建立时间
    }

    // 更新PWM和方向控制的时序参数
    *_pwmc[c][PWM_CH_P_T0] = p_t0;     // 保存低电平时间
    *_pwmc[c][PWM_CH_P_T1] = p_t1;     // 保存高电平时间
    *_pwmc[c][PWM_CH_D_T0] = d_t0;     // 保存方向保持时间
    *_pwmc[c][PWM_CH_D_T1] = d_t1;     // 保存方向建立时间

    // 释放PWM锁
    _pwm_spin_unlock();

    return 0;
}

// 获取PWM通道位置的内联函数(以毫脉冲为单位)
static inline
int64_t pwm_ch_pos_get(uint32_t c, uint32_t safe)
{
    int64_t pos = 0, a = 0;
    int32_t pos32;
    uint32_t tc, tt;

    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= PWM_CH_MAX_CNT ) return 0;
    }

    // 获取PWM锁
    _pwm_spin_lock();

    // 获取位置计数值并转换为64位
    pos32 = (int32_t) *_pwmc[c][PWM_CH_POS];
    pos = (int64_t) pos32;
    pos *= 1000;  // 转换为毫脉冲单位

    // 计算总时间(周期)
    tt = *_pwmc[c][PWM_CH_P_T0] + *_pwmc[c][PWM_CH_P_T1];

    // 如果在PWM周期内(P0或P1状态)，计算插值
    if ( tt &&
         ( *_pwmc[c][PWM_CH_STATE] == PWM_CH_STATE_P0 ||
           *_pwmc[c][PWM_CH_STATE] == PWM_CH_STATE_P1 )
    ) {
        // 计算当前时刻
        tc = *_pwmd[PWM_TIMER_TICK] - *_pwmc[c][PWM_CH_P_TICK];
        if ( tc > tt ) tc = tt;
        // 计算插值
        a = 1000 * tc / tt;
        a = *_pwmc[c][PWM_CH_D] ? 1000 - a : a - 1000;
    }

    // 释放PWM锁
    _pwm_spin_unlock();

    // 返回位置值(包含插值)
    return pos + a;
}




#if ENC_MODULE_ENABLED    // 如果编码器模块被启用

// 清理编码器配置的内联函数
static inline
int32_t enc_cleanup(uint32_t safe)
{
    uint32_t c, d;
    // 如果启用安全检查，获取编码器锁
    if ( safe ) _enc_spin_lock();
    // 遍历所有编码器通道
    for ( c = ENC_CH_MAX_CNT; c--; ) {
        // 清除通道的所有数据
        for ( d = ENC_CH_DATA_CNT; d--; ) *_encc[c][d] = 0;
    }
    // 清除模块的所有数据
    for ( d = ENC_DATA_CNT; d--; ) *_encd[d] = 0;
    // 如果启用安全检查，释放编码器锁
    if ( safe ) _enc_spin_unlock();
    return 0;
}

// 设置编码器模块数据的内联函数
static inline
int32_t enc_data_set(uint32_t name, uint32_t value, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查数据名称是否有效
        if ( name >= ENC_DATA_CNT ) return -1;
        // 如果是设置通道数，检查通道数是否超出最大值
        if ( name == ENC_CH_CNT && value >= ENC_CH_MAX_CNT ) return -2;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 设置指定数据
    *_encd[name] = value;
    // 释放编码器锁
    _enc_spin_unlock();
    return 0;
}

// 获取编码器模块数据的内联函数
static inline
uint32_t enc_data_get(uint32_t name, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查数据名称是否有效
        if ( name >= ENC_DATA_CNT ) return 0;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 读取指定数据
    uint32_t value = *_encd[name];
    // 释放编码器锁
    _enc_spin_unlock();
    return value;
}

// 设置编码器通道数据的内联函数
static inline
int32_t enc_ch_data_set(uint32_t c, uint32_t name, uint32_t value, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= ENC_CH_MAX_CNT ) return -1;
        // 检查数据名称是否有效
        if ( name >= ENC_CH_DATA_CNT ) return -2;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 设置指定通道的指定数据（如果是位置数据，需要类型转换为int32_t）
    *_encc[c][name] = (name == ENC_CH_POS) ? (int32_t)value : value;
    // 释放编码器锁
    _enc_spin_unlock();
    return 0;
}

// 获取编码器通道数据的内联函数
static inline
uint32_t enc_ch_data_get(uint32_t c, uint32_t name, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= ENC_CH_MAX_CNT ) return 0;
        // 检查数据名称是否有效
        if ( name >= ENC_CH_DATA_CNT ) return 0;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 读取指定通道的指定数据
    uint32_t value = *_encc[c][name];
    // 释放编码器锁
    _enc_spin_unlock();
    return value;
}

// 设置编码器通道引脚配置的内联函数
static inline
int32_t enc_ch_pins_setup(
    uint32_t c,          // 通道号
    uint32_t a_port,     // A相端口号
    uint32_t a_pin,      // A相引脚号
    uint32_t a_inv,      // A相是否反相
    uint32_t a_all,      // A相是否使用所有边沿
    uint32_t b_port,     // B相端口号
    uint32_t b_pin,      // B相引脚号
    uint32_t z_port,     // Z相端口号
    uint32_t z_pin,      // Z相引脚号
    uint32_t z_inv,      // Z相是否反相
    uint32_t z_all,      // Z相是否使用所有边沿
    uint32_t safe        // 是否启用安全检查
) {
    uint32_t b_use = 0, z_use = 0;  // B相和Z相使用标志

    // 如果启用安全检查
    if ( safe )
    {
        // 检查通道号是否有效
        if ( c >= ENC_CH_MAX_CNT ) return -1;
        // 检查A相配置是否有效
        if ( a_port >= GPIO_PORTS_MAX_CNT ) return -1;
        if ( a_pin >= GPIO_PINS_MAX_CNT ) return -1;
    }

    // 检查B相配置是否有效
    if ( b_port < GPIO_PORTS_MAX_CNT && b_pin < GPIO_PINS_MAX_CNT ) b_use = 1;
    // 检查Z相配置是否有效
    if ( z_port < GPIO_PORTS_MAX_CNT && z_pin < GPIO_PINS_MAX_CNT ) z_use = 1;

    // 配置A相引脚
    gpio_pin_func_set(a_port, a_pin, GPIO_FUNC_IN, safe);      // 设置为输入模式
    gpio_pin_pull_set(a_port, a_pin, GPIO_PULL_DISABLE, safe); // 禁用上下拉

    // 如果使用B相，配置B相引脚
    if ( b_use ) {
        gpio_pin_func_set(b_port, b_pin, GPIO_FUNC_IN, safe);      // 设置为输入模式
        gpio_pin_pull_set(b_port, b_pin, GPIO_PULL_DISABLE, safe); // 禁用上下拉
    }

    // 如果使用Z相，配置Z相引脚
    if ( z_use ) {
        gpio_pin_func_set(z_port, z_pin, GPIO_FUNC_IN, safe);      // 设置为输入模式
        gpio_pin_pull_set(z_port, z_pin, GPIO_PULL_DISABLE, safe); // 禁用上下拉
    }

    // 获取编码器锁
    _enc_spin_lock();

    // 保存A相配置
    *_encc[c][ENC_CH_A_PORT] = a_port;
    *_encc[c][ENC_CH_A_PIN_MSK] = 1UL << a_pin;
    *_encc[c][ENC_CH_A_INV] = a_inv;
    *_encc[c][ENC_CH_A_ALL] = a_all;

    // 保存B相配置
    *_encc[c][ENC_CH_B_USE] = b_use;
    if ( b_use ) {
        *_encc[c][ENC_CH_B_PORT] = b_port;
        *_encc[c][ENC_CH_B_PIN_MSK] = 1UL << b_pin;
    }

    // 保存Z相配置
    *_encc[c][ENC_CH_Z_USE] = z_use;
    if ( z_use ) {
        *_encc[c][ENC_CH_Z_PORT] = z_port;
        *_encc[c][ENC_CH_Z_PIN_MSK] = 1UL << z_pin;
        *_encc[c][ENC_CH_Z_INV] = z_inv;
        *_encc[c][ENC_CH_Z_ALL] = z_all;
    }

    // 释放编码器锁
    _enc_spin_unlock();

    return 0;
}

// 获取编码器通道位置和周期计数的内联函数
static inline
int32_t enc_ch_pos_get(uint32_t c, volatile int32_t *pos, volatile uint32_t *per_ticks, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= ENC_CH_MAX_CNT ) return -1;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 读取位置计数值
    *pos = (int32_t) *_encc[c][ENC_CH_POS];
    // 读取周期计数值
    *per_ticks = (uint32_t) * _encc[c][ENC_CH_PER_TICKS];
    // 释放编码器锁
    _enc_spin_unlock();
    return 0;
}

// 设置编码器通道位置和周期计数的内联函数
static inline
int32_t enc_ch_pos_set(uint32_t c, int32_t pos, uint32_t per_ticks, uint32_t safe)
{
    // 如果启用安全检查
    if ( safe ) {
        // 检查通道号是否有效
        if ( c >= ENC_CH_MAX_CNT ) return -1;
    }
    // 获取编码器锁
    _enc_spin_lock();
    // 设置位置计数值
    *_encc[c][ENC_CH_POS] = (uint32_t) pos;
    // 设置周期计数值
    *_encc[c][ENC_CH_PER_TICKS] = (uint32_t) per_ticks;
    // 释放编码器锁
    _enc_spin_unlock();
    return 0;
}

#endif  // ENC_MODULE_ENABLED

// 初始化共享内存的内联函数
static inline
int32_t shmem_init(const char *comp_name)
{
    int32_t mem_fd;
    uint32_t addr, off, port, ch, name, *p;

    // 打开物理内存文件
    // 临时切换到root权限
    if (seteuid(0) != 0) {
        PRINT_ERROR_AND_RETURN("ERROR: seteuid failed\n",-1);
    }
    setfsuid( geteuid() );
    mem_fd = open("/dev/mem", O_RDWR|O_SYNC);  // 以读写和同步方式打开
    if ( mem_fd  < 0 ) PRINT_ERROR_AND_RETURN("ERROR: can't open /dev/mem file\n",-1);
    setfsuid( getuid() );  // 恢复原来的用户权限

    // 映射共享内存区域
    addr = ARISC_SHM_BASE & ~(ARISC_SHM_SIZE - 1);  // 对齐到页边界
    off = ARISC_SHM_BASE & (ARISC_SHM_SIZE - 1);    // 计算页内偏移
    _shm_vrt_addr = mmap(NULL, ARISC_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, addr);
    if (_shm_vrt_addr == MAP_FAILED) PRINT_ERROR_AND_RETURN("ERROR: shm mmap() failed\n",-1);

    // 初始化共享内存指针
    p = _shm_vrt_addr + off/4;
    // 初始化PWM模块数据指针
    for ( name = 0; name < PWM_DATA_CNT; name++, p++ ) _pwmd[name] = p;
    // 初始化PWM通道数据指针
    for ( ch = 0; ch < PWM_CH_MAX_CNT; ch++ ) {
        for ( name = 0; name < PWM_CH_DATA_CNT; name++, p++ ) _pwmc[ch][name] = p;
    }
#if ENC_MODULE_ENABLED
    // 初始化编码器模块数据指针
    for ( name = 0; name < ENC_DATA_CNT; name++, p++ ) _encd[name] = p;
    // 初始化编码器通道数据指针
    for ( ch = 0; ch < ENC_CH_MAX_CNT; ch++ ) {
        for ( name = 0; name < ENC_CH_DATA_CNT; name++, p++ ) _encc[ch][name] = p;
    }
#endif

    // 映射GPIO寄存器区域
    addr = GPIO_BASE & ~(4096 - 1);         // 对齐到页边界
    off = GPIO_BASE & (4096 - 1);           // 计算页内偏移
    // 映射GPIO寄存器
    _gpio_vrt_addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, addr);
    if (_gpio_vrt_addr == MAP_FAILED) PRINT_ERROR_AND_RETURN("ERROR: gpio mmap() failed\n",-3);
    // 初始化GPIO端口寄存器指针(PA-PG)
    for ( port = PA; port <= PG; ++port )
    {
        _GPIO[port] = (_GPIO_PORT_REG_t *)(_gpio_vrt_addr + (off + port*0x24)/4);
    }

    // 映射R_GPIO(PL端口)寄存器区域
    addr = GPIO_R_BASE & ~(4096 - 1);       // 对齐到页边界
    off = GPIO_R_BASE & (4096 - 1);         // 计算页内偏移
    // 映射R_GPIO寄存器
    _r_gpio_vrt_addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, addr);
    if (_r_gpio_vrt_addr == MAP_FAILED) PRINT_ERROR_AND_RETURN("ERROR: r_gpio mmap() failed\n",-4);
    // 初始化PL端口寄存器指针
    _GPIO[PL] = (_GPIO_PORT_REG_t *)(_r_gpio_vrt_addr + off/4);

    // 关闭物理内存文件(映射完成后不再需要)
    close(mem_fd);

    return 0;
}

// 取消共享内存映射的函数
static inline
void shmem_deinit(void)
{
    // 取消共享内存区域的映射
    munmap(_shm_vrt_addr, ARISC_SHM_SIZE);
    // 取消GPIO寄存器区域的映射
    munmap(_gpio_vrt_addr, 4096);
    // 取消R_GPIO寄存器区域的映射
    munmap(_r_gpio_vrt_addr, 4096);
}

// 结束头文件保护
#endif

