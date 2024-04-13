ephy_string_find_and_replace (const char *haystack,
                              const char *to_find,
                              const char *to_repl)
{
  GString *str;

  g_assert (haystack);
  g_assert (to_find);
  g_assert (to_repl);

  str = g_string_new (haystack);
  g_string_replace (str, to_find, to_repl, 0);
  return g_string_free (str, FALSE);
}