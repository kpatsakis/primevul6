set_window_size(int fd, short *high, short *wide)
{
    STRUCT_WINSIZE win;
    (void) ioctl(fd, IOCTL_GET_WINSIZE, &win);
    if (WINSIZE_ROWS(win) == 0 &&
	WINSIZE_COLS(win) == 0) {
	if (*high > 0 && *wide > 0) {
	    WINSIZE_ROWS(win) = (unsigned short) *high;
	    WINSIZE_COLS(win) = (unsigned short) *wide;
	    (void) ioctl(fd, IOCTL_SET_WINSIZE, &win);
	}
    } else if (WINSIZE_ROWS(win) > 0 &&
	       WINSIZE_COLS(win) > 0) {
	*high = (short) WINSIZE_ROWS(win);
	*wide = (short) WINSIZE_COLS(win);
    }
}