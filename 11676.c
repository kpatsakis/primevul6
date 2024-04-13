read_SGR(SCREEN *sp, SGR_DATA * result)
{
    char kbuf[80];		/* bigger than any possible mouse response */
    int grabbed = 0;
    int ch = 0;
    int now = -1;
    int marker = 1;

    memset(result, 0, sizeof(*result));
# if USE_PTHREADS_EINTR
#  if USE_WEAK_SYMBOLS
    if ((pthread_self) && (pthread_kill) && (pthread_equal))
#  endif
	_nc_globals.read_thread = pthread_self();
# endif

    do {
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
	if ((grabbed + MAX_KBUF) >= (int) sizeof(kbuf)) {
	    result->nerror++;
	    break;
	}
	ch = UChar(kbuf[grabbed]);
	kbuf[grabbed + 1] = 0;
	switch (ch) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    if (marker) {
		++now;
		result->nparam = (now + 1);
	    }
	    marker = 0;
	    result->params[now] = (result->params[now] * 10) + (ch - '0');
	    break;
	case ';':
	    if (marker) {
		++now;
		result->nparam = (now + 1);
	    }
	    marker = 1;
	    break;
	default:
	    if (ch < 32 || ch > 126) {
		/*
		 * Technically other characters could be interspersed in the
		 * response.  Ignore those for now.
		 */
		result->nerror++;
		continue;
	    } else if (isFinal(ch)) {
		if (marker) {
		    result->nparam++;
		}
		result->final = ch;
	    } else {
		result->nerror++;
	    }
	    break;
	}
	++grabbed;
    } while (!isFinal(ch));
#if USE_PTHREADS_EINTR
    _nc_globals.read_thread = 0;
#endif

    kbuf[++grabbed] = 0;
    TR(TRACE_IEVENT,
       ("_nc_mouse_inline sees the following xterm data: '%s'", kbuf));
    return (grabbed > 0) && (result->nerror == 0);
}