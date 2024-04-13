ephy_string_remove_leading (char *string,
                            char  ch)
{
  char *start;

  g_assert (string);

  for (start = string; *start && *start == ch; start++)
    ;

  memmove (string, start, strlen (start) + 1);

  return string;
}