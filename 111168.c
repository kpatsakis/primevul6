get_int_operand(codegen_scope *s, struct mrb_insn_data *data, mrb_int *n)
{
  switch (data->insn) {
  case OP_LOADI__1:
    *n = -1;
    return TRUE;

  case OP_LOADINEG:
    *n = -data->b;
    return TRUE;

  case OP_LOADI_0: case OP_LOADI_1: case OP_LOADI_2: case OP_LOADI_3:
  case OP_LOADI_4: case OP_LOADI_5: case OP_LOADI_6: case OP_LOADI_7:
    *n = data->insn - OP_LOADI_0;
    return TRUE;

  case OP_LOADI:
  case OP_LOADI16:
    *n = (int16_t)data->b;
    return TRUE;

  case OP_LOADI32:
    *n = (mrb_int)((uint32_t)data->b<<16)+data->c;
    return TRUE;

  case OP_LOADL:
    {
      mrb_pool_value *pv = &s->pool[data->b];

      if (pv->tt == IREP_TT_INT32) {
        *n = (mrb_int)pv->u.i32;
      }
#ifdef MRB_INT64
      else if (pv->tt == IREP_TT_INT64) {
        *n = (mrb_int)pv->u.i64;
      }
#endif
      else {
        return FALSE;
      }
    }
    return TRUE;

  default:
    return FALSE;
  }
}