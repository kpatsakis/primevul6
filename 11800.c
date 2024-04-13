safe_strdup(const char *value)
{
    if (value == NULL)
	value = "";
    return strdup(value);
}