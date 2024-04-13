write_event(SCREEN *sp, int down, int button, int x, int y)
{
    char buf[6];
    unsigned long ignore;

    _nc_STRCPY(buf, "\033[M", sizeof(buf));	/* should be the same as key_mouse */
    buf[3] = ' ' + (button - 1) + (down ? 0 : 0x40);
    buf[4] = ' ' + x - LEFT_COL + 1;
    buf[5] = ' ' + y - TOP_ROW + 1;
    DosWrite(sp->_emxmouse_wfd, buf, 6, &ignore);
}