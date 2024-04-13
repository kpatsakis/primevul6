do_clipboard(void)
{
  char *s = term.cmd_buf;
  char *output;
  int len;
  int ret;

  if (!cfg.allow_set_selection) {
    return;
  }

  while (*s != ';' && *s != '\0') {
    s += 1;
  }
  if (*s != ';') {
    return;
  }
  s += 1;
  if (*s == '?') {
    /* Reading from clipboard is unsupported */
    return;
  }
  len = strlen(s);

  output = malloc(len + 1);
  if (output == NULL) {
    return;
  }

  ret = base64_decode_clip(s, len, output, len);
  if (ret > 0) {
    output[ret] = '\0';
    win_copy_text(output);
  }
  free(output);
}