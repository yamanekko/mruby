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
extern void enable_irq ( void );

static mrb_value
mrb_rs_timer_check(mrb_state *mrb, mrb_value self)
{
	PUT32(ARM_TIMER_CLI,0);
	return mrb_true_value();
}

static mrb_value
mrb_rs_timer_set(mrb_state* mrb, mrb_value self)
{

	mrb_value val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@interval"));
	mrb_int i = mrb_fixnum(val) -1;
	unsigned int interval = (unsigned int)i * 1000;

	PUT32(IRQ_DISABLE_BASIC,1);
    PUT32(ARM_TIMER_CTL,0x003E0000);
    //initial setting?
    PUT32(ARM_TIMER_LOD,1000);
    PUT32(ARM_TIMER_RLD,1000);

    PUT32(ARM_TIMER_DIV,0x000000F9);
    PUT32(ARM_TIMER_CLI,0);
    PUT32(ARM_TIMER_CTL,0x003E00A2);

	while(1) if(GET32(ARM_TIMER_MIS)) break;
	PUT32(ARM_TIMER_CLI,0);

    PUT32(IRQ_ENABLE_BASIC,1);	//don't remove

    while(1) if(GET32(IRQ_BASIC)&1) break;
	PUT32(ARM_TIMER_CLI,0);

    PUT32(ARM_TIMER_LOD,interval);
    PUT32(ARM_TIMER_RLD,interval);

    enable_irq();

	return self;
}

static mrb_value
mrb_rs_timer_initialize(mrb_state* mrb, mrb_value self) {

	mrb_value val;
	val = mrb_fixnum_value(999);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);

	return self;
}

static mrb_value
mrb_rs_timter_interval(mrb_state *mrb, mrb_value self)
{
	mrb_int n;
	mrb_value val;

	mrb_get_args(mrb, "i", &n);
	val = mrb_fixnum_value(n);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@interval"), val);

	return self;
}


void
mrb_mruby_rs_timer_gem_init(mrb_state* mrb) {
	struct RClass *rs_timer;
	rs_timer = mrb_define_class(mrb, "RsTimer", mrb->object_class);

	mrb_define_method(mrb, rs_timer, "check", mrb_rs_timer_check, ARGS_NONE());
	mrb_define_method(mrb, rs_timer, "enable", mrb_rs_timer_set, ARGS_NONE());
	mrb_define_method(mrb, rs_timer, "initialize", mrb_rs_timer_initialize, ARGS_REQ(1));
	mrb_define_method(mrb, rs_timer, "interval", mrb_rs_timter_interval, ARGS_REQ(1));

	PUT32(IRQ_DISABLE_BASIC,1);

}

void
mrb_mruby_rs_timer_gem_final(mrb_state* mrb) {
  // finalizer
}
