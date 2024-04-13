write_return(void)
{
  term.curs.wrapnext = false;
  if (term.curs.x < term.marg_left)
    term.curs.x = 0;
  else
    term.curs.x = term.marg_left;
  enable_progress();
}