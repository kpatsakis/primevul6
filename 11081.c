rt__valid_date_frags_p(VALUE hash, VALUE sg)
{
    {
	VALUE vjd;

	if (!NIL_P(vjd = ref_hash("jd"))) {
	    VALUE jd = rt__valid_jd_p(vjd, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, yday;

	if (!NIL_P(yday = ref_hash("yday")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_ordinal_p(year, yday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, mon, mday;

	if (!NIL_P(mday = ref_hash("mday")) &&
	    !NIL_P(mon = ref_hash("mon")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_civil_p(year, mon, mday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("cwday");
	if (NIL_P(wday)) {
	    wday = ref_hash("wday");
	    if (!NIL_P(wday))
		if (f_zero_p(wday))
		    wday = INT2FIX(7);
	}

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("cweek")) &&
	    !NIL_P(year = ref_hash("cwyear"))) {
	    VALUE jd = rt__valid_commercial_p(year, week, wday, sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("wday");
	if (NIL_P(wday)) {
	    wday = ref_hash("cwday");
	    if (!NIL_P(wday))
		if (f_eqeq_p(wday, INT2FIX(7)))
		    wday = INT2FIX(0);
	}

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("wnum0")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_weeknum_p(year, week, wday, INT2FIX(0), sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }

    {
	VALUE year, week, wday;

	wday = ref_hash("wday");
	if (NIL_P(wday))
	    wday = ref_hash("cwday");
	if (!NIL_P(wday))
	    wday = f_mod(f_sub(wday, INT2FIX(1)),
			 INT2FIX(7));

	if (!NIL_P(wday) &&
	    !NIL_P(week = ref_hash("wnum1")) &&
	    !NIL_P(year = ref_hash("year"))) {
	    VALUE jd = rt__valid_weeknum_p(year, week, wday, INT2FIX(1), sg);
	    if (!NIL_P(jd))
		return jd;
	}
    }
    return Qnil;
}