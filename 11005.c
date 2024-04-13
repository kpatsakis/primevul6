d_complex_new_internal(VALUE klass,
		       VALUE nth, int jd,
		       int df, VALUE sf,
		       int of, double sg,
		       int y, int m, int d,
		       int h, int min, int s,
		       unsigned flags)
{
    struct ComplexDateData *dat;
    VALUE obj;

    obj = TypedData_Make_Struct(klass, struct ComplexDateData,
				&d_lite_type, dat);
    set_to_complex(obj, dat, nth, jd, df, sf, of, sg,
		   y, m, d, h, min, s, flags);

    assert(have_jd_p(dat) || have_civil_p(dat));
    assert(have_df_p(dat) || have_time_p(dat));

    return obj;
}