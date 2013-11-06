#include <mruby.h>
#include "mruby/variable.h"

#include "stm32f4xx_rcc.h"


static mrb_value
mrb_mruby_stm_systimer_config(mrb_state* mrb, mrb_value self) {

	mrb_int n, i;
	mrb_value val;
	uint32_t interval, j;
	RCC_ClocksTypeDef RCC_Clocks;

	mrb_get_args(mrb, "i", &n);
	interval = (uint32_t)n;
	RCC_GetClocksFreq(&RCC_Clocks);
	j = RCC_Clocks.HCLK_Frequency /1000 * interval;
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000 * interval);

	return self;

}

static mrb_value
mrb_stm_systimer_initialize(mrb_state* mrb, mrb_value self) {

	return self;

}

void
mrb_mruby_stm_systimer_gem_init(mrb_state* mrb) {

	struct RClass *stm_systimer;
	stm_systimer = mrb_define_class(mrb, "StmSystemTimer", mrb->object_class);

	mrb_define_method(mrb, stm_systimer, "enable", mrb_mruby_stm_systimer_config, ARGS_REQ(1));
	mrb_define_method(mrb, stm_systimer, "initialize", mrb_stm_systimer_initialize, ARGS_NONE());

}

void
mrb_mruby_stm_systimer_gem_final(mrb_state* mrb) {
  // finalizer
}
