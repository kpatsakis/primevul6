test_unit_v2v(VALUE i,
	      VALUE (* conv1)(VALUE),
	      VALUE (* conv2)(VALUE))
{
    VALUE c, o;
    c = (*conv1)(i);
    o = (*conv2)(c);
    return f_eqeq_p(o, i);
}