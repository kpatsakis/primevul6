initialize_mousetype(SCREEN *sp)
{
    T((T_CALLED("initialize_mousetype()")));

    /* Try gpm first, because gpm may be configured to run in xterm */
#if USE_GPM_SUPPORT
    if (allow_gpm_mouse(sp)) {
	if (!sp->_mouse_gpm_loaded) {
#ifdef HAVE_LIBDL
	    load_gpm_library(sp);
#else /* !HAVE_LIBDL */
	    sp->_mouse_gpm_found = TRUE;
	    sp->_mouse_gpm_loaded = TRUE;
#endif
	}

	/*
	 * The gpm_fd file-descriptor may be negative (xterm).  So we have to
	 * maintain our notion of whether the mouse connection is active
	 * without testing the file-descriptor.
	 */
	if (sp->_mouse_gpm_found && enable_gpm_mouse(sp, TRUE)) {
	    sp->_mouse_type = M_GPM;
	    sp->_mouse_fd = *(my_gpm_fd);
	    T(("GPM mouse_fd %d", sp->_mouse_fd));
	    returnVoid;
	}
    }
#endif /* USE_GPM_SUPPORT */

    /* OS/2 VIO */
#if USE_EMX_MOUSE
    if (!sp->_emxmouse_thread
	&& strstr(SP_TERMTYPE term_names, "xterm") == 0
	&& NonEmpty(key_mouse)) {
	int handles[2];

	if (pipe(handles) < 0) {
	    perror("mouse pipe error");
	    returnVoid;
	} else {
	    int rc;

	    if (!sp->_emxmouse_buttons[0]) {
		const char *s = getenv("MOUSE_BUTTONS_123");

		sp->_emxmouse_buttons[0] = 1;
		if (s && strlen(s) >= 3) {
		    sp->_emxmouse_buttons[1] = s[0] - '0';
		    sp->_emxmouse_buttons[2] = s[1] - '0';
		    sp->_emxmouse_buttons[3] = s[2] - '0';
		} else {
		    sp->_emxmouse_buttons[1] = 1;
		    sp->_emxmouse_buttons[2] = 3;
		    sp->_emxmouse_buttons[3] = 2;
		}
	    }
	    sp->_emxmouse_wfd = handles[1];
	    M_FD(sp) = handles[0];
	    /* Needed? */
	    setmode(handles[0], O_BINARY);
	    setmode(handles[1], O_BINARY);
	    /* Do not use CRT functions, we may single-threaded. */
	    rc = DosCreateThread((unsigned long *) &sp->_emxmouse_thread,
				 mouse_server, (long) sp, 0, 8192);
	    if (rc) {
		printf("mouse thread error %d=%#x", rc, rc);
	    } else {
		sp->_mouse_type = M_XTERM;
	    }
	    returnVoid;
	}
    }
#endif /* USE_EMX_MOUSE */

#if USE_SYSMOUSE
    {
	static char dev_tty[] = "/dev/tty";
	struct mouse_info the_mouse;
	char *the_device = 0;

	if (NC_ISATTY(sp->_ifd))
	    the_device = ttyname(sp->_ifd);
	if (the_device == 0)
	    the_device = dev_tty;

	sp->_mouse_fd = open(the_device, O_RDWR);

	if (sp->_mouse_fd >= 0) {
	    /*
	     * sysmouse does not have a usable user interface for obtaining
	     * mouse events.  The logical way to proceed (reading data on a
	     * stream) only works if one opens the device as root.  Even in
	     * that mode, careful examination shows we lose events
	     * occasionally.  The interface provided for user programs is to
	     * establish a signal handler.  really.
	     *
	     * Take over SIGUSR2 for this purpose since SIGUSR1 is more
	     * likely to be used by an application.  getch() will have to
	     * handle the misleading EINTR's.
	     */
	    signal(SIGUSR2, SIG_IGN);
	    the_mouse.operation = MOUSE_MODE;
	    the_mouse.u.mode.mode = 0;
	    the_mouse.u.mode.signal = SIGUSR2;
	    if (ioctl(sp->_mouse_fd, CONS_MOUSECTL, &the_mouse) != -1) {
		signal(SIGUSR2, handle_sysmouse);
		the_mouse.operation = MOUSE_SHOW;
		ioctl(sp->_mouse_fd, CONS_MOUSECTL, &the_mouse);

#if defined(FBIO_MODEINFO) || defined(CONS_MODEINFO)	/* FreeBSD > 2.x */
		{
#ifndef FBIO_GETMODE		/* FreeBSD 3.x */
#define FBIO_GETMODE    CONS_GET
#define FBIO_MODEINFO   CONS_MODEINFO
#endif /* FBIO_GETMODE */
		    video_info_t the_video;

		    if (ioctl(sp->_mouse_fd,
			      FBIO_GETMODE,
			      &the_video.vi_mode) != -1
			&& ioctl(sp->_mouse_fd,
				 FBIO_MODEINFO,
				 &the_video) != -1) {
			sp->_sysmouse_char_width = the_video.vi_cwidth;
			sp->_sysmouse_char_height = the_video.vi_cheight;
		    }
		}
#endif /* defined(FBIO_MODEINFO) || defined(CONS_MODEINFO) */

		if (sp->_sysmouse_char_width <= 0)
		    sp->_sysmouse_char_width = 8;
		if (sp->_sysmouse_char_height <= 0)
		    sp->_sysmouse_char_height = 16;
		sp->_mouse_type = M_SYSMOUSE;
		returnVoid;
	    }
	}
    }
#endif /* USE_SYSMOUSE */

#ifdef USE_TERM_DRIVER
    CallDriver(sp, td_initmouse);
#else
    /* we know how to recognize mouse events under "xterm" */
    if (NonEmpty(key_mouse)) {
	init_xterm_mouse(sp);
    } else if (strstr(SP_TERMTYPE term_names, "xterm") != 0) {
	if (_nc_add_to_try(&(sp->_keytry), xterm_kmous, KEY_MOUSE) == OK)
	    init_xterm_mouse(sp);
    }
#endif

    returnVoid;
}