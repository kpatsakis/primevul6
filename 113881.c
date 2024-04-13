str_exist_check_with_esc(OnigCodePoint s[], int n, UChar* from, UChar* to,
		 OnigCodePoint bad, OnigEncoding enc, OnigSyntaxType* syn)
{
  int i, in_esc;
  OnigCodePoint x;
  UChar *q;
  UChar *p = from;

  in_esc = 0;
  while (p < to) {
    if (in_esc) {
      in_esc = 0;
      p += enclen(enc, p);
    }
    else {
      x = ONIGENC_MBC_TO_CODE(enc, p, to);
      q = p + enclen(enc, p);
      if (x == s[0]) {
        for (i = 1; i < n && q < to; i++) {
          x = ONIGENC_MBC_TO_CODE(enc, q, to);
          if (x != s[i]) break;
          q += enclen(enc, q);
        }
        if (i >= n) return 1;
        p += enclen(enc, p);
      }
      else {
        x = ONIGENC_MBC_TO_CODE(enc, p, to);
        if (x == bad) return 0;
        else if (x == MC_ESC(syn)) in_esc = 1;
        p = q;
      }
    }
  }
  return 0;
}