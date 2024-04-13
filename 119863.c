die2 (
  const char *fmt,
  const char *arg
)
{
  fprintf (stderr, "%s: ", appname);
  if (fmt)
    fprintf (stderr, fmt, arg);
  fprintf (stderr, "Try `%s --help' for more information.\n", appname);
  exit (-1);
}