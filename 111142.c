mrb_decode_insn(const mrb_code *pc)
{
  struct mrb_insn_data data = { 0 };
  if (pc == 0) return data;
  data.addr = pc;
  mrb_code insn = READ_B();
  uint16_t a = 0;
  uint16_t b = 0;
  uint16_t c = 0;

  switch (insn) {
#define FETCH_Z() /* empty */
#define OPCODE(i,x) case OP_ ## i: FETCH_ ## x (); break;
#include "mruby/ops.h"
#undef OPCODE
  }
  switch (insn) {
  case OP_EXT1:
    insn = READ_B();
    switch (insn) {
#define OPCODE(i,x) case OP_ ## i: FETCH_ ## x ## _1 (); break;
#include "mruby/ops.h"
#undef OPCODE
    }
    break;
  case OP_EXT2:
    insn = READ_B();
    switch (insn) {
#define OPCODE(i,x) case OP_ ## i: FETCH_ ## x ## _2 (); break;
#include "mruby/ops.h"
#undef OPCODE
    }
    break;
  case OP_EXT3:
    insn = READ_B();
    switch (insn) {
#define OPCODE(i,x) case OP_ ## i: FETCH_ ## x ## _3 (); break;
#include "mruby/ops.h"
#undef OPCODE
    }
    break;
  default:
    break;
  }
  data.insn = insn;
  data.a = a;
  data.b = b;
  data.c = c;
  return data;
}