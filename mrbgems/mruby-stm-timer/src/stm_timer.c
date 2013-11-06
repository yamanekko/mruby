#include "mruby.h"
#include "mruby/variable.h"

#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

static mrb_value
mrb_stm_timer_check(mrb_state *mrb, mrb_value self)
{
	TIM_TypeDef *TIMx = TIM2;

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@timerno"));
	mrb_int i = mrb_fixnum(val);
	uint16_t no = (uint16_t)i;

	switch(no){
	case 2:
		TIMx = TIM2;
		break;
	case 3:
		TIMx = TIM3;
		break;
	case 4:
		TIMx = TIM4;
		break;
	}

	if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
		return mrb_true_value();
	}
	return mrb_false_value();
}

static mrb_value
mrb_stm_timer_restart(mrb_state *mrb, mrb_value self)
{
	TIM_TypeDef *TIMx = TIM2;

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@timerno"));
	mrb_int i = mrb_fixnum(val);

	uint16_t no = (uint16_t)i;
	switch(no){
	case 2:
		TIMx = TIM2;
		break;
	case 3:
		TIMx = TIM3;
		break;
	case 4:
		TIMx = TIM4;
		break;
	}

	TIM_Cmd(TIMx, ENABLE);

	return mrb_false_value();
}

static mrb_value
mrb_stm_timer_stop(mrb_state *mrb, mrb_value self)
{

	TIM_TypeDef *TIMx = TIM2;

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@timerno"));
	mrb_int i = mrb_fixnum(val);
	uint16_t no = (uint16_t)i;

	switch(no){
	case 2:
		TIMx = TIM2;
		break;
	case 3:
		TIMx = TIM3;
		break;
	case 4:
		TIMx = TIM4;
		break;
	}

	TIM_Cmd(TIMx, DISABLE);

	return mrb_false_value();
}

static mrb_value
mrb_stm_timer_set(mrb_state* mrb, mrb_value self) {

	TIM_TypeDef *TIMx = TIM2;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@interval"));
	mrb_int i = mrb_fixnum(val) -1;
	uint32_t interval = (uint32_t)i;

	val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@timerno"));
	i = mrb_fixnum(val);
	uint16_t no = (uint16_t)i;

	switch(no){
	case 2:
		TIMx = TIM2;
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		break;
	case 3:
		TIMx = TIM3;
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		break;
	case 4:
		TIMx = TIM4;
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
		break;
	}

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// TIMx clock enable
	switch(no){
	case 2:
		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		break;
	case 3:
		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		break;
	case 4:
		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		break;
	}
	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = (10 * interval) - 1;  // 1 MHz down to 1 KHz (1 ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1; // 24 MHz Clock down to 1 MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	// TIM IT enable
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	// TIMx enable counter
	TIM_Cmd(TIMx, ENABLE);

	return self;
}

static mrb_value
mrb_stm_timer_initialize(mrb_state* mrb, mrb_value self) {
	mrb_value val;
	val = mrb_fixnum_value(1000);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);

	val = mrb_fixnum_value(2);
	mrb_get_args(mrb, "|o", &val);
	if(!mrb_fixnum_p(val)){
		mrb_iv_set(mrb, self, mrb_intern(mrb, "@timerno"), mrb_fixnum_value(2));
	}else{
		mrb_iv_set(mrb, self, mrb_intern(mrb, "@timerno"), val);
	}

	return self;
}

static mrb_value
mrb_stm_timter_interval(mrb_state *mrb, mrb_value self)
{
	  mrb_int n;
	  mrb_value val;

	//todo: param check
	mrb_get_args(mrb, "i", &n);
	val = mrb_fixnum_value(n);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);

	return self;
}

void
mrb_mruby_stm_timer_gem_init(mrb_state* mrb) {
  struct RClass *stm_timer;
  stm_timer = mrb_define_class(mrb, "StmTimer", mrb->object_class);

  mrb_define_method(mrb, stm_timer, "check", mrb_stm_timer_check, ARGS_NONE());
  mrb_define_method(mrb, stm_timer, "enable", mrb_stm_timer_set, ARGS_NONE());
  mrb_define_method(mrb, stm_timer, "initialize", mrb_stm_timer_initialize, ARGS_OPT(1));
  mrb_define_method(mrb, stm_timer, "interval", mrb_stm_timter_interval, ARGS_REQ(1));
  mrb_define_method(mrb, stm_timer, "restart", mrb_stm_timer_restart, ARGS_NONE());
  mrb_define_method(mrb, stm_timer, "stop", mrb_stm_timer_stop, ARGS_NONE());

}

void
mrb_mruby_stm_timer_gem_final(mrb_state* mrb) {
  // finalizer
}
