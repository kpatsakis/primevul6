rt_complete_frags(VALUE klass, VALUE hash)
{
    static VALUE tab = Qnil;
    int g;
    long e;
    VALUE k, a, d;

    if (NIL_P(tab)) {
	tab = f_frozen_ary(11,
			  f_frozen_ary(2,
				      sym("time"),
				      f_frozen_ary(3,
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      Qnil,
				      f_frozen_ary(1,
						  sym("jd"))),
			  f_frozen_ary(2,
				      sym("ordinal"),
				      f_frozen_ary(5,
						  sym("year"),
						  sym("yday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      sym("civil"),
				      f_frozen_ary(6,
						  sym("year"),
						  sym("mon"),
						  sym("mday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      sym("commercial"),
				      f_frozen_ary(6,
						  sym("cwyear"),
						  sym("cweek"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      sym("wday"),
				      f_frozen_ary(4,
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      sym("wnum0"),
				      f_frozen_ary(6,
						  sym("year"),
						  sym("wnum0"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      sym("wnum1"),
				      f_frozen_ary(6,
						  sym("year"),
						  sym("wnum1"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      Qnil,
				      f_frozen_ary(6,
						  sym("cwyear"),
						  sym("cweek"),
						  sym("wday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      Qnil,
				      f_frozen_ary(6,
						  sym("year"),
						  sym("wnum0"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))),
			  f_frozen_ary(2,
				      Qnil,
				      f_frozen_ary(6,
						  sym("year"),
						  sym("wnum1"),
						  sym("cwday"),
						  sym("hour"),
						  sym("min"),
						  sym("sec"))));
	rb_gc_register_mark_object(tab);
    }

    {
	long i, eno = 0, idx = 0;

	for (i = 0; i < RARRAY_LEN(tab); i++) {
	    VALUE x, a;

	    x = RARRAY_AREF(tab, i);
	    a = RARRAY_AREF(x, 1);

	    {
		long j, n = 0;

		for (j = 0; j < RARRAY_LEN(a); j++)
		    if (!NIL_P(ref_hash0(RARRAY_AREF(a, j))))
			n++;
		if (n > eno) {
		    eno = n;
		    idx = i;
		}
	    }
	}
	if (eno == 0)
	    g = 0;
	else {
	    g = 1;
	    k = RARRAY_AREF(RARRAY_AREF(tab, idx), 0);
	    a = RARRAY_AREF(RARRAY_AREF(tab, idx), 1);
	    e =	eno;
	}
    }

    d = Qnil;

    if (g && !NIL_P(k) && (RARRAY_LEN(a) - e)) {
	if (k == sym("ordinal")) {
	    if (NIL_P(ref_hash("year"))) {
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash("year", d_lite_year(d));
	    }
	    if (NIL_P(ref_hash("yday")))
		set_hash("yday", INT2FIX(1));
	}
	else if (k == sym("civil")) {
	    long i;

	    for (i = 0; i < RARRAY_LEN(a); i++) {
		VALUE e = RARRAY_AREF(a, i);

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("mon")))
		set_hash("mon", INT2FIX(1));
	    if (NIL_P(ref_hash("mday")))
		set_hash("mday", INT2FIX(1));
	}
	else if (k == sym("commercial")) {
	    long i;

	    for (i = 0; i < RARRAY_LEN(a); i++) {
		VALUE e = RARRAY_AREF(a, i);

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("cweek")))
		set_hash("cweek", INT2FIX(1));
	    if (NIL_P(ref_hash("cwday")))
		set_hash("cwday", INT2FIX(1));
	}
	else if (k == sym("wday")) {
	    if (NIL_P(d))
		d = date_s_today(0, (VALUE *)0, cDate);
	    set_hash("jd", d_lite_jd(f_add(f_sub(d,
						 d_lite_wday(d)),
					   ref_hash("wday"))));
	}
	else if (k == sym("wnum0")) {
	    long i;

	    for (i = 0; i < RARRAY_LEN(a); i++) {
		VALUE e = RARRAY_AREF(a, i);

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("wnum0")))
		set_hash("wnum0", INT2FIX(0));
	    if (NIL_P(ref_hash("wday")))
		set_hash("wday", INT2FIX(0));
	}
	else if (k == sym("wnum1")) {
	    long i;

	    for (i = 0; i < RARRAY_LEN(a); i++) {
		VALUE e = RARRAY_AREF(a, i);

		if (!NIL_P(ref_hash0(e)))
		    break;
		if (NIL_P(d))
		    d = date_s_today(0, (VALUE *)0, cDate);
		set_hash0(e, rb_funcall(d, SYM2ID(e), 0));
	    }
	    if (NIL_P(ref_hash("wnum1")))
		set_hash("wnum1", INT2FIX(0));
	    if (NIL_P(ref_hash("wday")))
		set_hash("wday", INT2FIX(1));
	}
    }

    if (g && k == sym("time")) {
	if (f_le_p(klass, cDateTime)) {
	    if (NIL_P(d))
		d = date_s_today(0, (VALUE *)0, cDate);
	    if (NIL_P(ref_hash("jd")))
		set_hash("jd", d_lite_jd(d));
	}
    }

    if (NIL_P(ref_hash("hour")))
	set_hash("hour", INT2FIX(0));
    if (NIL_P(ref_hash("min")))
	set_hash("min", INT2FIX(0));
    if (NIL_P(ref_hash("sec")))
	set_hash("sec", INT2FIX(0));
    else if (f_gt_p(ref_hash("sec"), INT2FIX(59)))
	set_hash("sec", INT2FIX(59));

    return hash;
}