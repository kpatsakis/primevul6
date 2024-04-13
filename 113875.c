onig_warning(const char* s)
{
  if (onig_warn == onig_null_warn) return ;

  (*onig_warn)(s);
}