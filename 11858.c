uses_SGR_39_49(const char *value)
{
    return (strstr(value, "39;49") != 0
	    || strstr(value, "49;39") != 0);
}