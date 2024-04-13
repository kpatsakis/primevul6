ephy_string_blank_chr (char *source)
{
  char *p;

  if (source == NULL)
    return NULL;

  p = source;
  while (*p != '\0') {
    if ((guchar) * p < 0x20)
      *p = ' ';

    p++;
  }

  return source;
}