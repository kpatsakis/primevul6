__nc_putp_flush(SCREEN *sp, const char *name, const char *value)
{
    int rc = __nc_putp(sp, name, value);
    if (rc != ERR) {
	NCURSES_SP_NAME(_nc_flush) (sp);
    }
    return rc;
}