#include <mruby.h>
#include "mruby/variable.h"

#include "stm32f4_discovery.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 54618;
uint16_t PrescalerValue = 0;
//uint32_t interval = 1000;	//ms

uint16_t capture = 0;
uint16_t flg = 0;

// TIM_TypeDef * TIMx = TIM2;

static mrb_value
mrb_stm_timer_check(mrb_state *mrb, mrb_value self)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		return mrb_true_value();
	}
	return mrb_false_value();
}

static mrb_value
mrb_stm_timer_set(mrb_state* mrb, mrb_value self) {

	NVIC_InitTypeDef NVIC_InitStructure;
	// Enable the TIM2 gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@interval"));
//	mrb_int i = mrb_fixnum(val)*1000 -1;
	mrb_int i = mrb_fixnum(val) -1;
	uint32_t interval = (uint32_t)i;

	  /* TIM2 clock enable */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = (10 * interval) - 1;  // 1 MHz down to 1 KHz (1 ms)
	  TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	  /* TIM IT enable */
	  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	  /* TIM2 enable counter */
	  TIM_Cmd(TIM2, ENABLE);

	return self;
}

static mrb_value
mrb_stm_timer_initialize(mrb_state* mrb, mrb_value self) {
	mrb_value val;
	val = mrb_fixnum_value(1000);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);
		return self;
}

static mrb_value
mrb_stm_timter_interval(mrb_state *mrb, mrb_value self)
{
	  mrb_int n;
	  mrb_value val;

	//todo: �����`�F�b�N
	  	mrb_get_args(mrb, "i", &n);
	  	val = mrb_fixnum_value(n);
	  	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);
	return self;
}


void
mrb_mruby_stm_timer_gem_init(mrb_state* mrb) {
  struct RClass *stm_timer;
  stm_timer = mrb_define_class(mrb, "StmTimer", mrb->object_class);

  /* �������` */
/*  mrb_define_method(mrb, stm_timer, "initialize", mrb_stm_timer_initialize, ARGS_REQ(1));
*/
  mrb_define_method(mrb, stm_timer, "check", mrb_stm_timer_check, ARGS_NONE());
  mrb_define_method(mrb, stm_timer, "enable", mrb_stm_timer_set, ARGS_NONE());
  mrb_define_method(mrb, stm_timer, "initialize", mrb_stm_timer_initialize, ARGS_REQ(1));
  mrb_define_method(mrb, stm_timer, "interval", mrb_stm_timter_interval, ARGS_REQ(1));

}

void
mrb_mruby_stm_timer_gem_final(mrb_state* mrb) {
  // finalizer
}
