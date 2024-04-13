ephy_string_to_int (const char *string,
                    gulong     *integer)
{
  gulong result;
  char *parse_end;

  /* Check for the case of an empty string. */
  if (string == NULL || *string == '\0')
    return FALSE;

  /* Call the standard library routine to do the conversion. */
  errno = 0;
  result = strtol (string, &parse_end, 0);

  /* Check that the result is in range. */
  if (errno == ERANGE)
    return FALSE;

  /* Check that all the trailing characters are spaces. */
  while (*parse_end != '\0') {
    if (!g_ascii_isspace (*parse_end++))
      return FALSE;
  }

  /* Return the result. */
  *integer = result;
  return TRUE;
}