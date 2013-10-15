#include <mruby.h>
#include "mruby/variable.h"
#define ARM_TIMER_LOD 0x2000B400
#define ARM_TIMER_VAL 0x2000B404
#define ARM_TIMER_CTL 0x2000B408
#define ARM_TIMER_CLI 0x2000B40C
#define ARM_TIMER_RIS 0x2000B410
#define ARM_TIMER_MIS 0x2000B414
#define ARM_TIMER_RLD 0x2000B418
#define ARM_TIMER_DIV 0x2000B41C
#define ARM_TIMER_CNT 0x2000B420

#define IRQ_BASIC 0x2000B200
#define IRQ_PEND1 0x2000B204
#define IRQ_PEND2 0x2000B208
#define IRQ_FIQ_CONTROL 0x2000B210
#define IRQ_ENABLE_BASIC 0x2000B218
#define IRQ_DISABLE_BASIC 0x2000B224

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );
extern void enable_irq ( void );
extern void enable_fiq ( void );


/*
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 54618;
uint16_t PrescalerValue = 0;
uint32_t interval = 1000;	//ms

uint16_t capture = 0;
uint16_t flg = 0;
*/
uint32_t interval = 999999;	//1000000-1 1s

static mrb_value
mrb_rs_timer_check(mrb_state *mrb, mrb_value self)
{
/*	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		return mrb_true_value();
	}
*/
    PUT32(ARM_TIMER_CLI,0);

	return mrb_true_value();
}

static mrb_value
mrb_rs_timer_set(mrb_state* mrb, mrb_value self) {
/*	  NVIC_InitTypeDef NVIC_InitStructure;
	  // Enable the TIM2 gloabal Interrupt
	  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
*/
    PUT32(ARM_TIMER_CTL,0x003E0000);
    //初期化時に使うタイマーの周期
    PUT32(ARM_TIMER_LOD,1000-1);
    PUT32(ARM_TIMER_RLD,1000-1);

    PUT32(ARM_TIMER_DIV,0x000000F9);
    PUT32(ARM_TIMER_CLI,0);
    PUT32(ARM_TIMER_CTL,0x003E00A2);

	while(1) if(GET32(ARM_TIMER_MIS)) break;
	PUT32(ARM_TIMER_CLI,0);

	//消してはだめ
    PUT32(IRQ_ENABLE_BASIC,1);

    while(1) if(GET32(IRQ_BASIC)&1) break;
	PUT32(ARM_TIMER_CLI,0);

/*	  mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@interval"));
	  mrb_int i = mrb_fixnum(val)*1000 -1;
	  interval = (uint32_t)i;
*/

	  /* TIM2 clock enable */
//	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	  /* Time base configuration */
//	  TIM_TimeBaseStructure.TIM_Period = (10 * interval) - 1;  // 1 MHz down to 1 KHz (1 ms)
//	  TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
//	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	  /* TIM IT enable */
//	  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	  /* TIM2 enable counter */
//	  TIM_Cmd(TIM2, ENABLE);
    PUT32(ARM_TIMER_LOD,1000000-1);
    PUT32(ARM_TIMER_RLD,1000000-1);

    enable_irq();



	  return self;
}

static mrb_value
mrb_rs_timer_initialize(mrb_state* mrb, mrb_value self) {
/*
	mrb_value val;
	val = mrb_fixnum_value(1000);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);
*/
		return self;
}

static mrb_value
mrb_rs_timter_interval(mrb_state *mrb, mrb_value self)
{
	  mrb_int n;
	  mrb_value val;

	//todo: �����`�F�b�N
/*	mrb_get_args(mrb, "i", &n);
	val = mrb_fixnum_value(n) -1;
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);
*/
	return self;
}


void
mrb_mruby_rs_timer_gem_init(mrb_state* mrb) {
  struct RClass *rs_timer;
  rs_timer = mrb_define_class(mrb, "RsTimer", mrb->object_class);

  /* �������` */
/*  mrb_define_method(mrb, stm_timer, "initialize", mrb_stm_timer_initialize, ARGS_REQ(1));
*/
  mrb_define_method(mrb, rs_timer, "check", mrb_rs_timer_check, ARGS_NONE());
  mrb_define_method(mrb, rs_timer, "enable", mrb_rs_timer_set, ARGS_NONE());
  mrb_define_method(mrb, rs_timer, "initialize", mrb_rs_timer_initialize, ARGS_REQ(1));
  mrb_define_method(mrb, rs_timer, "interval", mrb_rs_timter_interval, ARGS_REQ(1));

}

void
mrb_mruby_rs_timer_gem_final(mrb_state* mrb) {
  // finalizer
}
