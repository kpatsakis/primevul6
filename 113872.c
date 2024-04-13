clear_not_flag_cclass(CClassNode* cc, OnigEncoding enc)
{
  BBuf *tbuf;
  int r;

  if (IS_NCCLASS_NOT(cc)) {
    bitset_invert(cc->bs);

    if (! ONIGENC_IS_SINGLEBYTE(enc)) {
      r = not_code_range_buf(enc, cc->mbuf, &tbuf);
      if (r != 0) return r;

      bbuf_free(cc->mbuf);
      cc->mbuf = tbuf;
    }

    NCCLASS_CLEAR_NOT(cc);
  }

  return 0;
}