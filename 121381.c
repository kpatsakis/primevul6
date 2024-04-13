EmitButtonCode(XtermWidget xw,
	       Char *line,
	       unsigned count,
	       XButtonEvent *event,
	       int button)
{
    TScreen *screen = TScreenOf(xw);
    int value;

    if (okSendMousePos(xw) == X10_MOUSE) {
	value = CharOf(' ' + button);
    } else {
	value = BtnCode(xw, event, button);
    }

    switch (screen->extend_coords) {
    default:
	line[count++] = CharOf(value);
	break;
    case SET_SGR_EXT_MODE_MOUSE:
    case SET_PIXEL_POSITION_MOUSE:
	value -= 32;		/* encoding starts at zero */
	/* FALLTHRU */
    case SET_URXVT_EXT_MODE_MOUSE:
	count += (unsigned) sprintf((char *) line + count, "%d", value);
	break;
    case SET_EXT_MODE_MOUSE:
	if (value < 128) {
	    line[count++] = CharOf(value);
	} else {
	    line[count++] = CharOf(0xC0 + (value >> 6));
	    line[count++] = CharOf(0x80 + (value & 0x3F));
	}
	break;
    }
    return count;
}