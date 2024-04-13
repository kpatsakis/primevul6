is_csi(const char *s)
{
    int result = 0;
    if (s != 0) {
	if (UChar(s[0]) == CSI)
	    result = 1;
	else if (s[0] == ESC && s[1] == L_BRACK)
	    result = 2;
    }
    return result;
}