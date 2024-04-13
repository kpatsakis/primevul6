read_string(uschar *s, uschar *name)
{
uschar *yield;
uschar *ss;

if (*s != '\"') return string_copy(s);

ss = s;
yield = string_dequote(&s);

if (s == ss+1 || s[-1] != '\"')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG_IN,
    "missing quote at end of string value for %s", name);

if (*s != 0) extra_chars_error(s, US"string value for ", name, US"");

return yield;
}