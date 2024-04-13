append_escaped_text (GString     *str,
                     const gchar *text,
                     gssize       length)
{
  const gchar *p;
  const gchar *end;
  gunichar c;

  p = text;
  end = text + length;

  while (p < end)
    {
      const gchar *next;
      next = g_utf8_next_char (p);

      switch (*p)
        {
        case '&':
          g_string_append (str, "&amp;");
          break;

        case '<':
          g_string_append (str, "&lt;");
          break;

        case '>':
          g_string_append (str, "&gt;");
          break;

        case '\'':
          g_string_append (str, "&apos;");
          break;

        case '"':
          g_string_append (str, "&quot;");
          break;

        default:
          c = g_utf8_get_char (p);
          if ((0x1 <= c && c <= 0x8) ||
              (0xb <= c && c  <= 0xc) ||
              (0xe <= c && c <= 0x1f) ||
              (0x7f <= c && c <= 0x84) ||
              (0x86 <= c && c <= 0x9f))
            g_string_append_printf (str, "&#x%x;", c);
          else
            g_string_append_len (str, p, next - p);
          break;
        }

      p = next;
    }
}