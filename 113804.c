fetch_escaped_value(UChar** src, UChar* end, ScanEnv* env, OnigCodePoint* val)
{
  int v;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

  PFETCH_S(c);
  switch (c) {
  case 'M':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH_S(c);
      if (c != '-') return ONIGERR_META_CODE_SYNTAX;
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH_S(c);
      if (c == MC_ESC(env->syntax)) {
        v = fetch_escaped_value(&p, end, env, &c);
        if (v < 0) return v;
      }
      c = ((c & 0xff) | 0x80);
    }
    else
      goto backslash;
    break;

  case 'C':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH_S(c);
      if (c != '-') return ONIGERR_CONTROL_CODE_SYNTAX;
      goto control;
    }
    else
      goto backslash;

  case 'c':
    if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_C_CONTROL)) {
    control:
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH_S(c);
      if (c == '?') {
        c = 0177;
      }
      else {
        if (c == MC_ESC(env->syntax)) {
          v = fetch_escaped_value(&p, end, env, &c);
          if (v < 0) return v;
        }
        c &= 0x9f;
      }
      break;
    }
    /* fall through */

  default:
    {
    backslash:
      c = conv_backslash_value(c, env);
    }
    break;
  }

  *src = p;
  *val = c;
  return 0;
}