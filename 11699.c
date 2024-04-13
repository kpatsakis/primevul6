reset_tty_settings(int fd, TTY * tty_settings)
{
    GET_TTY(fd, tty_settings);

#ifdef TERMIOS
#if defined(VDISCARD) && defined(CDISCARD)
    reset_char(VDISCARD, CDISCARD);
#endif
    reset_char(VEOF, CEOF);
    reset_char(VERASE, CERASE);
#if defined(VFLUSH) && defined(CFLUSH)
    reset_char(VFLUSH, CFLUSH);
#endif
    reset_char(VINTR, CINTR);
    reset_char(VKILL, CKILL);
#if defined(VLNEXT) && defined(CLNEXT)
    reset_char(VLNEXT, CLNEXT);
#endif
    reset_char(VQUIT, CQUIT);
#if defined(VREPRINT) && defined(CRPRNT)
    reset_char(VREPRINT, CRPRNT);
#endif
#if defined(VSTART) && defined(CSTART)
    reset_char(VSTART, CSTART);
#endif
#if defined(VSTOP) && defined(CSTOP)
    reset_char(VSTOP, CSTOP);
#endif
#if defined(VSUSP) && defined(CSUSP)
    reset_char(VSUSP, CSUSP);
#endif
#if defined(VWERASE) && defined(CWERASE)
    reset_char(VWERASE, CWERASE);
#endif

    tty_settings->c_iflag &= ~((unsigned) (IGNBRK
					   | PARMRK
					   | INPCK
					   | ISTRIP
					   | INLCR
					   | IGNCR
#ifdef IUCLC
					   | IUCLC
#endif
#ifdef IXANY
					   | IXANY
#endif
					   | IXOFF));

    tty_settings->c_iflag |= (BRKINT
			      | IGNPAR
			      | ICRNL
			      | IXON
#ifdef IMAXBEL
			      | IMAXBEL
#endif
	);

    tty_settings->c_oflag &= ~((unsigned) (0
#ifdef OLCUC
					   | OLCUC
#endif
#ifdef OCRNL
					   | OCRNL
#endif
#ifdef ONOCR
					   | ONOCR
#endif
#ifdef ONLRET
					   | ONLRET
#endif
#ifdef OFILL
					   | OFILL
#endif
#ifdef OFDEL
					   | OFDEL
#endif
#ifdef NLDLY
					   | NLDLY
#endif
#ifdef CRDLY
					   | CRDLY
#endif
#ifdef TABDLY
					   | TABDLY
#endif
#ifdef BSDLY
					   | BSDLY
#endif
#ifdef VTDLY
					   | VTDLY
#endif
#ifdef FFDLY
					   | FFDLY
#endif
			       ));

    tty_settings->c_oflag |= (OPOST
#ifdef ONLCR
			      | ONLCR
#endif
	);

    tty_settings->c_cflag &= ~((unsigned) (CSIZE
					   | CSTOPB
					   | PARENB
					   | PARODD
					   | CLOCAL));
    tty_settings->c_cflag |= (CS8 | CREAD);
    tty_settings->c_lflag &= ~((unsigned) (ECHONL
					   | NOFLSH
#ifdef TOSTOP
					   | TOSTOP
#endif
#ifdef ECHOPTR
					   | ECHOPRT
#endif
#ifdef XCASE
					   | XCASE
#endif
			       ));

    tty_settings->c_lflag |= (ISIG
			      | ICANON
			      | ECHO
			      | ECHOE
			      | ECHOK
#ifdef ECHOCTL
			      | ECHOCTL
#endif
#ifdef ECHOKE
			      | ECHOKE
#endif
	);
#endif

    SET_TTY(fd, tty_settings);
}