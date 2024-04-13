tparm(const char *string,
      TPARM_ARG a1,
      TPARM_ARG a2,
      TPARM_ARG a3,
      TPARM_ARG a4,
      TPARM_ARG a5,
      TPARM_ARG a6,
      TPARM_ARG a7,
      TPARM_ARG a8,
      TPARM_ARG a9)
{
    TPARM_DATA myData;
    char *result = NULL;

    _nc_tparm_err = 0;
#ifdef TRACE
    TPS(tname) = "tparm";
#endif /* TRACE */

    if (tparm_setup(string, &myData) == OK) {

	myData.param[0] = a1;
	myData.param[1] = a2;
	myData.param[2] = a3;
	myData.param[3] = a4;
	myData.param[4] = a5;
	myData.param[5] = a6;
	myData.param[6] = a7;
	myData.param[7] = a8;
	myData.param[8] = a9;

	result = tparam_internal(string, &myData);
    }
    return result;
}