soundfold_find(slang_T *slang, char_u *word)
{
    idx_T	arridx = 0;
    int		len;
    int		wlen = 0;
    int		c;
    char_u	*ptr = word;
    char_u	*byts;
    idx_T	*idxs;
    int		wordnr = 0;

    byts = slang->sl_sbyts;
    idxs = slang->sl_sidxs;

    for (;;)
    {
	// First byte is the number of possible bytes.
	len = byts[arridx++];

	// If the first possible byte is a zero the word could end here.
	// If the word ends we found the word.  If not skip the NUL bytes.
	c = ptr[wlen];
	if (byts[arridx] == NUL)
	{
	    if (c == NUL)
		break;

	    // Skip over the zeros, there can be several.
	    while (len > 0 && byts[arridx] == NUL)
	    {
		++arridx;
		--len;
	    }
	    if (len == 0)
		return -1;    // no children, word should have ended here
	    ++wordnr;
	}

	// If the word ends we didn't find it.
	if (c == NUL)
	    return -1;

	// Perform a binary search in the list of accepted bytes.
	if (c == TAB)	    // <Tab> is handled like <Space>
	    c = ' ';
	while (byts[arridx] < c)
	{
	    // The word count is in the first idxs[] entry of the child.
	    wordnr += idxs[idxs[arridx]];
	    ++arridx;
	    if (--len == 0)	// end of the bytes, didn't find it
		return -1;
	}
	if (byts[arridx] != c)	// didn't find the byte
	    return -1;

	// Continue at the child (if there is one).
	arridx = idxs[arridx];
	++wlen;

	// One space in the good word may stand for several spaces in the
	// checked word.
	if (c == ' ')
	    while (ptr[wlen] == ' ' || ptr[wlen] == TAB)
		++wlen;
    }

    return wordnr;
}