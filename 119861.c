my_fribidi_strdup (
  char *s
)
{
  char *m;

  m = fribidi_malloc (strlen (s) + 1);
  if (!m)
    return NULL;

  strcpy (m, s);

  return m;
}