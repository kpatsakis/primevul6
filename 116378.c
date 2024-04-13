write_error(void)
{
  // Write one of REPLACEMENT CHARACTER or, if that does not exist,
  // MEDIUM SHADE which looks appropriately erroneous.
  wchar errch = 0xFFFD;
  win_check_glyphs(&errch, 1, term.curs.attr.attr);
  if (!errch)
    errch = 0x2592;
  write_char(errch, 1);
}