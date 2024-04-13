enable_progress(void)
{
  term.lines[term.curs.y]->lattr |= LATTR_PROGRESS;
}