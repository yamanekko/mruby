#include <mruby.h>
#include "mruby/variable.h"

#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"



static mrb_value
mrb_stm_button_clear_pending_bit(mrb_state* mrb, mrb_value self) {
	  EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);

	return self;
}

static mrb_value
mrb_stm_button_initialize(mrb_state* mrb, mrb_value self) {
	  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	return self;
}

static mrb_value
mrb_stm_button_pressing_p(mrb_state* mrb, mrb_value self) {
	if(STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET){
		return mrb_true_value();
	}else{
		return mrb_false_value();
	}
}

void
mrb_mruby_stm_button_gem_init(mrb_state* mrb) {
  struct RClass *stm_button;
  stm_button = mrb_define_class(mrb, "Button", mrb->object_class);

//  mrb_define_method(mrb, stm_timer, "enable", mrb_stm_timer_set, ARGS_NONE());
//  mrb_define_method(mrb, stm_timer, "initialize", mrb_stm_timer_initialize, ARGS_OPT(1));
//  mrb_define_method(mrb, stm_timer, "interval", mrb_stm_timter_interval, ARGS_REQ(1));
  mrb_define_method(mrb, stm_button, "initialize", mrb_stm_button_initialize, ARGS_NONE());
  mrb_define_method(mrb, stm_button, "pressing?", mrb_stm_button_pressing_p, ARGS_NONE());
  mrb_define_method(mrb, stm_button, "checked", mrb_stm_button_clear_pending_bit, ARGS_NONE());

}

void
mrb_mruby_stm_button_gem_final(mrb_state* mrb) {
  // finalizer
}
