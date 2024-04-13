term_flush(void)
{
  if (term.suspbuf) {
    term_do_write(term.suspbuf, term.suspbuf_pos);
    free(term.suspbuf);
    term.suspbuf = 0;
    term.suspbuf_pos = 0;
    term.suspbuf_size = 0;
  }
}