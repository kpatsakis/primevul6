static void gf_m2ts_section_filter_reset(GF_M2TS_SectionFilter *sf)
{
	if (sf->section) {
		gf_free(sf->section);
		sf->section = NULL;
	}
	while (sf->table) {
		GF_M2TS_Table *t = sf->table;
		sf->table = t->next;
		gf_m2ts_reset_sections(t->sections);
		gf_list_del(t->sections);
		gf_free(t);
	}
	sf->cc = -1;
	sf->length = sf->received = 0;
	sf->demux_restarted = 1;

}