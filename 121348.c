_qWriteSelectionData(XtermWidget xw, Char *lag, unsigned length)
{
    TScreen *screen = TScreenOf(xw);

    /*
     * If we are pasting into a window which is using NRCS, we want to map
     * the text from the normal encoding (ISO-8859-1 or UTF-8) into the coding
     * that an application would use to write characters with NRCS.
     *
     * TODO: handle conversion from UTF-8, and adjust length.  This can be done
     * in the same buffer because the target is always 8-bit.
     */
    if ((xw->flags & NATIONAL) && (length != 0)) {
	ToNational(xw, lag, &length);
    }
#if OPT_PASTE64
    if (screen->base64_paste) {
	/* Send data as base64 */
	Char *p = lag;
	Char buf[64];
	unsigned x = 0;

	TRACE(("convert to base64 %d:%s\n", length, visibleChars(p, length)));

	/*
	 * Handle the case where the selection is from _this_ xterm, which
	 * puts part of the reply in the buffer before the selection callback
	 * happens.
	 */
	if (screen->base64_paste && screen->unparse_len) {
	    unparse_end(xw);
	}
	while (length--) {
	    switch (screen->base64_count) {
	    case 0:
		buf[x++] = CharOf(base64_code[*p >> 2]);
		screen->base64_accu = (unsigned) (*p & 0x3);
		screen->base64_count = 2;
		++p;
		break;
	    case 2:
		buf[x++] = CharOf(base64_code[(screen->base64_accu << 4) +
					      (*p >> 4)]);
		screen->base64_accu = (unsigned) (*p & 0xF);
		screen->base64_count = 4;
		++p;
		break;
	    case 4:
		buf[x++] = CharOf(base64_code[(screen->base64_accu << 2) +
					      (*p >> 6)]);
		buf[x++] = CharOf(base64_code[*p & 0x3F]);
		screen->base64_accu = 0;
		screen->base64_count = 0;
		++p;
		break;
	    }
	    if (x >= 63) {
		/* Write 63 or 64 characters */
		screen->base64_pad += x;
		TRACE(("writing base64 interim %s\n", visibleChars(buf, x)));
		tty_vwrite(screen->respond, buf, x);
		x = 0;
	    }
	}
	if (x != 0) {
	    screen->base64_pad += x;
	    TRACE(("writing base64 finish %s\n", visibleChars(buf, x)));
	    tty_vwrite(screen->respond, buf, x);
	}
    } else
#endif /* OPT_PASTE64 */
#if OPT_READLINE
    if (SCREEN_FLAG(screen, paste_quotes)) {
	while (length--) {
	    tty_vwrite(screen->respond, (const Char *) "\026", 1);	/* Control-V */
	    tty_vwrite(screen->respond, lag++, 1);
	}
    } else
#endif
    {
	TRACE(("writing base64 padding %s\n", visibleChars(lag, length)));
	tty_vwrite(screen->respond, lag, length);
    }
}