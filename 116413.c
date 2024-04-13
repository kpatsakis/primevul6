do_vt52_move(void)
{
  term.state = NORMAL;
  uchar y = term.cmd_buf[0];
  uchar x = term.cmd_buf[1];
  if (y < ' ' || x < ' ')
    return;
  move(x - ' ', y - ' ', 0);
}