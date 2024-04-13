g_markup_vprintf_escaped (const gchar *format,
                          va_list      args)
{
  GString *format1;
  GString *format2;
  GString *result = NULL;
  gchar *output1 = NULL;
  gchar *output2 = NULL;
  const char *p, *op1, *op2;
  va_list args2;

  /* The technique here, is that we make two format strings that
   * have the identical conversions in the identical order to the
   * original strings, but differ in the text in-between. We
   * then use the normal g_strdup_vprintf() to format the arguments
   * with the two new format strings. By comparing the results,
   * we can figure out what segments of the output come from
   * the original format string, and what from the arguments,
   * and thus know what portions of the string to escape.
   *
   * For instance, for:
   *
   *  g_markup_printf_escaped ("%s ate %d apples", "Susan & Fred", 5);
   *
   * We form the two format strings "%sX%dX" and %sY%sY". The results
   * of formatting with those two strings are
   *
   * "%sX%dX" => "Susan & FredX5X"
   * "%sY%dY" => "Susan & FredY5Y"
   *
   * To find the span of the first argument, we find the first position
   * where the two arguments differ, which tells us that the first
   * argument formatted to "Susan & Fred". We then escape that
   * to "Susan & Fred" and join up with the intermediate portions
   * of the format string and the second argument to get
   * "Susan & Fred ate 5 apples".
   */

  /* Create the two modified format strings
   */
  format1 = g_string_new (NULL);
  format2 = g_string_new (NULL);
  p = format;
  while (TRUE)
    {
      const char *after;
      const char *conv = find_conversion (p, &after);
      if (!conv)
        break;

      g_string_append_len (format1, conv, after - conv);
      g_string_append_c (format1, 'X');
      g_string_append_len (format2, conv, after - conv);
      g_string_append_c (format2, 'Y');

      p = after;
    }

  /* Use them to format the arguments
   */
  G_VA_COPY (args2, args);

  output1 = g_strdup_vprintf (format1->str, args);

  if (!output1)
    {
      va_end (args2);
      goto cleanup;
    }

  output2 = g_strdup_vprintf (format2->str, args2);
  va_end (args2);
  if (!output2)
    goto cleanup;
  result = g_string_new (NULL);

  /* Iterate through the original format string again,
   * copying the non-conversion portions and the escaped
   * converted arguments to the output string.
   */
  op1 = output1;
  op2 = output2;
  p = format;
  while (TRUE)
    {
      const char *after;
      const char *output_start;
      const char *conv = find_conversion (p, &after);
      char *escaped;

      if (!conv)        /* The end, after points to the trailing \0 */
        {
          g_string_append_len (result, p, after - p);
          break;
        }

      g_string_append_len (result, p, conv - p);
      output_start = op1;
      while (*op1 == *op2)
        {
          op1++;
          op2++;
        }

      escaped = g_markup_escape_text (output_start, op1 - output_start);
      g_string_append (result, escaped);
      g_free (escaped);

      p = after;
      op1++;
      op2++;
    }

 cleanup:
  g_string_free (format1, TRUE);
  g_string_free (format2, TRUE);
  g_free (output1);
  g_free (output2);

  if (result)
    return g_string_free (result, FALSE);
  else
    return NULL;
}