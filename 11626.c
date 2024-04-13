skip_list(char *value)
{
    while (*value != '\0' &&
	   (isdigit(UChar(*value)) ||
	    isspace(UChar(*value)) ||
	    strchr("+,", UChar(*value)) != 0)) {
	++value;
    }
    return value;
}