g_markup_printf_escaped (const gchar *format, ...)
{
  char *result;
  va_list args;

  va_start (args, format);
  result = g_markup_vprintf_escaped (format, args);
  va_end (args);

  return result;
}