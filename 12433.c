find_command(int cmdchar)
{
    int		i;
    int		idx;
    int		top, bot;
    int		c;

    // A multi-byte character is never a command.
    if (cmdchar >= 0x100)
	return -1;

    // We use the absolute value of the character.  Special keys have a
    // negative value, but are sorted on their absolute value.
    if (cmdchar < 0)
	cmdchar = -cmdchar;

    // If the character is in the first part: The character is the index into
    // nv_cmd_idx[].
    if (cmdchar <= nv_max_linear)
	return nv_cmd_idx[cmdchar];

    // Perform a binary search.
    bot = nv_max_linear + 1;
    top = NV_CMDS_SIZE - 1;
    idx = -1;
    while (bot <= top)
    {
	i = (top + bot) / 2;
	c = nv_cmds[nv_cmd_idx[i]].cmd_char;
	if (c < 0)
	    c = -c;
	if (cmdchar == c)
	{
	    idx = nv_cmd_idx[i];
	    break;
	}
	if (cmdchar > c)
	    bot = i + 1;
	else
	    top = i - 1;
    }
    return idx;
}