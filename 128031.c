g_markup_escape_text (const gchar *text,
                      gssize       length)
{
  GString *str;

  g_return_val_if_fail (text != NULL, NULL);

  if (length < 0)
    length = strlen (text);

  /* prealloc at least as long as original text */
  str = g_string_sized_new (length);
  append_escaped_text (str, text, length);

  return g_string_free (str, FALSE);
}