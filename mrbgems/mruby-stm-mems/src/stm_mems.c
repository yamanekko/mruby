#include <mruby.h>
#include "mruby/variable.h"
#include "mruby/array.h"


#include "stm32f4_discovery_lis302dl.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"

#define TIM_ARR                          (uint16_t)1999
#define TIM_CCR                          (uint16_t)1000
LIS302DL_InitTypeDef  LIS302DL_InitStruct;


static mrb_value
mrb_stm_mems_initialize(mrb_state* mrb, mrb_value self) {
	return self;
}

static mrb_value
mrb_stm_mems_read(mrb_state* mrb, mrb_value self) {

	uint8_t buf[6];
	mrb_value ret[3];
	LIS302DL_Read(buf, LIS302DL_OUT_X_ADDR, 6);
	ret[0] = mrb_fixnum_value((mrb_int)buf[0]);
	ret[1] = mrb_fixnum_value((mrb_int)buf[2]);
	ret[2] = mrb_fixnum_value((mrb_int)buf[4]);

	return mrb_ary_new_from_values(mrb, 3, ret);
}


static mrb_value
mrb_stm_mems_set_autoreload(mrb_state* mrb, mrb_value self) {
	  mrb_int n;
	  uint8_t TempAcceleration;
	  mrb_get_args(mrb, "i", &n);
	  TempAcceleration = (uint8_t)n;

	TIM_SetAutoreload(TIM4,  TIM_ARR/TempAcceleration);
	return self;
}


static mrb_value
mrb_stm_mems_config(mrb_state* mrb, mrb_value self) {

	uint16_t PrescalerValue = 0;
	LIS302DL_InitTypeDef  LIS302DL_InitStruct;
	LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;

	GPIO_InitTypeDef GPIO_InitStructure;
	  TIM_OCInitTypeDef  TIM_OCInitStructure;
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	  /* --------------------------- System Clocks Configuration -----------------*/
	  /* TIM4 clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	  /* GPIOD clock enable */
//	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	  /*-------------------------- GPIO Configuration ----------------------------*/
	  /* GPIOD Configuration: Pins 12, 13, 14 and 15 in output push-pull */
/*	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);
*/
	  /* Connect TIM4 pins to AF2 */
/*	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
*/
	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 2000) - 1;

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = TIM_ARR;
	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	  /* Enable TIM4 Preload register on ARR */
	  TIM_ARRPreloadConfig(TIM4, ENABLE);

	  /* TIM PWM1 Mode configuration: Channel */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = TIM_CCR;
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  /* Output Compare PWM1 Mode configuration: Channel1 */
	  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	  TIM_CCxCmd(TIM4, TIM_Channel_1, DISABLE);

	  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	  /* Output Compare PWM1 Mode configuration: Channel2 */
	  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	  TIM_CCxCmd(TIM4, TIM_Channel_2, DISABLE);

	  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	  /* Output Compare PWM1 Mode configuration: Channel3 */
	  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	  TIM_CCxCmd(TIM4, TIM_Channel_3, DISABLE);

	  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	  /* Output Compare PWM1 Mode configuration: Channel4 */
	  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	  TIM_CCxCmd(TIM4, TIM_Channel_4, DISABLE);

	  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	  /* TIM4 enable counter */
	  TIM_Cmd(TIM4, ENABLE);

	    /* MEMS configuration */
	    LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	    LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	    LIS302DL_InitStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	    LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	    LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	    LIS302DL_Init(&LIS302DL_InitStruct);

	    /* MEMS High Pass Filter configuration */
	    LIS302DL_FilterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
	    LIS302DL_FilterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_1;
	    LIS302DL_FilterStruct.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_1_2;
	    LIS302DL_FilterConfig(&LIS302DL_FilterStruct);


	    return self;
}

void
mrb_mruby_stm_mems_gem_init(mrb_state* mrb) {
  struct RClass *stm_mems;
  stm_mems = mrb_define_class(mrb, "StmMEMS", mrb->object_class);

  mrb_define_method(mrb, stm_mems, "initialize", mrb_stm_mems_initialize, ARGS_NONE());
  mrb_define_method(mrb, stm_mems, "config", mrb_stm_mems_config, ARGS_NONE());
  mrb_define_method(mrb, stm_mems, "read", mrb_stm_mems_read, ARGS_NONE());
  mrb_define_method(mrb, stm_mems, "autoreload", mrb_stm_mems_set_autoreload, ARGS_REQ(1));
//  mrb_define_method(mrb, stm_mems, "interval", mrb_stm_mems_interval, ARGS_REQ(1));


}

void
mrb_mruby_stm_mems_gem_final(mrb_state* mrb) {
  // finalizer
}
