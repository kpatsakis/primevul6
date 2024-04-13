ephy_string_remove_trailing (char *string,
                             char  ch)
{
  g_assert (string);

  for (gssize i = strlen (string) - 1; i >= 0 && string[i] == ch; i--)
    string[i] = '\0';

  return string;
}