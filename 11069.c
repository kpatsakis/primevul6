date_s_test_weeknum(VALUE klass)
{
    int f;

    for (f = 0; f <= 1; f++) {
	if (!test_weeknum(MIN_JD, MIN_JD + 366, f, GREGORIAN))
	    return Qfalse;
	if (!test_weeknum(2305814, 2598007, f, GREGORIAN))
	    return Qfalse;
	if (!test_weeknum(MAX_JD - 366, MAX_JD, f, GREGORIAN))
	    return Qfalse;

	if (!test_weeknum(MIN_JD, MIN_JD + 366, f, ITALY))
	    return Qfalse;
	if (!test_weeknum(2305814, 2598007, f, ITALY))
	    return Qfalse;
	if (!test_weeknum(MAX_JD - 366, MAX_JD, f, ITALY))
	    return Qfalse;
    }

    return Qtrue;
}