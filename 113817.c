fetch_char_property_to_ctype(UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *prev, *start, *p = *src;

  r = 0;
  start = prev = p;

  while (!PEND) {
    prev = p;
    PFETCH_S(c);
    if (c == '}') {
      r = ONIGENC_PROPERTY_NAME_TO_CTYPE(enc, start, prev);
      if (r < 0) break;

      *src = p;
      return r;
    }
    else if (c == '(' || c == ')' || c == '{' || c == '|') {
      r = ONIGERR_INVALID_CHAR_PROPERTY_NAME;
      break;
    }
  }

  onig_scan_env_set_error_string(env, r, *src, prev);
  return r;
}