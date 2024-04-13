parseItem(char *value, char *nextc)
{
    char *nextp = value;
    while (*nextp != '\0' && *nextp != ',') {
	*nextp = x_toupper(*nextp);
	++nextp;
    }
    *nextc = *nextp;
    *nextp = '\0';

    return nextp;
}