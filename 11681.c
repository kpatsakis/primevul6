csi_length(const char *value)
{
    int result = 0;

    if (value[0] == '\033' && value[1] == '[') {
	result = 2;
    } else if (UChar(value[0]) == 0x9a) {
	result = 1;
    }
    return result;
}