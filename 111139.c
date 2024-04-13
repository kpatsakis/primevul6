gen_blkmove(codegen_scope *s, uint16_t ainfo, int lv)
{
  int m1 = (ainfo>>7)&0x3f;
  int r  = (ainfo>>6)&0x1;
  int m2 = (ainfo>>1)&0x1f;
  int kd = (ainfo)&0x1;
  int off = m1+r+m2+kd+1;
  if (lv == 0) {
    gen_move(s, cursp(), off, 0);
  }
  else {
    genop_3(s, OP_GETUPVAR, cursp(), off, lv);
  }
  push();
}