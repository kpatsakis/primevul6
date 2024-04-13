d_lite_fill(VALUE self)
{
    get_d1(self);

    if (simple_dat_p(dat)) {
	get_s_jd(dat);
	get_s_civil(dat);
    }
    else {
	get_c_jd(dat);
	get_c_civil(dat);
	get_c_df(dat);
	get_c_time(dat);
    }
    return self;
}