default_erase(void)
{
    int result;

    if (over_strike
	&& VALID_STRING(key_backspace)
	&& strlen(key_backspace) == 1) {
	result = key_backspace[0];
    } else {
	result = CERASE;
    }

    return result;
}