qf_get_next_file_line(qfstate_T *state)
{
    int	    discard;
    int	    growbuflen;

    if (fgets((char *)IObuff, IOSIZE, state->fd) == NULL)
	return QF_END_OF_INPUT;

    discard = FALSE;
    state->linelen = (int)STRLEN(IObuff);
    if (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	if (state->growbuf == NULL)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    if (state->growbuf == NULL)
		return QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;

	for (;;)
	{
	    char_u	*p;

	    if (fgets((char *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == NULL)
		break;
	    state->linelen = (int)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    if ((state->growbuf)[growbuflen - 1] == '\n')
		break;
	    if (state->growbufsiz == LINE_MAXLEN)
	    {
		discard = TRUE;
		break;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < LINE_MAXLEN
		? 2 * state->growbufsiz : LINE_MAXLEN;
	    if ((p = vim_realloc(state->growbuf, state->growbufsiz)) == NULL)
		return QF_NOMEM;
	    state->growbuf = p;
	}

	while (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    if (fgets((char *)IObuff, IOSIZE, state->fd) == NULL
		    || (int)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		break;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    else
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    if (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*line;

	line = string_convert(&state->vc, state->linebuf, &state->linelen);
	if (line != NULL)
	{
	    if (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, line);
		vim_free(line);
	    }
	    else
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = line;
		state->growbufsiz = state->linelen < LINE_MAXLEN
						? state->linelen : LINE_MAXLEN;
	    }
	}
    }

    return QF_OK;
}