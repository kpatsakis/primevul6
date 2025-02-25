ImagingPcdDecode(Imaging im, ImagingCodecState state, UINT8* buf, int bytes)
{
    int x;
    int chunk;
    UINT8* out;
    UINT8* ptr;

    ptr = buf;

    chunk = 3 * state->xsize;

    for (;;) {

	/* We need data for two full lines before we can do anything */
	if (bytes < chunk)
	    return ptr - buf;

	/* Unpack first line */
	out = state->buffer;
	for (x = 0; x < state->xsize; x++) {
	    out[0] = ptr[x];
	    out[1] = ptr[(x+4*state->xsize)/2];
	    out[2] = ptr[(x+5*state->xsize)/2];
	    out += 3;
	}

	state->shuffle((UINT8*) im->image[state->y],
		       state->buffer, state->xsize);

	if (++state->y >= state->ysize)
	    return -1; /* This can hardly happen */

	/* Unpack second line */
	out = state->buffer;
	for (x = 0; x < state->xsize; x++) {
	    out[0] = ptr[x+state->xsize];
	    out[1] = ptr[(x+4*state->xsize)/2];
	    out[2] = ptr[(x+5*state->xsize)/2];
	    out += 3;
	}

	state->shuffle((UINT8*) im->image[state->y],
		       state->buffer, state->xsize);

	if (++state->y >= state->ysize)
	    return -1;

	ptr += chunk;
	bytes -= chunk;

    }
}