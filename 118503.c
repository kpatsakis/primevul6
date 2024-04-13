static GF_M2TS_MetadataDescriptor *gf_m2ts_read_metadata_descriptor(GF_BitStream *bs, u32 length)
{
	u32 size;
	GF_M2TS_MetadataDescriptor *d;
	GF_SAFEALLOC(d, GF_M2TS_MetadataDescriptor);
	if (!d) return NULL;
	d->application_format = gf_bs_read_u16(bs);
	size = 2;
	if (d->application_format == 0xFFFF) {
		d->application_format_identifier = gf_bs_read_u32(bs);
		size += 4;
	}
	d->format = gf_bs_read_u8(bs);
	size += 1;
	if (d->format == 0xFF) {
		d->format_identifier = gf_bs_read_u32(bs);
		size += 4;
	}
	d->service_id = gf_bs_read_u8(bs);
	d->decoder_config_flags = gf_bs_read_int(bs, 3);
	d->dsmcc_flag = (gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE);
	gf_bs_read_int(bs, 4); /* reserved */
	size += 2;
	if (d->dsmcc_flag) {
		d->service_id_record_length = gf_bs_read_u8(bs);
		d->service_id_record = (char *)gf_malloc(d->service_id_record_length);
		size += 1 + d->service_id_record_length;
		gf_bs_read_data(bs, d->service_id_record, d->service_id_record_length);
	}
	if (d->decoder_config_flags == 1) {
		d->decoder_config_length = gf_bs_read_u8(bs);
		d->decoder_config = (char *)gf_malloc(d->decoder_config_length);
		size += 1 + d->decoder_config_length;
		gf_bs_read_data(bs, d->decoder_config, d->decoder_config_length);
	}
	if (d->decoder_config_flags == 3) {
		d->decoder_config_id_length = gf_bs_read_u8(bs);
		d->decoder_config_id = (char *)gf_malloc(d->decoder_config_id_length);
		size += 1 + d->decoder_config_id_length;
		gf_bs_read_data(bs, d->decoder_config_id, d->decoder_config_id_length);
	}
	if (d->decoder_config_flags == 4) {
		d->decoder_config_service_id = gf_bs_read_u8(bs);
		size++;
	}
	return d;
}