static double percentage(i64 num, i64 den)
{
	double d_num, d_den;

	if (den < 100) {
		d_num = num * 100;
		d_den = den;
		if (!d_den)
			d_den = 1;
	} else {
		d_num = num;
		d_den = den / 100;
	}
	return d_num / d_den;
}