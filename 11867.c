decode_xterm_X10(SCREEN *sp, MEVENT * eventp)
{
#define MAX_KBUF 3
    unsigned char kbuf[MAX_KBUF + 1];
    size_t grabbed;
    int res;
    bool result;

# if USE_PTHREADS_EINTR
#  if USE_WEAK_SYMBOLS
    if ((pthread_self) && (pthread_kill) && (pthread_equal))
#  endif
	_nc_globals.read_thread = pthread_self();
# endif
    for (grabbed = 0; grabbed < MAX_KBUF; grabbed += (size_t) res) {

	/* For VIO mouse we add extra bit 64 to disambiguate button-up. */
	res = (int) read(
#if USE_EMX_MOUSE
			    (M_FD(sp) >= 0) ? M_FD(sp) : sp->_ifd,
#else
			    sp->_ifd,
#endif
			    kbuf + grabbed, (size_t) (MAX_KBUF - (int) grabbed));
	if (res == -1)
	    break;
    }
#if USE_PTHREADS_EINTR
    _nc_globals.read_thread = 0;
#endif
    kbuf[MAX_KBUF] = '\0';

    TR(TRACE_IEVENT,
       ("_nc_mouse_inline sees the following xterm data: '%s'", kbuf));

    /* there's only one mouse... */
    eventp->id = NORMAL_EVENT;

    result = decode_X10_bstate(sp, eventp, kbuf[0]);

    eventp->x = (kbuf[1] - ' ') - 1;
    eventp->y = (kbuf[2] - ' ') - 1;

    return result;
}