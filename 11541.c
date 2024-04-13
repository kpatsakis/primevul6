_trace_slot(SCREEN *sp, const char *tag)
{
    MEVENT *ep;

    _tracef("%s", tag);

    for (ep = FirstEV(sp); ep <= LastEV(sp); ep++)
	_tracef("mouse event queue slot %ld = %s",
		(long) IndexEV(sp, ep),
		_nc_tracemouse(sp, ep));
}