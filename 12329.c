find_is_eval_item(
    char_u	*ptr,
    int		*colp,
    int		*bnp,
    int		dir)
{
    // Accept everything inside [].
    if ((*ptr == ']' && dir == BACKWARD) || (*ptr == '[' && dir == FORWARD))
	++*bnp;
    if (*bnp > 0)
    {
	if ((*ptr == '[' && dir == BACKWARD) || (*ptr == ']' && dir == FORWARD))
	    --*bnp;
	return TRUE;
    }

    // skip over "s.var"
    if (*ptr == '.')
	return TRUE;

    // two-character item: s->var
    if (ptr[dir == BACKWARD ? 0 : 1] == '>'
	    && ptr[dir == BACKWARD ? -1 : 0] == '-')
    {
	*colp += dir;
	return TRUE;
    }
    return FALSE;
}