code_exist_check(OnigCodePoint c, UChar* from, UChar* end, int ignore_escaped,
		 ScanEnv* env)
{
  int in_esc;
  OnigCodePoint code;
  OnigEncoding enc = env->enc;
  UChar* p = from;

  in_esc = 0;
  while (! PEND) {
    if (ignore_escaped && in_esc) {
      in_esc = 0;
    }
    else {
      PFETCH_S(code);
      if (code == c) return 1;
      if (code == MC_ESC(env->syntax)) in_esc = 1;
    }
  }
  return 0;
}