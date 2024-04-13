set_conversions(TTY * tty_settings)
{
#ifdef ONLCR
    tty_settings->c_oflag |= ONLCR;
#endif
    tty_settings->c_iflag |= ICRNL;
    tty_settings->c_lflag |= ECHO;
#ifdef OXTABS
    tty_settings->c_oflag |= OXTABS;
#endif /* OXTABS */

    /* test used to be tgetflag("NL") */
    if (VALID_STRING(newline) && newline[0] == '\n' && !newline[1]) {
	/* Newline, not linefeed. */
#ifdef ONLCR
	tty_settings->c_oflag &= ~((unsigned) ONLCR);
#endif
	tty_settings->c_iflag &= ~((unsigned) ICRNL);
    }
#ifdef OXTABS
    /* test used to be tgetflag("pt") */
    if (VALID_STRING(set_tab) && VALID_STRING(clear_all_tabs))
	tty_settings->c_oflag &= ~OXTABS;
#endif /* OXTABS */
    tty_settings->c_lflag |= (ECHOE | ECHOK);
}