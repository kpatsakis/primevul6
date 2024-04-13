void CLASS tiff_get(unsigned base, unsigned *tag, unsigned *type, unsigned *len, unsigned *save)
{
  *tag = get2();
  *type = get2();
  *len = get4();
  *save = ftell(ifp) + 4;
  if (*len * ("11124811248484"[*type < 14 ? *type : 0] - '0') > 4)
    fseek(ifp, get4() + base, SEEK_SET);
}