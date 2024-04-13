mch_errmsg_c(char *str)
{
    int	    len = (int)STRLEN(str);
    DWORD   nwrite = 0;
    DWORD   mode = 0;
    HANDLE  h = GetStdHandle(STD_ERROR_HANDLE);

    if (GetConsoleMode(h, &mode) && enc_codepage >= 0
	    && (int)GetConsoleCP() != enc_codepage)
    {
	WCHAR	*w = enc_to_utf16((char_u *)str, &len);

	WriteConsoleW(h, w, len, &nwrite, NULL);
	vim_free(w);
    }
    else
    {
	fprintf(stderr, "%s", str);
    }
}