new_lit(codegen_scope *s, mrb_value val)
{
  int i;
  mrb_pool_value *pv;

  switch (mrb_type(val)) {
  case MRB_TT_STRING:
    for (i=0; i<s->irep->plen; i++) {
      mrb_int len;
      pv = &s->pool[i];
      if (pv->tt & IREP_TT_NFLAG) continue;
      len = pv->tt>>2;
      if (RSTRING_LEN(val) != len) continue;
      if (memcmp(pv->u.str, RSTRING_PTR(val), len) == 0)
        return i;
    }
    break;
#ifndef MRB_NO_FLOAT
  case MRB_TT_FLOAT:
    for (i=0; i<s->irep->plen; i++) {
      mrb_float f1, f2;
      pv = &s->pool[i];
      if (pv->tt != IREP_TT_FLOAT) continue;
      pv = &s->pool[i];
      f1 = pv->u.f;
      f2 = mrb_float(val);
      if (f1 == f2 && !signbit(f1) == !signbit(f2)) return i;
    }
    break;
#endif
  case MRB_TT_INTEGER:
    for (i=0; i<s->irep->plen; i++) {
      mrb_int v = mrb_integer(val);
      pv = &s->pool[i];
      if (pv->tt == IREP_TT_INT32) {
        if (v == pv->u.i32) return i;
      }
#ifdef MRB_64BIT
      else if (pv->tt == IREP_TT_INT64) {
        if (v == pv->u.i64) return i;
      }
      continue;
#endif
    }
    break;
  default:
    /* should not happen */
    return 0;
  }

  if (s->irep->plen == s->pcapa) {
    s->pcapa *= 2;
    s->pool = (mrb_pool_value*)codegen_realloc(s, s->pool, sizeof(mrb_pool_value)*s->pcapa);
  }

  pv = &s->pool[s->irep->plen];
  i = s->irep->plen++;

  switch (mrb_type(val)) {
  case MRB_TT_STRING:
    if (RSTR_NOFREE_P(RSTRING(val))) {
      pv->tt = (uint32_t)(RSTRING_LEN(val)<<2) | IREP_TT_SSTR;
      pv->u.str = RSTRING_PTR(val);
    }
    else {
      char *p;
      mrb_int len = RSTRING_LEN(val);
      pv->tt = (uint32_t)(len<<2) | IREP_TT_STR;
      p = (char*)codegen_realloc(s, NULL, len+1);
      memcpy(p, RSTRING_PTR(val), len);
      p[len] = '\0';
      pv->u.str = p;
    }
    break;

#ifndef MRB_NO_FLOAT
  case MRB_TT_FLOAT:
    pv->tt = IREP_TT_FLOAT;
    pv->u.f = mrb_float(val);
    break;
#endif
  case MRB_TT_INTEGER:
#ifdef MRB_INT64
    pv->tt = IREP_TT_INT64;
    pv->u.i64 = mrb_integer(val);
#else
    pv->tt = IREP_TT_INT32;
    pv->u.i32 = mrb_integer(val);
#endif
    break;

  default:
    /* should not happen */
    break;
  }
  return i;
}