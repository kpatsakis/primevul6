keypad_final(const char *string)
{
    char result = '\0';

    if (VALID_STRING(string)
	&& *string++ == '\033'
	&& *string++ == 'O'
	&& strlen(string) == 1) {
	result = *string;
    }

    return result;
}