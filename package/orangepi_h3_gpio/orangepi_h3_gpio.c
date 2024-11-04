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

#include "orangepi_h3_gpio.h"

// 在RTAPI环境下定义模块信息
#ifdef RTAPI
MODULE_AUTHOR("orangepi_h3_gpio");
MODULE_DESCRIPTION("Orangepi H3 GPIO for ARMCNC");
MODULE_LICENSE("GPL");                             // 模块许可证
#endif

// 组件相关全局变量
static int32_t comp_id;                     // 组件ID
static const uint8_t * comp_name = "orangepi_h3_gpio"; // 组件名称

// 输入引脚配置字符串，默认为空
static int8_t *in = "";
#ifdef RTAPI
// 定义模块参数：输入引脚列表（逗号分隔）
RTAPI_MP_STRING(in, "input pins, comma separated");
#endif

// 输出引脚配置字符串，默认为空
static int8_t *out = "";
#ifdef RTAPI
// 定义模块参数：输出引脚列表（逗号分隔）
RTAPI_MP_STRING(out, "output pins, comma separated");
#endif

// PWM配置字符串，默认为空
static char *pwm = "";
#ifdef RTAPI
// 定义模块参数：通道控制类型列表（逗号分隔）
RTAPI_MP_STRING(pwm, "channels control type, comma separated");
#endif

// 如果编码器模块启用
#if ENC_MODULE_ENABLED
// 编码器通道数配置字符串，默认为"0"
static char *encoders = "0";
#ifdef RTAPI
// 定义模块参数：编码器通道数
RTAPI_MP_STRING(encoders, "number of encoder channels");
#endif
#endif

// GPIO端口名称数组
static const char *gpio_name[GPIO_PORTS_MAX_CNT] = {"PA","PB","PC","PD","PE","PF","PG","PL"};

// GPIO HAL引脚相关数组声明
// 正常逻辑电平的HAL引脚数组(每个端口的所有引脚)
static hal_bit_t **gpio_hal_0[GPIO_PORTS_MAX_CNT];
// 反相逻辑电平的HAL引脚数组(每个端口的所有引脚)
static hal_bit_t **gpio_hal_1[GPIO_PORTS_MAX_CNT];
// 正常逻辑电平的上一次状态缓存
static hal_bit_t gpio_hal_0_prev[GPIO_PORTS_MAX_CNT][GPIO_PINS_MAX_CNT];
// 反相逻辑电平的上一次状态缓存
static hal_bit_t gpio_hal_1_prev[GPIO_PORTS_MAX_CNT][GPIO_PINS_MAX_CNT];

// GPIO上下拉配置相关数组
// 上下拉配置的HAL引脚数组
static hal_s32_t **gpio_hal_pull[GPIO_PORTS_MAX_CNT];
// 上下拉配置的上一次状态缓存
static hal_s32_t gpio_hal_pull_prev[GPIO_PORTS_MAX_CNT][GPIO_PINS_MAX_CNT];

// GPIO驱动强度配置相关数组
// 驱动强度配置的HAL引脚数组
static hal_u32_t **gpio_hal_drive[GPIO_PORTS_MAX_CNT];
// 驱动强度配置的上一次状态缓存
static hal_u32_t gpio_hal_drive_prev[GPIO_PORTS_MAX_CNT][GPIO_PINS_MAX_CNT];

// GPIO端口掩码
static uint32_t gpio_out_mask[GPIO_PORTS_MAX_CNT] = {0};  // 输出引脚掩码
static uint32_t gpio_in_mask[GPIO_PORTS_MAX_CNT] = {0};   // 输入引脚掩码

// GPIO统计计数器
static uint32_t gpio_in_cnt = 0;     // 输入引脚总数
static uint32_t gpio_out_cnt = 0;    // 输出引脚总数
static uint32_t gpio_ports_cnt = 0;  // 使用的端口总数
static uint32_t gpio_pins_cnt[GPIO_PINS_MAX_CNT] = {0};  // 每个端口使用的引脚数

// 引脚掩码数组，用于快速访问单个引脚
static uint32_t pin_msk[GPIO_PINS_MAX_CNT] = {0};

// PWM通道共享内存数据结构定义
typedef struct
{
    hal_bit_t *enable;      // 使能控制(输入)

    hal_u32_t *pwm_port;    // PWM端口配置(输入)
    hal_u32_t *pwm_pin;     // PWM引脚配置(输入)
    hal_bit_t *pwm_inv;     // PWM信号反相设置(输入)

    hal_u32_t *dir_port;    // 方向控制端口配置(输入)
    hal_u32_t *dir_pin;     // 方向控制引脚配置(输入)
    hal_bit_t *dir_inv;     // 方向信号反相设置(输入)
    hal_u32_t *dir_hold;    // 方向保持时间(输入输出)
    hal_u32_t *dir_setup;   // 方向建立时间(输入输出)

    hal_float_t *dc_cmd;    // 占空比命令(输入)
    hal_float_t *dc_scale;  // 占空比比例因子(输入输出)
    hal_float_t *dc_min;    // 最小占空比(输入输出)
    hal_float_t *dc_max;    // 最大占空比(输入输出)
    hal_u32_t *dc_max_t;    // 最大占空时间(输入输出)
    hal_float_t *dc_offset; // 占空比偏移(输入输出)

    hal_float_t *pos_cmd;   // 位置命令(输入)
    hal_float_t *pos_scale; // 位置比例因子(输入输出)

    hal_float_t *vel_cmd;   // 速度命令(输入)
    hal_float_t *vel_scale; // 速度比例因子(输入输出)

    hal_float_t *freq_cmd;  // 频率命令(输入输出)
    hal_float_t *freq_min;  // 最小频率(输入输出)
    hal_float_t *freq_max;  // 最大频率(输入输出)

    hal_float_t *dc_fb;     // 占空比反馈(输出)
    hal_float_t *pos_fb;    // 位置反馈(输出)
    hal_float_t *vel_fb;    // 速度反馈(输出)
    hal_float_t *freq_fb;   // 频率反馈(输出)
    hal_s32_t *counts;      // 计数值(输出)
}
pwm_ch_shmem_t;

// PWM通道私有数据结构定义
typedef struct
{
    hal_bit_t enable;       // 使能状态

    hal_u32_t pwm_port;     // PWM端口号
    hal_u32_t pwm_pin;      // PWM引脚号
    hal_bit_t pwm_inv;      // PWM信号是否反相

    hal_u32_t dir_port;     // 方向控制端口号
    hal_u32_t dir_pin;      // 方向控制引脚号
    hal_bit_t dir_inv;      // 方向信号是否反相
    hal_u32_t dir_hold;     // 方向保持时间
    hal_u32_t dir_setup;    // 方向建立时间

    hal_float_t dc_cmd;     // 占空比命令
    hal_float_t dc_scale;   // 占空比比例因子
    hal_float_t dc_min;     // 最小占空比
    hal_float_t dc_max;     // 最大占空比
    hal_u32_t *dc_max_t;    // 最大占空时间
    hal_float_t dc_offset;  // 占空比偏移

    hal_float_t pos_cmd;    // 位置命令
    hal_float_t pos_scale;  // 位置比例因子

    hal_float_t vel_cmd;    // 速度命令
    hal_float_t vel_scale;  // 速度比例因子

    hal_float_t freq_cmd;   // 频率命令
    hal_float_t freq_min;   // 最小频率
    hal_float_t freq_max;   // 最大频率

    hal_float_t dc_fb;      // 占空比反馈
    hal_float_t pos_fb;     // 位置反馈
    hal_float_t vel_fb;     // 速度反馈
    hal_float_t freq_fb;    // 频率反馈
    hal_s32_t counts;       // 计数值

    hal_u32_t ctrl_type;    // 控制类型
    hal_s32_t freq_mHz;     // 频率(毫赫兹)
    hal_u32_t freq_min_mHz; // 最小频率(毫赫兹)
    hal_u32_t freq_max_mHz; // 最大频率(毫赫兹)
    hal_s32_t dc_s32;       // 占空比(32位整数格式)
}
pwm_ch_priv_t;

// PWM相关全局变量
static pwm_ch_shmem_t *pwmh;                    // PWM共享内存指针
static pwm_ch_priv_t pwmp[PWM_CH_MAX_CNT] = {0};// PWM私有数据数组
static uint8_t pwm_ch_cnt = 0;                  // PWM通道计数

// 用于简化PWM HAL数据访问的宏定义
#define ph *pwmh[ch]   // PWM HAL数据访问宏(ph意为PWM HAL)
#define pp pwmp[ch]    // PWM私有数据访问宏(pp意为PWM Private)

// PWM控制类型枚举
enum
{
    PWM_CTRL_BY_POS,   // 位置控制模式
    PWM_CTRL_BY_VEL,   // 速度控制模式
    PWM_CTRL_BY_FREQ   // 频率控制模式
};

#if ENC_MODULE_ENABLED    // 如果编码器模块被启用

// 编码器通道共享内存数据结构定义
typedef struct
{
    hal_bit_t *enable;        // 使能控制(输入)

    hal_bit_t *cnt_mode;      // 计数器模式选择(输入输出)
    hal_bit_t *x4_mode;       // 4倍计数模式选择(输入输出)
    hal_bit_t *index_enable;  // 索引信号使能(输入输出)
    hal_bit_t *reset;         // 复位控制(输入)

    hal_u32_t *a_port;        // A相端口配置(输入)
    hal_u32_t *a_pin;         // A相引脚配置(输入)
    hal_bit_t *a_inv;         // A相信号反相设置(输入)
    hal_bit_t *a_all;         // A相所有边沿触发设置(输入)

    hal_u32_t *b_port;        // B相端口配置(输入)
    hal_u32_t *b_pin;         // B相引脚配置(输入)

    hal_u32_t *z_port;        // Z相端口配置(输入)
    hal_u32_t *z_pin;         // Z相引脚配置(输入)
    hal_bit_t *z_inv;         // Z相信号反相设置(输入)
    hal_bit_t *z_all;         // Z相所有边沿触发设置(输入)

    hal_float_t *pos_scale;   // 位置比例因子(输入输出)

    hal_float_t *pos;         // 位置值(输出)
    hal_float_t *vel;         // 速度值(输出)
    hal_float_t *vel_rpm;     // 转速值(RPM)(输出)
    hal_s32_t *counts;        // 计数值(输出)
    hal_u32_t *period_ticks;  // 周期计数值(输出)
}
enc_ch_shmem_t;

// 编码器通道私有数据结构定义
typedef struct
{
    hal_bit_t enable;         // 使能状态

    hal_bit_t cnt_mode;       // 计数器模式
    hal_bit_t x4_mode;        // 4倍计数模式
    hal_bit_t index_enable;   // 索引信号使能
    hal_bit_t reset;          // 复位状态

    hal_u32_t a_port;         // A相端口号
    hal_u32_t a_pin;          // A相引脚号
    hal_bit_t a_inv;          // A相信号反相
    hal_bit_t a_all;          // A相所有边沿触发

    hal_u32_t b_port;         // B相端口号
    hal_u32_t b_pin;          // B相引脚号

    hal_u32_t z_port;         // Z相端口号
    hal_u32_t z_pin;          // Z相引脚号
    hal_bit_t z_inv;          // Z相信号反相
    hal_bit_t z_all;          // Z相所有边沿触发

    hal_float_t pos_scale;    // 位置比例因子

    hal_float_t pos;          // 位置值
    hal_float_t vel;          // 速度值
    hal_float_t vel_rpm;      // 转速值
    hal_s32_t counts;         // 计数值
    hal_u32_t period_ticks;   // 周期计数值

    hal_u32_t no_counts_time; // 无计数时间计数
}
enc_ch_priv_t;

// 编码器相关全局变量
static enc_ch_shmem_t *ench;                    // 编码器共享内存指针
static enc_ch_priv_t encp[ENC_CH_MAX_CNT] = {0};// 编码器私有数据数组
static uint8_t enc_ch_cnt = 0;                  // 编码器通道计数

// 用于简化编码器HAL数据访问的宏定义
#define eh *ench[ch]   // 编码器HAL数据访问宏(eh意为Encoder HAL)
#define ep encp[ch]    // 编码器私有数据访问宏(ep意为Encoder Private)
#endif

// 函数声明部分
// TOOLS - 基本工具函数声明
static void gpio_write(void *arg, long period);      // GPIO写入函数
static void gpio_read(void *arg, long period);       // GPIO读取函数
static void pwm_write(void *arg, long period);       // PWM写入函数
static void pwm_read(void *arg, long period);        // PWM读取函数
#if ENC_MODULE_ENABLED
static void enc_read(void *arg, long period);        // 编码器读取函数
#endif

// 内存分配和导出函数的内联实现
static inline
int32_t malloc_and_export(const char *comp_name, int32_t comp_id)
{
    int8_t* arg_str[2] = {in, out};    // 输入输出引脚配置字符串数组
    int8_t n;                          // 循环计数器
    uint8_t port;                      // 端口号
    int32_t r, ch;                     // 返回值和通道号
    // PWM配置相关变量
    int8_t *data = pwm, *token, type[PWM_CH_MAX_CNT] = {0};
    char name[HAL_NAME_LEN + 1];       // HAL名称缓冲区

    // 初始化引脚掩码数组
    for ( n = GPIO_PINS_MAX_CNT; n--; ) pin_msk[n] = 1UL << n;

    // 为GPIO分配共享内存
    for ( port = GPIO_PORTS_MAX_CNT; port--; )
    {
        // 为每个端口的正常逻辑引脚分配内存
        gpio_hal_0[port] = hal_malloc(GPIO_PINS_MAX_CNT * sizeof(hal_bit_t *));
        // 为每个端口的反相逻辑引脚分配内存
        gpio_hal_1[port] = hal_malloc(GPIO_PINS_MAX_CNT * sizeof(hal_bit_t *));
        // 为每个端口的上下拉配置分配内存
        gpio_hal_pull[port] = hal_malloc(GPIO_PINS_MAX_CNT * sizeof(hal_s32_t *));
        // 为每个端口的驱动强度配置分配内存
        gpio_hal_drive[port] = hal_malloc(GPIO_PINS_MAX_CNT * sizeof(hal_u32_t *));

        // 检查内存分配是否成功
        if ( !gpio_hal_0[port] || !gpio_hal_1[port] ||
             !gpio_hal_pull[port] || !gpio_hal_drive[port]
        ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.gpio: port %s hal_malloc() failed \n", comp_name, gpio_name[port]);
            return -1;
        }
    }

    // 导出GPIO HAL引脚部分
    // 遍历输入和输出配置(n=0表示输入，n=1表示输出)
    for ( n = 2; n--; )
    {
        // 跳过空配置
        if ( !arg_str[n] ) continue;

        int8_t *data = arg_str[n], *token;
        uint8_t pin, found;
        int32_t retval;
        int8_t* type_str = n ? "out" : "in";  // 确定引脚类型字符串

        // 使用逗号分隔符解析配置字符串
        while ( (token = strtok(data, ",")) != NULL )
        {
            // strtok第一次调用后需要将data设为NULL
            if ( data != NULL ) data = NULL;
            // 跳过长度小于3的配置项(至少需要"PAn"格式)
            if ( strlen(token) < 3 ) continue;

            // 尝试匹配端口名称(如PA、PB等)
            for ( found = 0, port = GPIO_PORTS_MAX_CNT; port--; ) {
                if ( 0 == memcmp(token, gpio_name[port], 2) ) {
                    found = 1;
                    break;
                }
            }

            // 如果没找到匹配的端口名称，跳过此配置项
            if ( !found ) continue;

            // 解析引脚号(从字符串第3个字符开始)
            pin = (uint8_t) strtoul(&token[2], NULL, 10);

            // 验证引脚号的有效性
            if ( (pin == 0 && token[2] != '0') || pin >= GPIO_PINS_MAX_CNT ) continue;

            // 导出正常逻辑的引脚
            retval = hal_pin_bit_newf( (n ? HAL_IN : HAL_OUT), &gpio_hal_0[port][pin], comp_id, "%s.gpio.%s-%s", comp_name, token, type_str);

            // 导出反相逻辑的引脚
            retval += hal_pin_bit_newf( (n ? HAL_IN : HAL_OUT), &gpio_hal_1[port][pin], comp_id, "%s.gpio.%s-%s-not", comp_name, token, type_str);

            // 导出上下拉配置引脚
            retval += hal_pin_s32_newf( HAL_IN, &gpio_hal_pull[port][pin], comp_id,  "%s.gpio.%s-pull", comp_name, token);

            // 导出驱动强度配置引脚
            retval += hal_pin_u32_newf( HAL_IN, &gpio_hal_drive[port][pin], comp_id, "%s.gpio.%s-multi-drive-level", comp_name, token);

            // 检查是否所有引脚都导出成功
            if (retval < 0) {
                rtapi_print_msg(RTAPI_MSG_ERR, "%s.gpio: pin %s export failed \n", comp_name, token);
                return -1;
            }

            // 根据引脚类型配置GPIO
            if ( n ) {
                // 输出引脚配置
                gpio_out_cnt++;                         // 增加输出引脚计数
                gpio_out_mask[port] |= pin_msk[pin];   // 设置输出掩码
                gpio_pin_func_set(port, pin, GPIO_FUNC_OUT, 0);  // 设置为输出功能
            } else {
                // 输入引脚配置
                gpio_in_cnt++;                          // 增加输入引脚计数
                gpio_in_mask[port] |= pin_msk[pin];    // 设置输入掩码
                gpio_pin_func_set(port, pin, GPIO_FUNC_IN, 0);   // 设置为输入功能
            }

            // 禁用引脚的上下拉功能
            gpio_pin_pull_set(port, pin, GPIO_PULL_DISABLE, 0);

            // 获取引脚初始状态并设置HAL引脚值
            *gpio_hal_0[port][pin] = gpio_pin_get(port, pin, 0);                    // 设置正常逻辑值
            *gpio_hal_1[port][pin] = *gpio_hal_0[port][pin] ? 0 : 1;               // 设置反相逻辑值
            gpio_hal_0_prev[port][pin] = *gpio_hal_0[port][pin];                    // 保存正常逻辑状态
            gpio_hal_1_prev[port][pin] = *gpio_hal_1[port][pin];                    // 保存反相逻辑状态

            // 设置引脚上下拉状态
            switch ( gpio_pin_pull_get(port, pin, 0) ) {
                case GPIO_PULL_UP:      *gpio_hal_pull[port][pin] = 1; break;       // 上拉
                case GPIO_PULL_DOWN:    *gpio_hal_pull[port][pin] = -1; break;      // 下拉
                default:                *gpio_hal_pull[port][pin] = 0;              // 禁用上下拉
            }
            gpio_hal_pull_prev[port][pin] = *gpio_hal_pull[port][pin];             // 保存上下拉状态

            // 获取和保存驱动强度设置
            *gpio_hal_drive[port][pin] = gpio_pin_multi_drive_get(port, pin, 0);    // 获取当前驱动强度
            gpio_hal_drive_prev[port][pin] = *gpio_hal_drive[port][pin];            // 保存驱动强度状态

            // 更新端口使用统计
            if ( port >= gpio_ports_cnt ) gpio_ports_cnt = port + 1;                // 更新使用的最大端口号
            if ( pin >= gpio_pins_cnt[port] ) gpio_pins_cnt[port] = pin + 1;        // 更新端口使用的最大引脚号
        }
    }

    // 如果有GPIO引脚被配置，导出GPIO相关的HAL函数
    if ( gpio_out_cnt || gpio_in_cnt ) {
        r = 0;  // 重置返回值

        // 导出GPIO写函数
        rtapi_snprintf(name, sizeof(name), "%s.gpio.write", comp_name);
        r += hal_export_funct(name, gpio_write, 0, 0, 0, comp_id);

        // 导出GPIO读函数
        rtapi_snprintf(name, sizeof(name), "%s.gpio.read", comp_name);
        r += hal_export_funct(name, gpio_read, 0, 0, 0, comp_id);

        // 检查函数导出是否成功
        if ( r ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.gpio: HAL functions export failed\n", comp_name);
            return -1;
        }
    }

    // 获取PWM通道数量和类型配置
    while ( (token = strtok(data, ",")) != NULL ) {
        if ( data != NULL ) data = NULL;

        // 根据配置字符确定通道类型
        if      ( token[0] == 'P' || token[0] == 'p' ) type[pwm_ch_cnt++] = PWM_CTRL_BY_POS;   // 位置控制
        else if ( token[0] == 'V' || token[0] == 'v' ) type[pwm_ch_cnt++] = PWM_CTRL_BY_VEL;   // 速度控制
        else if ( token[0] == 'F' || token[0] == 'f' ) type[pwm_ch_cnt++] = PWM_CTRL_BY_FREQ;  // 频率控制
    }

    // 如果配置了PWM通道
    if ( pwm_ch_cnt )
    {
        // 导出PWM相关的HAL函数
        r = 0;  // 重置返回值
        // 导出PWM写函数(用于更新PWM输出)
        rtapi_snprintf(name, sizeof(name), "%s.pwm.write", comp_name);
        r += hal_export_funct(name, pwm_write, 0, 1, 0, comp_id);
        // 导出PWM读函数(用于读取PWM状态)
        rtapi_snprintf(name, sizeof(name), "%s.pwm.read", comp_name);
        r += hal_export_funct(name, pwm_read, 0, 1, 0, comp_id);
        // 检查函数导出是否成功
        if ( r ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.pwm: HAL functions export failed\n", comp_name);
            return -1;
        }

        // 限制PWM通道数量不超过最大值
        if ( pwm_ch_cnt > PWM_CH_MAX_CNT ) pwm_ch_cnt = PWM_CH_MAX_CNT;

        // 为PWM通道分配共享内存
        pwmh = hal_malloc(pwm_ch_cnt * sizeof(pwm_ch_shmem_t));
        if ( !pwmh ) PRINT_ERROR_AND_RETURN("hal_malloc() failed", -1);

        // 导出PWM HAL引脚并设置默认值
        #define EXPORT_PIN(IO_TYPE,VAR_TYPE,VAL,NAME,DEFAULT) \
            r += hal_pin_##VAR_TYPE##_newf(IO_TYPE, &(pwmh[ch].VAL), comp_id,\
            "%s.pwm.%d." NAME, comp_name, ch);\
            ph.VAL = DEFAULT;\
            pp.VAL = DEFAULT;

        // 遍历所有PWM通道
        for ( r = 0, ch = pwm_ch_cnt; ch--; )
        {
            // 导出HAL引脚

            // 通道控制
            EXPORT_PIN(HAL_IN,bit,enable,"enable", 0);         // 通道使能控制

            // PWM输出配置
            EXPORT_PIN(HAL_IN,u32,pwm_port,"pwm-port", UINT32_MAX);    // PWM端口
            EXPORT_PIN(HAL_IN,u32,pwm_pin,"pwm-pin", UINT32_MAX);      // PWM引脚
            EXPORT_PIN(HAL_IN,bit,pwm_inv,"pwm-invert", 0);            // PWM信号反相

            // 方向控制配置
            EXPORT_PIN(HAL_IN,u32,dir_port,"dir-port", UINT32_MAX);    // 方向控制端口
            EXPORT_PIN(HAL_IN,u32,dir_pin,"dir-pin", UINT32_MAX);      // 方向控制引脚
            EXPORT_PIN(HAL_IN,bit,dir_inv,"dir-invert", 0);            // 方向信号反相
            EXPORT_PIN(HAL_IO,u32,dir_hold,"dir-hold", 50000);         // 方向保持时间(ns)
            EXPORT_PIN(HAL_IO,u32,dir_setup,"dir-setup", 50000);       // 方向建立时间(ns)

            // 占空比控制参数
            EXPORT_PIN(HAL_IN,float,dc_cmd,"dc-cmd", 0.0);             // 占空比命令
            EXPORT_PIN(HAL_IO,float,dc_min,"dc-min", -1.0);            // 最小占空比
            EXPORT_PIN(HAL_IO,float,dc_max,"dc-max", 1.0);             // 最大占空比
            EXPORT_PIN(HAL_IO,u32,dc_max_t,"dc-max-t", 0);            // 最大占空时间
            EXPORT_PIN(HAL_IO,float,dc_offset,"dc-offset", 0.0);       // 占空比偏移
            EXPORT_PIN(HAL_IO,float,dc_scale,"dc-scale", 1.0);         // 占空比比例因子

            // 位置控制参数
            EXPORT_PIN(HAL_IO,float,pos_scale,"pos-scale", 1.0);       // 位置比例因子
            EXPORT_PIN(HAL_IN,float,pos_cmd,"pos-cmd", 0.0);           // 位置命令值

            // 速度控制参数
            EXPORT_PIN(HAL_IO,float,vel_scale,"vel-scale", 1.0);       // 速度比例因子
            EXPORT_PIN(HAL_IN,float,vel_cmd,"vel-cmd", 0.0);           // 速度命令值

            // 频率控制参数
            EXPORT_PIN(HAL_IO,float,freq_cmd,"freq-cmd", 0.0);         // 频率命令值
            EXPORT_PIN(HAL_IO,float,freq_min,"freq-min", 50.0);        // 最小频率(Hz)
            EXPORT_PIN(HAL_IO,float,freq_max,"freq-max", 500000.0);    // 最大频率(Hz)

            // 反馈参数
            EXPORT_PIN(HAL_OUT,float,dc_fb,"dc-fb", 0.0);             // 占空比反馈
            EXPORT_PIN(HAL_OUT,float,pos_fb,"pos-fb", 0.0);           // 位置反馈
            EXPORT_PIN(HAL_OUT,float,freq_fb,"freq-fb", 0.0);         // 频率反馈
            EXPORT_PIN(HAL_OUT,float,vel_fb,"vel-fb", 0.0);           // 速度反馈
            EXPORT_PIN(HAL_OUT,s32,counts,"counts", 0);               // 计数值反馈

            // 初始化通道私有数据
            pp.ctrl_type = type[ch];           // 设置控制类型
            pp.freq_mHz = 0;                   // 初始频率为0
            pp.freq_min_mHz = 50000;           // 设置最小频率(50Hz = 50000mHz)
            pp.freq_max_mHz = 500000000;       // 设置最大频率(500kHz = 500000000mHz)
            pp.dc_s32 = 0;                     // 初始占空比为0
        }

        // 检查是否所有引脚导出成功
        if ( r ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.pwm: HAL pins export failed\n", comp_name);
            return -1;
        }

        #undef EXPORT_PIN  // 取消宏定义
    }

    // 设置PWM通道数量到共享内存
    pwm_data_set(PWM_CH_CNT, pwm_ch_cnt, 1);

#if ENC_MODULE_ENABLED  // 如果编码器模块被使能
    // 获取编码器通道数量配置
    enc_ch_cnt = (uint8_t) strtoul((const char *)encoders, NULL, 10);

    // 如果配置了编码器通道
    if ( enc_ch_cnt )
    {
        // 导出编码器HAL函数
        r = 0;
        // 导出编码器读取函数
        rtapi_snprintf(name, sizeof(name), "%s.encoder.read", comp_name);
        r += hal_export_funct(name, enc_read, 0, 1, 0, comp_id);
        // 检查函数导出是否成功
        if ( r ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.encoder: HAL functions export failed\n", comp_name);
            return -1;
        }

        // 限制编码器通道数量不超过最大值
        if ( enc_ch_cnt > ENC_CH_MAX_CNT ) enc_ch_cnt = ENC_CH_MAX_CNT;

        // 为编码器通道分配共享内存
        ench = hal_malloc(enc_ch_cnt * sizeof(enc_ch_shmem_t));
        if ( !ench ) PRINT_ERROR_AND_RETURN("hal_malloc() failed", -1);

        // 导出编码器HAL引脚并设置默认值的宏定义
        #define EXPORT_PIN(IO_TYPE,VAR_TYPE,VAL,NAME,DEFAULT) \
            r += hal_pin_##VAR_TYPE##_newf(IO_TYPE, &(ench[ch].VAL), comp_id,\
            "%s.encoder.%d." NAME, comp_name, ch);\
            eh.VAL = DEFAULT;\
            ep.VAL = DEFAULT;

        // 遍历所有编码器通道
        for ( r = 0, ch = enc_ch_cnt; ch--; )
        {
            // 导出HAL引脚

            // 基本控制引脚
            EXPORT_PIN(HAL_IN,bit,enable,"enable", 0);              // 通道使能控制

            // 模式控制引脚
            EXPORT_PIN(HAL_IO,bit,cnt_mode,"counter-mode", 0);      // 计数器模式
            EXPORT_PIN(HAL_IO,bit,x4_mode,"x4-mode", 0);           // 4倍计数模式
            EXPORT_PIN(HAL_IO,bit,index_enable,"index-enable", 0);  // 索引信号使能
            EXPORT_PIN(HAL_IN,bit,reset,"reset", 0);               // 复位控制

            // A相配置引脚
            EXPORT_PIN(HAL_IN,u32,a_port,"A-port", UINT32_MAX);    // A相端口
            EXPORT_PIN(HAL_IN,u32,a_pin,"A-pin", UINT32_MAX);      // A相引脚
            EXPORT_PIN(HAL_IN,bit,a_inv,"A-invert", 0);            // A相信号反相
            EXPORT_PIN(HAL_IN,bit,a_all,"A-all-edges", 0);         // A相所有边沿触发

            // B相配置引脚
            EXPORT_PIN(HAL_IN,u32,b_port,"B-port", UINT32_MAX);    // B相端口
            EXPORT_PIN(HAL_IN,u32,b_pin,"B-pin", UINT32_MAX);      // B相引脚

            // Z相(索引)配置引脚
            EXPORT_PIN(HAL_IN,u32,z_port,"Z-port", UINT32_MAX);    // Z相端口
            EXPORT_PIN(HAL_IN,u32,z_pin,"Z-pin", UINT32_MAX);      // Z相引脚
            EXPORT_PIN(HAL_IN,bit,z_inv,"Z-invert", 0);            // Z相信号反相
            EXPORT_PIN(HAL_IN,bit,z_all,"Z-all-edges", 0);         // Z相所有边沿触发

            // 比例因子
            EXPORT_PIN(HAL_IO,float,pos_scale,"pos-scale", 1.0);   // 位置比例因子

            // 反馈引脚
            EXPORT_PIN(HAL_OUT,float,pos,"pos", 0.0);              // 位置反馈
            EXPORT_PIN(HAL_OUT,float,vel,"vel", 0.0);              // 速度反馈
            EXPORT_PIN(HAL_OUT,float,vel_rpm,"vel-rpm", 0.0);      // RPM速度反馈
            EXPORT_PIN(HAL_OUT,s32,counts,"counts", 0);            // 计数值反馈
            EXPORT_PIN(HAL_OUT,u32,period_ticks,"period_ticks", 0);// 周期计数反馈

            // 初始化通道私有数据
            ep.no_counts_time = 0;  // 初始化无计数时间
        }
        // 检查是否所有编码器引脚导出成功
        if ( r ) {
            rtapi_print_msg(RTAPI_MSG_ERR, "%s.encoder: HAL pins export failed\n", comp_name);
            return -1;
        }

        #undef EXPORT_PIN  // 取消宏定义
    }

    // 设置编码器通道数量到共享内存
    enc_data_set(ENC_CH_CNT, enc_ch_cnt, 1);
#endif

    return 0;
}

// GPIO写入功能实现
static inline
void gpio_read(void *arg, long period)
{
    static uint32_t port, pin, port_state;

    // 如果没有输入引脚，直接返回
    if ( !gpio_in_cnt ) return;

    // 遍历所有GPIO端口
    for ( port = gpio_ports_cnt; port--; )
    {
        // 如果该端口没有输入引脚，跳过
        if ( !gpio_in_mask[port] ) continue;

        // 获取端口状态
        port_state = gpio_port_get(port, 0);

        // 遍历端口的所有引脚
        for ( pin = gpio_pins_cnt[port]; pin--; ) {
            // 如果该引脚不是输入引脚，跳过
            if ( !(gpio_in_mask[port] & pin_msk[pin]) ) continue;

            // 更新HAL引脚状态
            if ( port_state & pin_msk[pin] ) {
                // 引脚为高电平
                *gpio_hal_0[port][pin] = 1;    // 设置正常逻辑值为1
                *gpio_hal_1[port][pin] = 0;    // 设置反相逻辑值为0
            } else {
                // 引脚为低电平
                *gpio_hal_0[port][pin] = 0;    // 设置正常逻辑值为0
                *gpio_hal_1[port][pin] = 1;    // 设置反相逻辑值为1
            }
        }
    }
}

// GPIO写入功能实现
static inline
void gpio_write(void *arg, long period)
{
    static uint32_t port, pin, mask_0, mask_1;

    // 如果既没有输入引脚也没有输出引脚，直接返回
    if ( !gpio_in_cnt && !gpio_out_cnt ) return;

    // 遍历所有GPIO端口
    for ( port = gpio_ports_cnt; port--; )
    {
        // 如果该端口既没有输入引脚也没有输出引脚，跳过
        if ( !gpio_in_mask[port] && !gpio_out_mask[port] ) continue;

        // 初始化端口掩码
        mask_0 = 0;  // 用于清零的掩码
        mask_1 = 0;  // 用于置位的掩码

        // 遍历端口的所有引脚
        for ( pin = gpio_pins_cnt[port]; pin--; )
        {
            // 如果该引脚既不是输入也不是输出，跳过
            if ( !(gpio_in_mask[port] & pin_msk[pin]) && !(gpio_out_mask[port] & pin_msk[pin]) ) continue;

            // 处理引脚上下拉状态变化
            if ( gpio_hal_pull_prev[port][pin] != *gpio_hal_pull[port][pin] )
            {
                if ( *gpio_hal_pull[port][pin] > 0 ) {
                    // 设置为上拉
                    *gpio_hal_pull[port][pin] = 1;
                    gpio_pin_pull_set(port, pin, GPIO_PULL_UP, 0);
                }
                else if ( *gpio_hal_pull[port][pin] < 0 ) {
                    // 设置为下拉
                    *gpio_hal_pull[port][pin] = -1;
                    gpio_pin_pull_set(port, pin, GPIO_PULL_DOWN, 0);
                }
                else {
                    // 禁用上下拉
                    gpio_pin_pull_set(port, pin, GPIO_PULL_DISABLE, 0);
                }
                // 保存新的上下拉状态
                gpio_hal_pull_prev[port][pin] = *gpio_hal_pull[port][pin];
            }

            // 处理引脚驱动强度变化
            if ( gpio_hal_drive_prev[port][pin] != *gpio_hal_drive[port][pin] ) {
                // 确保驱动强度值在有效范围内
                *gpio_hal_drive[port][pin] &= (GPIO_PULL_CNT - 1);
                // 设置新的驱动强度
                gpio_pin_multi_drive_set(port, pin, *gpio_hal_drive[port][pin], 0);
                // 保存新的驱动强度状态
                gpio_hal_drive_prev[port][pin] = *gpio_hal_drive[port][pin];
            }

            // 如果不是输出引脚，继续下一个引脚
            if ( !(gpio_out_mask[port] & pin_msk[pin]) ) continue;

            // 处理正常逻辑引脚状态变化
            if ( *gpio_hal_0[port][pin] != gpio_hal_0_prev[port][pin] ) {
                if ( *gpio_hal_0[port][pin] ) {
                    // 如果正常逻辑值变为高电平
                    *gpio_hal_1[port][pin] = 0;         // 设置反相逻辑为低
                    mask_1 |= pin_msk[pin];            // 添加到置位掩码
                } else {
                    // 如果正常逻辑值变为低电平
                    *gpio_hal_1[port][pin] = 1;         // 设置反相逻辑为高
                    mask_0 |= pin_msk[pin];            // 添加到清零掩码
                }
                // 保存新的状态
                gpio_hal_0_prev[port][pin] = *gpio_hal_0[port][pin];
                gpio_hal_1_prev[port][pin] = *gpio_hal_1[port][pin];
            }

            // 处理反相逻辑引脚状态变化
            if ( *gpio_hal_1[port][pin] != gpio_hal_1_prev[port][pin] ) {
                if ( *gpio_hal_1[port][pin] ) {
                    // 如果反相逻辑值变为高电平
                    *gpio_hal_0[port][pin] = 0;         // 设置正常逻辑为低
                    mask_0 |= pin_msk[pin];            // 添加到清零掩码
                } else {
                    // 如果反相逻辑值变为低电平
                    *gpio_hal_0[port][pin] = 1;         // 设置正常逻辑为高
                    mask_1 |= pin_msk[pin];            // 添加到置位掩码
                }
                // 保存新的状态
                gpio_hal_1_prev[port][pin] = *gpio_hal_1[port][pin];
                gpio_hal_0_prev[port][pin] = *gpio_hal_0[port][pin];
            }
        }

        // 根据掩码更新端口状态
        if ( mask_0 ) gpio_port_clr(port, mask_0, 0);  // 将mask_0中的位清零
        if ( mask_1 ) gpio_port_set(port, mask_1, 0);  // 将mask_1中的位置位
    }
}

// PWM数据更新实现
static inline
int32_t pwm_get_new_dc(uint8_t ch)
{
    // 检查占空比相关参数是否有变化
    if ( ph.dc_cmd    == pp.dc_cmd &&
         ph.dc_scale  == pp.dc_scale &&
         ph.dc_offset == pp.dc_offset &&
         ph.dc_min    == pp.dc_min &&
         ph.dc_max    == pp.dc_max ) return pp.dc_s32;  // 如果没有变化，返回当前值

    // 限制最小最大占空比范围
    if ( ph.dc_min < -1.0 ) ph.dc_min = -1.0;
    if ( ph.dc_max > 1.0 ) ph.dc_max = 1.0;
    if ( ph.dc_max < ph.dc_min ) ph.dc_max = ph.dc_min;

    // 确保比例因子不为0
    if ( ph.dc_scale < 1e-20 && ph.dc_scale > -1e-20 ) ph.dc_scale = 1.0;

    // 计算实际占空比
    ph.dc_fb = ph.dc_cmd / ph.dc_scale + ph.dc_offset;

    // 限制占空比在最小最大值范围内
    if ( ph.dc_fb < ph.dc_min ) ph.dc_fb = ph.dc_min;
    if ( ph.dc_fb > ph.dc_max ) ph.dc_fb = ph.dc_max;

    // 更新私有数据中的占空比参数
    pp.dc_cmd = ph.dc_cmd;         // 保存命令值
    pp.dc_min = ph.dc_min;         // 保存最小值
    pp.dc_max = ph.dc_max;         // 保存最大值
    pp.dc_offset = ph.dc_offset;   // 保存偏移值
    pp.dc_scale = ph.dc_scale;     // 保存比例因子

    // 将浮点占空比转换为32位整数格式返回
    return (int32_t) (ph.dc_fb * INT32_MAX);
}

// PWM频率更新方式的宏定义
#define PWM_FREQ_UPDATE_MPULSES 1  // 使用毫脉冲更新方式
#define PWM_FREQ_UPDATE_SOFT 0     // 使用软件更新方式
#define PWM_FREQ_UPDATE_HARD 0     // 使用硬件更新方式
#define PWM_FREQ_UPDATE_TEST 0     // 使用测试更新方式

// PWM频率更新实现
static inline
int32_t pwm_get_new_freq(uint8_t ch, long period)
{
    int32_t freq = 0;

    // 处理最小频率变化
    if ( pp.freq_min != ph.freq_min ) {
        // 将Hz转换为mHz
        pp.freq_min_mHz = (hal_u32_t) round(ph.freq_min * 1000);
        pp.freq_min = ph.freq_min;
    }
    // 处理最大频率变化
    if ( pp.freq_max != ph.freq_max ) {
        // 将Hz转换为mHz
        pp.freq_max_mHz = (hal_u32_t) round(ph.freq_max * 1000);
        pp.freq_max = ph.freq_max;
    }

    // 根据控制类型计算频率
    switch ( pp.ctrl_type )
    {
        case PWM_CTRL_BY_POS: {  // 位置控制模式
#if PWM_FREQ_UPDATE_MPULSES
            // 使用毫脉冲方式更新
            int64_t pos_cmd_64 = (int64_t) (ph.pos_cmd * ph.pos_scale * 1000);  // 目标位置（毫脉冲）
            int64_t pos_fdb_64 = pwm_ch_pos_get(ch,1);                          // 当前位置（毫脉冲）
            int64_t task_64 = pos_cmd_64 - pos_fdb_64;                          // 位置误差
            // 如果位置误差小于阈值，频率为0
            if ( labs(task_64) < 500 ) freq = 0;
            else freq = (int32_t) (task_64 * ((int64_t)rtapi_clock_set_period(0)) / 1000);
#endif
#if PWM_FREQ_UPDATE_SOFT
            // 软件更新方式
            ph.counts = pwm_ch_data_get(ch, PWM_CH_POS, 1);   // 获取当前计数值
            pp.counts = (int32_t) (pp.pos_cmd * ph.pos_scale);// 计算目标计数值
            int32_t task = pp.counts - ph.counts;             // 计算计数值误差
            if ( abs(task) < 2 ) {
                // 如果误差很小，认为已到达目标位置
                ph.pos_fb = pp.pos_cmd;
            } else {
                // 否则更新位置反馈
                ph.pos_fb = ((hal_float_t)ph.counts) / ph.pos_scale;
            }
            // 根据位置误差和周期计算频率
            freq = (int32_t) ((ph.pos_cmd - ph.pos_fb) * ph.pos_scale * rtapi_clock_set_period(0));
            pp.pos_cmd = ph.pos_cmd;  // 更新命令值缓存
#endif

#if PWM_FREQ_UPDATE_HARD
            // 硬件更新方式
            int32_t counts_task;
            pp.counts = (int32_t) round(ph.pos_cmd * ph.pos_scale);  // 计算目标计数值
            ph.counts = pwm_ch_data_get(ch, PWM_CH_POS, 1);         // 获取当前计数值
            counts_task = pp.counts - ph.counts;                     // 计算计数值误差
            freq = counts_task * rtapi_clock_set_period(0);         // 计算频率
#endif

#if PWM_FREQ_UPDATE_TEST
            // 测试更新方式
            pp.pos_cmd = pp.pos_cmd * period / rtapi_clock_set_period(0);
            freq = (int32_t) ((ph.pos_cmd - pp.pos_cmd) * ph.pos_scale * rtapi_clock_set_period(0));
            pp.pos_cmd = ph.pos_cmd;
#endif
            // 限制频率在最小最大值范围内
            if ( abs(freq) < pp.freq_min_mHz ) freq = 0;
            else if ( abs(freq) > pp.freq_max_mHz ) freq = pp.freq_max_mHz * (freq < 0 ? -1 : 1);
            break;
        }

        case PWM_CTRL_BY_VEL: {  // 速度控制模式
            // 检查速度命令和比例因子是否有变化
            if ( pp.vel_cmd == ph.vel_cmd && pp.vel_scale == ph.vel_scale ) {
                freq = pp.freq_mHz;  // 如果无变化，保持当前频率
                break;
            }
            // 更新速度命令和比例因子
            pp.vel_cmd = ph.vel_cmd;
            pp.vel_scale = ph.vel_scale;

            // 如果速度命令接近零，停止输出
            if ( ph.vel_cmd < 1e-20 && ph.vel_cmd > -1e-20 ) {
                ph.vel_fb = 0;
                break;
            }
            // 确保比例因子不为零
            if ( ph.vel_scale < 1e-20 && ph.vel_scale > -1e-20 ) ph.vel_scale = 1.0;

            // 计算频率（转换为mHz）
            freq = (int32_t) round(ph.vel_scale * ph.vel_cmd * 1000);
            // 限制频率在最小最大值范围内
            if ( abs(freq) < pp.freq_min_mHz ) freq = 0;
            else if ( abs(freq) > pp.freq_max_mHz ) freq = pp.freq_max_mHz * (freq < 0 ? -1 : 1);
            // 计算速度反馈值
            ph.vel_fb = ((hal_float_t) freq) / ph.vel_scale / 1000;
            break;
        }

        case PWM_CTRL_BY_FREQ: {  // 频率控制模式
            // 检查频率命令是否有变化
            if ( pp.freq_cmd == ph.freq_cmd ) {
                freq = pp.freq_mHz;  // 如果无变化，保持当前频率
                break;
            }
            // 更新频率命令
            pp.freq_cmd = ph.freq_cmd;

            // 如果频率命令接近零，停止输出
            if ( ph.freq_cmd < 1e-20 && ph.freq_cmd > -1e-20 ) break;

            // 将Hz转换为mHz
            freq = (int32_t) round(ph.freq_cmd * 1000);
            // 限制频率在最小最大值范围内
            if ( abs(freq) < pp.freq_min_mHz ) freq = 0;
            else if ( abs(freq) > pp.freq_max_mHz ) freq = pp.freq_max_mHz * (freq < 0 ? -1 : 1);
            break;
        }
    }

    // 更新频率反馈值（转换为Hz）
    ph.freq_fb = freq ? ((hal_float_t) freq) / 1000 : 0.0;

    return freq;
}

// PWM引脚更新函数
static inline
void pwm_pins_update(uint8_t ch)
{
    uint32_t upd = 0;

    // 检查PWM引脚配置是否有变化
    if ( pp.pwm_port != ph.pwm_port ) { pp.pwm_port = ph.pwm_port; upd++; }
    if ( pp.pwm_pin  != ph.pwm_pin )  { pp.pwm_pin  = ph.pwm_pin;  upd++; }
    if ( pp.pwm_inv  != ph.pwm_inv )  { pp.pwm_inv  = ph.pwm_inv;  upd++; }

    // 检查方向控制引脚配置是否有变化
    if ( pp.dir_port != ph.dir_port ) { pp.dir_port = ph.dir_port; upd++; }
    if ( pp.dir_pin  != ph.dir_pin )  { pp.dir_pin  = ph.dir_pin;  upd++; }
    if ( pp.dir_inv  != ph.dir_inv )  { pp.dir_inv  = ph.dir_inv;  upd++; }

    // 如果有任何配置变化，重新设置引脚
    if ( upd ) pwm_ch_pins_setup(ch, ph.pwm_port, ph.pwm_pin, ph.pwm_inv, ph.dir_port, ph.dir_pin, ph.dir_inv, 1);
}

// PWM读取模式的宏定义
#define PWM_READ_SOFT 1    // 使用软件方式读取
#define PWM_READ_HARD 0    // 使用硬件方式读取
#define PWM_READ_TEST 0    // 使用测试方式读取

// PWM读取函数实现
static
void pwm_read(void *arg, long period)
{
    static int32_t ch;

    // 遍历所有PWM通道
    for ( ch = pwm_ch_cnt; ch--; ) {
        // 如果通道未使能，跳过
        if ( !ph.enable ) continue;

        // 确保位置比例因子不为零
        if ( ph.pos_scale < 1e-20 && ph.pos_scale > -1e-20 ) ph.pos_scale = 1.0;

        // 如果是位置控制模式
        if ( pp.ctrl_type == PWM_CTRL_BY_POS ) {
#if PWM_READ_HARD
            // 硬件读取方式
            ph.counts = (int32_t) pwm_ch_data_get(ch, PWM_CH_POS, 1);  // 获取当前计数值
            pp.counts = (int32_t) (ph.pos_cmd * ph.pos_scale);         // 计算目标计数值
            ph.pos_fb = abs(pp.counts - ph.counts) < 10 ?
                ph.pos_cmd :                                           // 如果误差小于10，认为到达目标位置
                ((hal_float_t)ph.counts) / ph.pos_scale;              // 否则计算实际位置
#endif

#if PWM_READ_SOFT
            // 软件读取方式
            ph.counts = (int32_t) pwm_ch_data_get(ch, PWM_CH_POS, 1);  // 获取当前计数值
            pp.counts = (int32_t) (ph.pos_scale * ph.pos_cmd);         // 计算目标计数值
            // 如果位置误差小于10，认为达到目标位置
            if ( abs(pp.counts - ph.counts) < 10 ) {
                ph.counts = pp.counts;
                ph.pos_fb = ph.pos_cmd;
            } else {
                // 否则根据实际位置计算反馈值（以毫脉冲为单位）
                ph.pos_fb = ((hal_float_t)pwm_ch_pos_get(ch,1)) / ph.pos_scale / 1000;
            }
#endif

#if PWM_READ_TEST
            // 测试读取方式
            ph.counts = (int32_t) (ph.pos_scale * ph.pos_cmd);  // 使用命令值直接作为计数值
            ph.pos_fb = ph.pos_cmd;                             // 反馈值等于命令值
#endif
        } else {
            // 非位置控制模式，直接获取实际位置作为反馈
            ph.pos_fb = ((hal_float_t)pwm_ch_pos_get(ch,1)) / ph.pos_scale / 1000;
        }
    }
}

// PWM写入函数实现
static
void pwm_write(void *arg, long period)
{
    static int32_t ch, dc, f;

    // 遍历所有PWM通道
    for ( ch = pwm_ch_cnt; ch--; ) {
        // 检查使能状态是否改变
        if ( pp.enable != ph.enable ) {
            pp.enable = ph.enable;
            if ( !ph.enable ) {
                // 如果通道被禁用
                pwm_ch_data_set(ch, PWM_CH_WATCHDOG, 0, 1);   // 清除看门狗
                pwm_ch_times_setup(ch, 0, 0, ph.dc_max_t, ph.dir_hold, ph.dir_setup, 1);  // 停止输出
                continue;
            }
        }

        // 更新引脚配置
        pwm_pins_update(ch);

        // 获取新的占空比和频率值
        dc = pwm_get_new_dc(ch);   // 计算新的占空比
        f = pwm_get_new_freq(ch, period);  // 计算新的频率

        // 设置看门狗值（如果频率和占空比都非零，则激活看门狗）
        pwm_ch_data_set(ch, PWM_CH_WATCHDOG, (f && dc ? 1000 : 0), 1);

        // 如果频率或占空比发生变化，更新PWM参数
        if ( pp.freq_mHz != f || pp.dc_s32 != dc ) {
            pwm_ch_times_setup(ch, f, dc, ph.dc_max_t, ph.dir_hold, ph.dir_setup, 1);
            // 保存新的频率和占空比值
            pp.dc_s32 = dc;
            pp.freq_mHz = f;
        }
    }
}

#if ENC_MODULE_ENABLED    // 如果编码器模块启用

// 检查编码器A相引脚配置是否有效
static inline
uint32_t enc_a_pins_ok(uint8_t ch)
{
    return eh.a_port < GPIO_PORTS_MAX_CNT && eh.a_pin < GPIO_PINS_MAX_CNT;
}

// 检查编码器B相引脚配置是否有效
static inline
uint32_t enc_b_pins_ok(uint8_t ch)
{
    return eh.b_port < GPIO_PORTS_MAX_CNT && eh.b_pin < GPIO_PINS_MAX_CNT;
}

// 检查编码器Z相引脚配置是否有效
static inline
uint32_t enc_z_pins_ok(uint8_t ch)
{
    return eh.z_port < GPIO_PORTS_MAX_CNT && eh.z_pin < GPIO_PINS_MAX_CNT;
}

// 更新编码器引脚配置
static inline
void enc_pins_update(uint8_t ch)
{
    uint32_t upd = 0;

    // 检查A相配置是否有变化
    if ( ep.a_port != eh.a_port ) { ep.a_port = eh.a_port; upd++; }  // 端口变化
    if ( ep.a_pin  != eh.a_pin )  { ep.a_pin  = eh.a_pin;  upd++; }  // 引脚变化
    if ( ep.a_inv  != eh.a_inv )  { ep.a_inv  = eh.a_inv;  upd++; }  // 反相设置变化
    if ( ep.a_all  != eh.a_all )  { ep.a_all  = eh.a_all;  upd++; }  // 边沿触发设置变化

    // 检查B相配置是否有变化
    if ( ep.b_port != eh.b_port ) { ep.b_port = eh.b_port; upd++; }  // 端口变化
    if ( ep.b_pin  != eh.b_pin )  { ep.b_pin  = eh.b_pin;  upd++; }  // 引脚变化

    // 检查Z相配置是否有变化
    if ( ep.z_port != eh.z_port ) { ep.z_port = eh.z_port; upd++; }  // 端口变化
    if ( ep.z_pin  != eh.z_pin )  { ep.z_pin  = eh.z_pin;  upd++; }  // 引脚变化
    if ( ep.z_inv  != eh.z_inv )  { ep.z_inv  = eh.z_inv;  upd++; }  // 反相设置变化
    if ( ep.z_all  != eh.z_all )  { ep.z_all  = eh.z_all;  upd++; }  // 边沿触发设置变化

    // 如果有任何配置变化，重新设置编码器引脚
    if ( !upd ) return;

    // 调用引脚设置函数，更新配置
    enc_ch_pins_setup(ch,
        eh.a_port, eh.a_pin, eh.a_inv, eh.a_all,     // A相配置
        eh.b_port, eh.b_pin,                         // B相配置
        eh.z_port, eh.z_pin, eh.z_inv, eh.z_all,     // Z相配置
        1);                                          // 启用安全检查
}

// 编码器读取函数实现
static
void enc_read(void *arg, long period)
{
    static int32_t counts, ch;

    // 遍历所有编码器通道
    for ( ch = enc_ch_cnt; ch--; )
    {
        // 检查使能状态是否改变
        if ( ep.enable != eh.enable ) {
            ep.enable = eh.enable;
            // 设置通道忙状态（只有在A相引脚配置有效且通道使能时才设置为忙）
            enc_ch_data_set(ch, ENC_CH_BUSY, (enc_a_pins_ok(ch) ? eh.enable : 0), 0);
            // 如果通道被禁用，跳过后续处理
            if ( !eh.enable ) continue;
        }

        // 更新引脚配置
        enc_pins_update(ch);

        // 处理计数模式变化
        if ( ep.cnt_mode != eh.cnt_mode ) {
            ep.cnt_mode = eh.cnt_mode;
            // 根据计数模式设置是否使用B相
            enc_ch_data_set(ch, ENC_CH_B_USE, (enc_b_pins_ok(ch) ? !eh.cnt_mode : 0), 0);
        }

        // 处理复位请求
        if ( eh.reset ) {
            eh.counts = 0;  // 清零计数值
            enc_ch_pos_set(ch, 0, 0, 0);  // 复位编码器位置
        } else {
            // 获取当前计数值和周期计数
            enc_ch_pos_get(ch, &(eh.counts), &(eh.period_ticks), 0);
            // 如果是4倍计数模式，计数值需要除以4
            if ( eh.x4_mode ) eh.counts /= 4;
        }

        // 确保位置比例因子不为零
        if ( eh.pos_scale < 1e-20 && eh.pos_scale > -1e-20 ) eh.pos_scale = 1.0;
        // 计算实际位置
        eh.pos = ((hal_float_t)eh.counts) / eh.pos_scale;

        // 处理索引信号使能状态变化
        if ( ep.index_enable != eh.index_enable ) {
            ep.index_enable = eh.index_enable;
            // 设置Z相使用状态
            enc_ch_data_set(ch, ENC_CH_Z_USE, (enc_z_pins_ok(ch) ? eh.index_enable : 0), 0);
        } else {
            // 从硬件获取当前索引使能状态
            eh.index_enable = enc_ch_data_get(ch, ENC_CH_Z_USE, 0);
        }

        // 处理复位和速度计算
        if ( eh.reset ) {
            // 复位时清零速度值
            eh.vel = 0;
            eh.vel_rpm = 0;
            ep.no_counts_time = 0;
        } else {
            // 累加无计数时间
            ep.no_counts_time += period;
            // 如果计数值发生变化
            if ( ep.counts != eh.counts) {
                // 如果周期计数值较小，使用频率计数方式
                if (eh.period_ticks < 4000) {
                    // 通过计数差值和时间计算速度
                    eh.vel = (((hal_float_t)eh.counts) - ((hal_float_t)ep.counts)) / eh.pos_scale / ((hal_float_t)ep.no_counts_time) * 1000000000;
                } else {
                    // 通过周期计算速度
                    eh.vel = (ARISC_CPU_FREQ / ((hal_float_t)eh.period_ticks)) / eh.pos_scale;
                }
                // 计算RPM值
                eh.vel_rpm = eh.vel * 60;
                // 重置无计数时间
                ep.no_counts_time = 0;
                // 保存当前计数值
                ep.counts = eh.counts;
            } else {
                // 如果超过1秒没有计数变化，认为速度为0
                if ( ep.no_counts_time > 1000000000 ) {
                    eh.vel = 0;
                    eh.vel_rpm = 0;
                    ep.no_counts_time = 0;
                }
            }
        }
    }
}
#endif  // ENC_MODULE_ENABLED


// RTAPI应用程序初始化入口函数
int32_t rtapi_app_main(void)
{
    // 初始化HAL组件，获取组件ID
    if ( (comp_id = hal_init(comp_name)) < 0 ) {
        PRINT_ERROR_AND_RETURN("ERROR: hal_init() failed\n",-1);
    }

    // 初始化共享内存
    if ( shmem_init(comp_name) ) {
        hal_exit(comp_id);  // 初始化失败，退出HAL组件
        return -1;
    }

    // 清理PWM配置
    pwm_cleanup(1);
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，清理编码器配置
    enc_cleanup(1);
#endif

    // 分配和导出HAL引脚
    if ( malloc_and_export(comp_name, comp_id) ) {
        hal_exit(comp_id);  // 分配失败，退出HAL组件
        return -1;
    }

    // 标记组件就绪
    hal_ready(comp_id);

    return 0;
}

// RTAPI应用程序退出函数
void rtapi_app_exit(void)
{
    // 清理PWM配置
    pwm_cleanup(1);
#if ENC_MODULE_ENABLED
    // 如果编码器模块启用，清理编码器配置
    enc_cleanup(1);
#endif
    // 取消共享内存映射
    shmem_deinit();
    // 退出HAL组件
    hal_exit(comp_id);
}
