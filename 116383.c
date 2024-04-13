term_write(const char *buf, uint len)
{
 /*
    During drag-selects, some people do not wish to process terminal output,
    because the user may want the screen to hold still to be selected.
    Therefore, we maintain a suspend-output-on-selection buffer which 
    can grow up to a configurable size.
  */
  if (term_selecting() && cfg.suspbuf_max > 0) {
    // if buffer size would be exceeded, flush; prevent uint overflow
    if (len > cfg.suspbuf_max - term.suspbuf_pos)
      term_flush();
    // if buffer length does not exceed max size, append output
    if (len <= cfg.suspbuf_max - term.suspbuf_pos) {
      // make sure buffer is large enough
      if (term.suspbuf_pos + len > term.suspbuf_size) {
        term.suspbuf_size = term.suspbuf_pos + len;
        term.suspbuf = renewn(term.suspbuf, term.suspbuf_size);
      }
      memcpy(term.suspbuf + term.suspbuf_pos, buf, len);
      term.suspbuf_pos += len;
      return;
    }
    // if we cannot buffer, output directly;
    // in this case, we've either flushed already or didn't need to
  }

  term_do_write(buf, len);
}