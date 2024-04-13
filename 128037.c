name_validate (GMarkupParseContext  *context,
               const gchar          *name,
               GError              **error)
{
  char mask;
  const char *p;

  /* name start char */
  p = name;
  if (G_UNLIKELY (IS_COMMON_NAME_END_CHAR (*p) ||
                  !(g_ascii_isalpha (*p) || *p == '_' || *p == ':')))
    goto slow_validate;

  for (mask = *p++; *p != '\0'; p++)
    {
      mask |= *p;

      /* is_name_char */
      if (G_UNLIKELY (!(g_ascii_isalnum (*p) ||
                        (!IS_COMMON_NAME_END_CHAR (*p) &&
                         (*p == '.' ||
                          *p == '-' ||
                          *p == '_' ||
                          *p == ':')))))
        goto slow_validate;
    }

  if (mask & 0x80) /* un-common / non-ascii */
    goto slow_validate;

  return TRUE;

 slow_validate:
  return slow_name_validate (context, name, error);
}