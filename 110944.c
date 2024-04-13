static void vp9_read_interpolation_filter(GF_BitStream *bs)
{
	Bool is_filter_switchable = gf_bs_read_int_log(bs, 1, "is_filter_switchable");
	if (!is_filter_switchable) {
		/*raw_interpolation_filter = */gf_bs_read_int_log(bs, 2, "raw_interpolation_filter");
	}
}