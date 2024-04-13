validate(const char *name)
{
    char *value = tigetstr(name);
    if (!VALID_STRING(value)) {
	if (v_opt > 1) {
	    printf("? %s %s\n",
		   (value == ABSENT_STRING)
		   ? "absent"
		   : "cancel",
		   name);
	}
	value = 0;
    }
    return value;
}