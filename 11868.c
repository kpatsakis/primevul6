get_baudrate(TERMINAL *termp)
{
    int my_ospeed;
    int result;
    if (GET_TTY(termp->Filedes, &termp->Nttyb) == OK) {
#ifdef TERMIOS
	termp->Nttyb.c_oflag &= (unsigned) (~OFLAGS_TABS);
#else
	termp->Nttyb.sg_flags &= (unsigned) (~XTABS);
#endif
    }
#ifdef USE_OLD_TTY
    result = (int) cfgetospeed(&(termp->Nttyb));
    my_ospeed = (NCURSES_OSPEED) _nc_ospeed(result);
#else /* !USE_OLD_TTY */
#ifdef TERMIOS
    my_ospeed = (NCURSES_OSPEED) cfgetospeed(&(termp->Nttyb));
#else
    my_ospeed = (NCURSES_OSPEED) termp->Nttyb.sg_ospeed;
#endif
    result = _nc_baudrate(my_ospeed);
#endif
    termp->_baudrate = result;
    ospeed = (NCURSES_OSPEED) my_ospeed;
}