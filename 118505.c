void gf_m2ts_demux_del(GF_M2TS_Demuxer *ts)
{
	u32 i;
	if (ts->pat) gf_m2ts_section_filter_del(ts->pat);
	if (ts->cat) gf_m2ts_section_filter_del(ts->cat);
	if (ts->sdt) gf_m2ts_section_filter_del(ts->sdt);
	if (ts->nit) gf_m2ts_section_filter_del(ts->nit);
	if (ts->eit) gf_m2ts_section_filter_del(ts->eit);
	if (ts->tdt_tot) gf_m2ts_section_filter_del(ts->tdt_tot);

	for (i=0; i<GF_M2TS_MAX_STREAMS; i++) {
		//bacause of pure PCR streams, en ES might be reassigned on 2 PIDs, one for the ES and one for the PCR
		if (ts->ess[i] && (ts->ess[i]->pid==i)) gf_m2ts_es_del(ts->ess[i], ts);
	}
	if (ts->buffer) gf_free(ts->buffer);
	while (gf_list_count(ts->programs)) {
		GF_M2TS_Program *p = (GF_M2TS_Program *)gf_list_last(ts->programs);
		gf_list_rem_last(ts->programs);
		gf_list_del(p->streams);
		/*reset OD list*/
		if (p->additional_ods) {
			gf_odf_desc_list_del(p->additional_ods);
			gf_list_del(p->additional_ods);
		}
		if (p->pmt_iod) gf_odf_desc_del((GF_Descriptor *)p->pmt_iod);
		if (p->metadata_pointer_descriptor)	gf_m2ts_metadata_pointer_descriptor_del(p->metadata_pointer_descriptor);
		gf_free(p);
	}
	gf_list_del(ts->programs);

	if (ts->TDT_time) gf_free(ts->TDT_time);
	gf_m2ts_reset_sdt(ts);
	if (ts->tdt_tot)
		gf_list_del(ts->SDTs);

#ifdef GPAC_ENABLE_MPE
	gf_dvb_mpe_shutdown(ts);
#endif

	if (ts->dsmcc_controler) {
		if (gf_list_count(ts->dsmcc_controler)) {
#ifdef GPAC_ENABLE_DSMCC
			GF_M2TS_DSMCC_OVERLORD* dsmcc_overlord = (GF_M2TS_DSMCC_OVERLORD*)gf_list_get(ts->dsmcc_controler,0);
			gf_cleanup_dir(dsmcc_overlord->root_dir);
			gf_rmdir(dsmcc_overlord->root_dir);
			gf_m2ts_delete_dsmcc_overlord(dsmcc_overlord);
			if(ts->dsmcc_root_dir) {
				gf_free(ts->dsmcc_root_dir);
			}
#endif
		}
		gf_list_del(ts->dsmcc_controler);
	}

	while(gf_list_count(ts->ChannelAppList)) {
#ifdef GPAC_ENABLE_DSMCC
		GF_M2TS_CHANNEL_APPLICATION_INFO* ChanAppInfo = (GF_M2TS_CHANNEL_APPLICATION_INFO*)gf_list_get(ts->ChannelAppList,0);
		gf_m2ts_delete_channel_application_info(ChanAppInfo);
		gf_list_rem(ts->ChannelAppList,0);
#endif
	}
	gf_list_del(ts->ChannelAppList);

	if (ts->dsmcc_root_dir) gf_free(ts->dsmcc_root_dir);
	gf_free(ts);
}