
#include "mruby.h"
#include "mruby/variable.h"

#include "stm32f4_discovery.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"



GPIO_InitTypeDef  GPIO_InitStructure;


static mrb_value
mrb_led_initialize(mrb_state *mrb, mrb_value self)
{
	  mrb_int n;
	  mrb_value pin;
	  mrb_get_args(mrb, "i", &n);
	  pin = mrb_fixnum_value(n);
	  mrb_iv_set(mrb, self, mrb_intern(mrb, "@pin"), pin);

	  /* GPIOD Periph clock enable */
//	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
//	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	  GPIO_Init(GPIOD, &GPIO_InitStructure);


	return self;
}

static mrb_value
mrb_led_on(mrb_state *mrb, mrb_value self)
{
	uint16_t n;
	mrb_value pin;
	pin = mrb_iv_get(mrb, self, mrb_intern(mrb, "@pin"));
		n = mrb_fixnum(pin);
/*	  GPIO_SetBits(GPIOD, n);	*/
		STM_EVAL_LEDOn(n);
	return self;
}

static mrb_value
mrb_led_off(mrb_state *mrb, mrb_value self)
{
	uint16_t n;
	mrb_value pin;
	pin = mrb_iv_get(mrb, self, mrb_intern(mrb, "@pin"));
	n = mrb_fixnum(pin);
/*	GPIO_ResetBits(GPIOD, n);	*/
	STM_EVAL_LEDOff(n);
	return self;
}

static mrb_value
mrb_led_toggle(mrb_state *mrb, mrb_value self)
{
	uint16_t n;
	mrb_value pin;
	pin = mrb_iv_get(mrb, self, mrb_intern(mrb, "@pin"));
	n = mrb_fixnum(pin);
	STM_EVAL_LEDToggle(n);
	return self;
}


void
mrb_mruby_led_gem_init(mrb_state* mrb) {
  struct RClass *led;
  led = mrb_define_class(mrb, "Led", mrb->object_class);

  /* define led collar */
/*  mrb_define_const(mrb,led, "GREEN", mrb_fixnum_value(GPIO_Pin_12));
  mrb_define_const(mrb,led, "ORANGE", mrb_fixnum_value(GPIO_Pin_13));
  mrb_define_const(mrb,led, "RED", mrb_fixnum_value(GPIO_Pin_14));
  mrb_define_const(mrb,led, "BLUE", mrb_fixnum_value(GPIO_Pin_15));
*/
  mrb_define_const(mrb,led, "GREEN", mrb_fixnum_value(0));
  mrb_define_const(mrb,led, "ORANGE", mrb_fixnum_value(1));
  mrb_define_const(mrb,led, "RED", mrb_fixnum_value(2));
  mrb_define_const(mrb,led, "BLUE", mrb_fixnum_value(3));


  /* �������` */
  mrb_define_method(mrb, led, "initialize", mrb_led_initialize, ARGS_REQ(1));
  mrb_define_method(mrb, led, "on", mrb_led_on, ARGS_NONE());
  mrb_define_method(mrb, led, "off", mrb_led_off, ARGS_NONE());
  mrb_define_method(mrb, led, "toggle", mrb_led_toggle, ARGS_NONE());

  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void
mrb_mruby_led_gem_final(mrb_state* mrb) {
  // finalizer
}
