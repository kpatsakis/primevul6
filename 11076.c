d_lite_eql_p(VALUE self, VALUE other)
{
    if (!k_date_p(other))
	return Qfalse;
    return f_zero_p(d_lite_cmp(self, other));
}