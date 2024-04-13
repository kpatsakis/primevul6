static GF_M2TS_SectionFilter *gf_m2ts_section_filter_new(gf_m2ts_section_callback process_section_callback, Bool process_individual)
{
	GF_M2TS_SectionFilter *sec;
	GF_SAFEALLOC(sec, GF_M2TS_SectionFilter);
	if (!sec) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] gf_m2ts_section_filter_new : OUT OF MEMORY\n"));
		return NULL;
	}
	sec->cc = -1;
	sec->process_section = process_section_callback;
	sec->process_individual = process_individual;
	return sec;
}