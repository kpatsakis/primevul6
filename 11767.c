tiparm(const char *string, ...)
{
    TPARM_DATA myData;
    va_list ap;
    char *result = NULL;

    _nc_tparm_err = 0;
#ifdef TRACE
    TPS(tname) = "tiparm";
#endif /* TRACE */

    if (tparm_setup(string, &myData) == OK) {

	va_start(ap, string);
	tparm_copy_valist(&myData, FALSE, ap);
	va_end(ap);

	result = tparam_internal(string, &myData);
    }
    return result;
}