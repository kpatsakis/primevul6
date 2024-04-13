conv_backslash_value(OnigCodePoint c, ScanEnv* env)
{
  if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_CONTROL_CHARS)) {
    switch (c) {
    case 'n': return '\n';
    case 't': return '\t';
    case 'r': return '\r';
    case 'f': return '\f';
    case 'a': return '\007';
    case 'b': return '\010';
    case 'e': return '\033';
    case 'v':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_V_VTAB))
	return '\v';
      break;

    default:
      break;
    }
  }
  return c;
}