jd_to_commercial(VALUE jd, double sg,
		 VALUE *nth, int *rjd,
		 int *ry, int *rw, int *rd)
{
    decode_jd(jd, nth, rjd);
    c_jd_to_commercial(*rjd, sg, ry, rw, rd);
}