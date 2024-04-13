sent_string(const char *s)
{
    bool sent = FALSE;
    if (VALID_STRING(s)) {
	tputs(s, 0, out_char);
	sent = TRUE;
    }
    return sent;
}