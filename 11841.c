tparm(const char *string, ...)
{
    TPARM_DATA myData;
    va_list ap;
    char *result = NULL;

    _nc_tparm_err = 0;
#ifdef TRACE
    TPS(tname) = "tparm";
#endif /* TRACE */

    if (tparm_setup(string, &myData) == OK) {

	va_start(ap, string);
	tparm_copy_valist(&myData, TRUE, ap);
	va_end(ap);

	result = tparam_internal(string, &myData);
    }
    return result;
}