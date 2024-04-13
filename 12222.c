qf_grow_linebuf(qfstate_T *state, int newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > LINE_MAXLEN ? LINE_MAXLEN - 1 : newsz;
    if (state->growbuf == NULL)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	if (state->growbuf == NULL)
	    return NULL;
	state->growbufsiz = state->linelen;
    }
    else if (state->linelen > state->growbufsiz)
    {
	if ((p = vim_realloc(state->growbuf, state->linelen + 1)) == NULL)
	    return NULL;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    return state->growbuf;
}