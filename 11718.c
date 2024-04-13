__nc_putp(SCREEN *sp, const char *name GCC_UNUSED, const char *value)
{
    int rc = ERR;

    if (value) {
	rc = NCURSES_PUTP2(name, value);
    }
    return rc;
}