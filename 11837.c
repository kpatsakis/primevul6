decode_xterm_1005(SCREEN *sp, MEVENT * eventp)
{
    char kbuf[80];
    size_t grabbed;
    size_t limit = (sizeof(kbuf) - 1);
    unsigned coords[2];
    bool result;

    coords[0] = 0;
    coords[1] = 0;

# if USE_PTHREADS_EINTR
#  if USE_WEAK_SYMBOLS
    if ((pthread_self) && (pthread_kill) && (pthread_equal))
#  endif
	_nc_globals.read_thread = pthread_self();
# endif
    for (grabbed = 0; grabbed < limit;) {
	int res;

	res = (int) read(
#if USE_EMX_MOUSE
			    (M_FD(sp) >= 0) ? M_FD(sp) : sp->_ifd,
#else
			    sp->_ifd,
#endif
			    (kbuf + grabbed), (size_t) 1);
	if (res == -1)
	    break;
	grabbed += (size_t) res;
	if (grabbed > 1) {
	    size_t check = 1;
	    int n;

	    for (n = 0; n < 2; ++n) {
		int rc;

		if (check >= grabbed)
		    break;
		rc = _nc_conv_to_utf32(&coords[n], kbuf + check, (unsigned)
				       (grabbed - check));
		if (!rc)
		    break;
		check += (size_t) rc;
	    }
	    if (n >= 2)
		break;
	}
    }
#if USE_PTHREADS_EINTR
    _nc_globals.read_thread = 0;
#endif

    TR(TRACE_IEVENT,
       ("_nc_mouse_inline sees the following xterm data: %s",
	_nc_visbufn(kbuf, (int) grabbed)));

    /* there's only one mouse... */
    eventp->id = NORMAL_EVENT;

    result = decode_X10_bstate(sp, eventp, UChar(kbuf[0]));

    eventp->x = (int) (coords[0] - ' ') - 1;
    eventp->y = (int) (coords[1] - ' ') - 1;

    return result;
}