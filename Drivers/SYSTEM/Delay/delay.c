#include "main.h"
#include "stm32f4xx_hal.h"
#include "./Delay/delay.h"

TIM_HandleTypeDef htim6;

static void TIM6_Init(void) {
    __HAL_RCC_TIM6_CLK_ENABLE();  // 使能TIM6时钟

    // 配置TIM6
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = (uint32_t)(120-1);  // 预分频器：将时钟分频到1MHz（1微秒）
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 1;  // 自动重装载值：1个计数周期（1微秒）
    htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
        // 初始化错误处理
        Error_Handler();
    }

    // 配置TIM6中断
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void Delay_Init(void)
{
    TIM6_Init();
}

volatile static uint32_t delay_counter = 0;

void TIM6_DAC_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        delay_counter++;
    }
}
void Delay_us(uint32_t us) {
    delay_counter = 0;
    HAL_TIM_Base_Start_IT(&htim6);  // 启动TIM6并使能中断

    while (delay_counter < us);

    HAL_TIM_Base_Stop_IT(&htim6);  // 停止TIM6
}
void Delay_ms(uint16_t ms)
{
    for (uint16_t i = 0; i < ms; i++)
    {
        Delay_us(1000);  // 每次延时1000微秒
    }
}
