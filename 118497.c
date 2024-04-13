static void gf_m2ts_metadata_descriptor_del(GF_M2TS_MetadataDescriptor *metad)
{
	if (metad) {
		if (metad->service_id_record) gf_free(metad->service_id_record);
		if (metad->decoder_config) gf_free(metad->decoder_config);
		if (metad->decoder_config_id) gf_free(metad->decoder_config_id);
		gf_free(metad);
	}
}