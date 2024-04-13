and_code_range_buf(BBuf* bbuf1, int not1, BBuf* bbuf2, int not2, BBuf** pbuf)
{
  int r;
  OnigCodePoint i, j, n1, n2, *data1, *data2;
  OnigCodePoint from, to, from1, to1, from2, to2;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf1)) {
    if (not1 != 0 && IS_NOT_NULL(bbuf2)) /* not1 != 0 -> not2 == 0 */
      return bbuf_clone(pbuf, bbuf2);
    return 0;
  }
  else if (IS_NULL(bbuf2)) {
    if (not2 != 0)
      return bbuf_clone(pbuf, bbuf1);
    return 0;
  }

  if (not1 != 0)
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  data1 = (OnigCodePoint* )(bbuf1->p);
  data2 = (OnigCodePoint* )(bbuf2->p);
  GET_CODE_POINT(n1, data1);
  GET_CODE_POINT(n2, data2);
  data1++;
  data2++;

  if (not2 == 0 && not1 == 0) { /* 1 AND 2 */
    for (i = 0; i < n1; i++) {
      from1 = data1[i*2];
      to1   = data1[i*2+1];
      for (j = 0; j < n2; j++) {
        from2 = data2[j*2];
        to2   = data2[j*2+1];
        if (from2 > to1) break;
        if (to2 < from1) continue;
        from = MAX(from1, from2);
        to   = MIN(to1, to2);
        r = add_code_range_to_buf(pbuf, from, to);
        if (r != 0) return r;
      }
    }
  }
  else if (not1 == 0) { /* 1 AND (not 2) */
    for (i = 0; i < n1; i++) {
      from1 = data1[i*2];
      to1   = data1[i*2+1];
      r = and_code_range1(pbuf, from1, to1, data2, n2);
      if (r != 0) return r;
    }
  }

  return 0;
}