equal_gen(VALUE self, VALUE other)
{
    get_d1(self);

    if (k_numeric_p(other))
	return f_eqeq_p(m_real_local_jd(dat), other);
    else if (k_date_p(other))
	return f_eqeq_p(m_real_local_jd(dat), f_jd(other));
    return rb_num_coerce_cmp(self, other, id_eqeq_p);
}