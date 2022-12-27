#ifndef BASIC_LIBRARY_H
#define BASIC_LIBRARY_H

void __(void);

// operators
void op_unary_plus(void);
void op_unary_minus(void);
void op_add(void);
void op_sub(void);
void op_mul(void);
void op_div(void);
void op_mod(void);
void op_power(void);
void op_shiftl(void);
void op_shiftr(void);
void op_equal(void);
void op_not_equal(void);
void op_sm_equal(void);
void op_smaller(void);
void op_gr_equal(void);
void op_greater(void);
void op_and(void);
void op_or(void);
void op_xor(void);
void op_not(void);

// commands
void cm_let(void);
void cm_gosub(void);
void cm_else(void);
void cm_endif(void);
void cm_wend(void);
void cm_repeat(void);
void cm_next(void);
void cm_continue(void);
void cm_label(void);
void cm_sub(void);
void cm_goto(void);
void cm_data(void);
void cm_end(void);
void cm_rewind(void);
void cm_pop(void);
void cm_poke(void);
void cm_reset(void);
void cm_sleep(void);
void cm_cls(void);
void cm_hloc(void);
void cm_vloc(void);
void cm_putch(void);
void cm_defch(void);
void cm_selcht(void);
void cm_cursor(void);
void cm_scroll(void);
void cm_tdref(void);
void cm_wait(void);
void cm_clear(void);
void cm_dout(void);
void cm_delete(void);
void cm_rename(void);

// functions
void fn_rnd(void);
void fn_abs(void);
void fn_round(void);
void fn_trunc(void);
void fn_fract(void);
void fn_logd(void);
void fn_logn(void);
void fn_exp(void);
void fn_sqr(void);
void fn_cbr(void);
void fn_sin(void);
void fn_asin(void);
void fn_hsin(void);
void fn_cos(void);
void fn_acos(void);
void fn_hcos(void);
void fn_tan(void);
void fn_atan(void);
void fn_htan(void);
void fn_cotan(void);
void fn_free(void);
void fn_tdram(void);
void fn_tdwdt(void);
void fn_tdhgt(void);
void fn_timer(void);
void fn_peek(void);
void fn_asc(void);
void fn_val(void);
void fn_str(void);
void fn_len(void);
void fn_left(void);
void fn_right(void);
void fn_mid(void);
void fn_instr(void);
void fn_chr(void);
void fn_ain(void);
void fn_din(void);
void fn_exist(void);

#endif
