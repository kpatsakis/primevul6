void imap_get_parent(char *output, const char *mbox, size_t olen, char delim)
{
  int n;

  /* Make a copy of the mailbox name, but only if the pointers are different */
  if (mbox != output)
    mutt_str_strfcpy(output, mbox, olen);

  n = mutt_str_strlen(output);

  /* Let's go backwards until the next delimiter
   *
   * If output[n] is a '/', the first n-- will allow us
   * to ignore it. If it isn't, then output looks like
   * "/aaaaa/bbbb". There is at least one "b", so we can't skip
   * the "/" after the 'a's.
   *
   * If output == '/', then n-- => n == 0, so the loop ends
   * immediately
   */
  for (n--; n >= 0 && output[n] != delim; n--)
    ;

  /* We stopped before the beginning. There is a trailing
   * slash.
   */
  if (n > 0)
  {
    /* Strip the trailing delimiter.  */
    output[n] = '\0';
  }
  else
  {
    output[0] = (n == 0) ? delim : '\0';
  }
}