extra_chars_error(uschar *s, uschar *t1, uschar *t2, uschar *t3)
{
uschar *comment = US"";
if (*s == '#') comment = US" (# is comment only at line start)";
log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
  "extra characters follow %s%s%s%s", t1, t2, t3, comment);
}