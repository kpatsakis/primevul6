term_push_cmd(char c)
{
  uint new_size;

  /* Need 1 more for null byte */
  if (term.cmd_len + 1 < term.cmd_buf_cap) {
    term.cmd_buf[term.cmd_len++] = c;
    term.cmd_buf[term.cmd_len] = 0;
    return true;
  }

  if (term.cmd_buf_cap >= TERM_CMD_BUF_MAX_SIZE) {
    /* Server sends too many cmd characters */
    return false;
  }
  new_size = term.cmd_buf_cap + TERM_CMD_BUF_INC_STEP;
  if (new_size >= TERM_CMD_BUF_MAX_SIZE) {
    // cosmetic limitation (relevant limitation above)
    new_size = TERM_CMD_BUF_MAX_SIZE;
  }
  term.cmd_buf = renewn(term.cmd_buf, new_size);
  term.cmd_buf_cap = new_size;
  term.cmd_buf[term.cmd_len++] = c;
  term.cmd_buf[term.cmd_len] = 0;
  return true;
}