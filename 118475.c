static void gf_m2ts_process_mpeg4section(GF_M2TS_Demuxer *ts, GF_M2TS_SECTION_ES *es, GF_List *sections, u8 table_id, u16 ex_table_id, u8 version_number, u8 last_section_number, u32 status)
{
	GF_M2TS_SL_PCK sl_pck;
	u32 nb_sections, i;
	GF_M2TS_Section *section;

	/*skip if already received*/
	if (status & GF_M2TS_TABLE_REPEAT)
		if (!(es->flags & GF_M2TS_ES_SEND_REPEATED_SECTIONS))
			return;

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] Sections for PID %d\n", es->pid) );
	/*send all sections (eg SL-packets)*/
	nb_sections = gf_list_count(sections);
	for (i=0; i<nb_sections; i++) {
		section = (GF_M2TS_Section *)gf_list_get(sections, i);
		sl_pck.data = (char *)section->data;
		sl_pck.data_len = section->data_size;
		sl_pck.stream = (GF_M2TS_ES *)es;
		sl_pck.version_number = version_number;
		if (ts->on_event) ts->on_event(ts, GF_M2TS_EVT_SL_PCK, &sl_pck);
	}
}