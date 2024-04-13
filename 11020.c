c_valid_start_p(double sg)
{
    if (isnan(sg))
	return 0;
    if (isinf(sg))
	return 1;
    if (sg < REFORM_BEGIN_JD || sg > REFORM_END_JD)
	return 0;
    return 1;
}