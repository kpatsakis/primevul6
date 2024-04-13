static void gf_m2ts_section_filter_del(GF_M2TS_SectionFilter *sf)
{
	gf_m2ts_section_filter_reset(sf);
	gf_free(sf);
}