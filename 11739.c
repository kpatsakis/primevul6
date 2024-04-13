outs(const char *s)
{
    if (VALID_STRING(s)) {
	tputs(s, 1, outc);
	return TRUE;
    }
    return FALSE;
}