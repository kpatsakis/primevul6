void gf_m2ts_es_del(GF_M2TS_ES *es, GF_M2TS_Demuxer *ts)
{
	gf_list_del_item(es->program->streams, es);

	if (es->flags & GF_M2TS_ES_IS_SECTION) {
		GF_M2TS_SECTION_ES *ses = (GF_M2TS_SECTION_ES *)es;
		if (ses->sec) gf_m2ts_section_filter_del(ses->sec);

#ifdef GPAC_ENABLE_MPE
		if (es->flags & GF_M2TS_ES_IS_MPE)
			gf_dvb_mpe_section_del(es);
#endif

	} else if (es->pid!=es->program->pmt_pid) {
		GF_M2TS_PES *pes = (GF_M2TS_PES *)es;

		if ((pes->flags & GF_M2TS_INHERIT_PCR) && ts->ess[es->program->pcr_pid]==es)
			ts->ess[es->program->pcr_pid] = NULL;

		if (pes->pck_data) gf_free(pes->pck_data);
		if (pes->prev_data) gf_free(pes->prev_data);
		if (pes->buf) gf_free(pes->buf);
		if (pes->reassemble_buf) gf_free(pes->reassemble_buf);
		if (pes->temi_tc_desc) gf_free(pes->temi_tc_desc);

		if (pes->metadata_descriptor) gf_m2ts_metadata_descriptor_del(pes->metadata_descriptor);

	}
	if (es->slcfg) gf_free(es->slcfg);
	gf_free(es);
}