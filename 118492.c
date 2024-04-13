static GF_M2TS_MetadataPointerDescriptor *gf_m2ts_read_metadata_pointer_descriptor(GF_BitStream *bs, u32 length)
{
	u32 size;
	GF_M2TS_MetadataPointerDescriptor *d;
	GF_SAFEALLOC(d, GF_M2TS_MetadataPointerDescriptor);
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
	d->locator_record_flag = (gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE);
	d->carriage_flag = (enum metadata_carriage)gf_bs_read_int(bs, 2);
	gf_bs_read_int(bs, 5); /*reserved */
	size += 2;
	if (d->locator_record_flag) {
		d->locator_length = gf_bs_read_u8(bs);
		d->locator_data = (char *)gf_malloc(d->locator_length);
		size += 1 + d->locator_length;
		gf_bs_read_data(bs, d->locator_data, d->locator_length);
	}
	if (d->carriage_flag != 3) {
		d->program_number = gf_bs_read_u16(bs);
		size += 2;
	}
	if (d->carriage_flag == 1) {
		d->ts_location = gf_bs_read_u16(bs);
		d->ts_id = gf_bs_read_u16(bs);
		size += 4;
	}
	if (length-size > 0) {
		d->data_size = length-size;
		d->data = (char *)gf_malloc(d->data_size);
		gf_bs_read_data(bs, d->data, d->data_size);
	}
	return d;
}