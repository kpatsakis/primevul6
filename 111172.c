readint(codegen_scope *s, const char *p, int base, mrb_bool neg, mrb_bool *overflow)
{
  const char *e = p + strlen(p);
  mrb_int result = 0;

  mrb_assert(base >= 2 && base <= 16);
  if (*p == '+') p++;
  while (p < e) {
    int n;
    char c = *p;
    switch (c) {
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
      n = c - '0'; break;
    case '8': case '9':
      n = c - '0'; break;
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      n = c - 'a' + 10; break;
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      n = c - 'A' + 10; break;
    default:
      codegen_error(s, "malformed readint input");
      *overflow = TRUE;
      /* not reached */
      return result;
    }
    if (mrb_int_mul_overflow(result, base, &result)) {
    overflow:
      *overflow = TRUE;
      return 0;
    }
    mrb_uint tmp = ((mrb_uint)result)+n;
    if (neg && tmp == (mrb_uint)MRB_INT_MAX+1) {
      *overflow = FALSE;
      return MRB_INT_MIN;
    }
    if (tmp > MRB_INT_MAX) goto overflow;
    result = (mrb_int)tmp;
    p++;
  }
  *overflow = FALSE;
  if (neg) return -result;
  return result;
}