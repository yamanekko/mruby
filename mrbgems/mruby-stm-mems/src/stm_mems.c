#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/array.h"

#include "stm32f4_discovery_lis302dl.h"

static mrb_value
mrb_stm_mems_initialize(mrb_state* mrb, mrb_value self) {
	return self;
}

static mrb_value
mrb_stm_mems_get_base_offset(mrb_state* mrb, mrb_value self) {

	uint8_t buf[6];
	mrb_value val;

	LIS302DL_Read(buf, LIS302DL_OUT_X_ADDR, 6);
	val = mrb_fixnum_value(buf[0]);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@x_offset"), val);
	val = mrb_fixnum_value(buf[2]);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@y_offset"), val);
	val = mrb_fixnum_value(buf[4]);
	mrb_iv_set(mrb, self, mrb_intern(mrb, "@z_offset"), val);

	return self;
}

static mrb_value
mrb_stm_mems_read(mrb_state* mrb, mrb_value self) {

	uint8_t buf[6];
	mrb_value ret[3];
	mrb_value val;
	mrb_int i;
	int8_t x;
	int8_t y;
	int8_t z;

	LIS302DL_Read(buf, LIS302DL_OUT_X_ADDR, 6);

	x = buf[0];
	y = buf[2];
	z = buf[4];
	ret[0] = mrb_fixnum_value((mrb_int)x);
	ret[1] = mrb_fixnum_value((mrb_int)y);
	ret[2] = mrb_fixnum_value((mrb_int)z);
//		ret[0] = mrb_fixnum_value((mrb_int)buf[0]);
//		ret[1] = mrb_fixnum_value((mrb_int)buf[2]);
//		ret[2] = mrb_fixnum_value((mrb_int)buf[4]);

	return mrb_ary_new_from_values(mrb, 3, ret);
}

static mrb_value
mrb_stm_mems_read_calc(mrb_state* mrb, mrb_value self) {

	uint8_t buf[6];
	mrb_value ret[3];
	mrb_value val;
	mrb_int i;
	int8_t offset;
	int8_t x;
	int8_t y;
	int8_t z;

	LIS302DL_Read(buf, LIS302DL_OUT_X_ADDR, 6);

	val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@x_offset"));
	i= mrb_fixnum(val);
	offset= (uint8_t)i;
	x = buf[0] - offset;
	val= mrb_iv_get(mrb, self, mrb_intern(mrb, "@y_offset"));
	i= mrb_fixnum(val);
	offset= (uint8_t)i;
	y = buf[2] - offset;
	z = buf[4];
	ret[0] = mrb_fixnum_value((mrb_int)x);
	ret[1] = mrb_fixnum_value((mrb_int)y);
	ret[2] = mrb_fixnum_value((mrb_int)z);

	return mrb_ary_new_from_values(mrb, 3, ret);
}


static mrb_value
mrb_stm_mems_config(mrb_state* mrb, mrb_value self) {

	LIS302DL_InitTypeDef  LIS302DL_InitStruct;
	LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;

	// MEMS configuration
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);

	// MEMS High Pass Filter configuration
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
  mrb_define_method(mrb, stm_mems, "readAndCalc", mrb_stm_mems_read_calc, ARGS_NONE());
  mrb_define_method(mrb, stm_mems, "getBaseOffset", mrb_stm_mems_get_base_offset, ARGS_NONE());

}

void
mrb_mruby_stm_mems_gem_final(mrb_state* mrb) {
  // finalizer
}
