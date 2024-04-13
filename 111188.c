new_litbn(codegen_scope *s, const char *p, int base, mrb_bool neg)
{
  int i;
  size_t plen;
  mrb_pool_value *pv;

  plen = strlen(p);
  if (plen > 255) {
    codegen_error(s, "integer too big");
  }
  for (i=0; i<s->irep->plen; i++) {
    size_t len;
    pv = &s->pool[i];
    if (pv->tt != IREP_TT_BIGINT) continue;
    len = pv->u.str[0];
    if (len == plen && pv->u.str[1] == base && memcmp(pv->u.str+2, p, len) == 0)
      return i;
  }

  if (s->irep->plen == s->pcapa) {
    s->pcapa *= 2;
    s->pool = (mrb_pool_value*)codegen_realloc(s, s->pool, sizeof(mrb_pool_value)*s->pcapa);
  }

  pv = &s->pool[s->irep->plen];
  i = s->irep->plen++;
  {
    char *buf;
    pv->tt = IREP_TT_BIGINT;
    buf = (char*)codegen_realloc(s, NULL, plen+3);
    buf[0] = (char)plen;
    buf[1] = base;
    if (neg) buf[1] = 0x80;
    memcpy(buf+2, p, plen);
    buf[plen+2] = '\0';
    pv->u.str = buf;
  }
  return i;
}