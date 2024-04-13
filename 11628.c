skip_DECSCNM(const char *value, int *flag)
{
    *flag = -1;
    if (value != 0) {
	int skip = csi_length(value);
	if (skip > 0 &&
	    value[skip++] == '?' &&
	    value[skip++] == '5') {
	    if (value[skip] == 'h') {
		*flag = 1;
	    } else if (value[skip] == 'l') {
		*flag = 0;
	    }
	    value += skip + 1;
	}
    }
    return value;
}