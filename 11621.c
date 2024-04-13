drv_read(TERMINAL_CONTROL_BLOCK * TCB, int *buf)
{
    SCREEN *sp;
    unsigned char c2 = 0;
    int n;

    AssertTCB();
    assert(buf);
    SetSP();

# if USE_PTHREADS_EINTR
    if ((pthread_self) && (pthread_kill) && (pthread_equal))
	_nc_globals.read_thread = pthread_self();
# endif
    n = (int) read(sp->_ifd, &c2, (size_t) 1);
#if USE_PTHREADS_EINTR
    _nc_globals.read_thread = 0;
#endif
    *buf = (int) c2;
    return n;
}