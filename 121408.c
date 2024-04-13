ToNational(XtermWidget xw, Char *buffer, unsigned *length)
{
    TScreen *screen = TScreenOf(xw);
    DECNRCM_codes gsetL = screen->gsets[screen->curgl];
    DECNRCM_codes gsetR = screen->gsets[screen->curgr];

#if OPT_WIDE_CHARS
    if ((screen->utf8_nrc_mode | screen->utf8_mode) != uFalse) {
	Char *p;
	PtyData *data = TypeXtMallocX(PtyData, *length);

	memset(data, 0, sizeof(*data));
	data->next = data->buffer;
	data->last = data->buffer + *length;
	memcpy(data->buffer, buffer, (size_t) *length);
	p = buffer;
	while (data->next < data->last) {
	    unsigned chr, out, gl, gr;

	    if (!decodeUtf8(screen, data)) {
		data->utf_size = 1;
		data->utf_data = data->next[0];
	    }
	    data->next += data->utf_size;
	    chr = data->utf_data;
	    out = chr;
	    if ((gl = xtermCharSetIn(xw, chr, gsetL)) != chr) {
		out = gl;
	    } else if ((gr = xtermCharSetIn(xw, chr, gsetR)) != chr) {
		out = gr;
	    }
	    *p++ = (Char) ((out < 256) ? out : ' ');
	}
	*length = (unsigned) (p - buffer);
	free(data);
    } else
#endif
    {
	Char *p;

	for (p = buffer; (int) (p - buffer) < (int) *length; ++p) {
	    unsigned gl, gr;
	    unsigned chr = *p;
	    unsigned out = chr;
	    if ((gl = xtermCharSetIn(xw, chr, gsetL)) != chr) {
		out = gl;
	    } else if ((gr = xtermCharSetIn(xw, chr, gsetR)) != chr) {
		out = gr;
	    }
	    *p = (Char) out;
	}
    }
}