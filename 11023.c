to_integer(VALUE x)
{
    if (RB_INTEGER_TYPE_P(x))
	return x;
    return f_to_i(x);
}