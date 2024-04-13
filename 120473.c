#ifndef GPAC_DISABLE_ISOM_WRITE
GF_Err dvcC_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	//GF_DOVIDecoderConfigurationRecord
	gf_bs_write_u8(bs,  ptr->DOVIConfig.dv_version_major);
	gf_bs_write_u8(bs,  ptr->DOVIConfig.dv_version_minor);
	gf_bs_write_int(bs, ptr->DOVIConfig.dv_profile, 7);
	gf_bs_write_int(bs, ptr->DOVIConfig.dv_level, 6);
	gf_bs_write_int(bs, ptr->DOVIConfig.rpu_present_flag, 1);
	gf_bs_write_int(bs, ptr->DOVIConfig.el_present_flag, 1);
	gf_bs_write_int(bs, ptr->DOVIConfig.bl_present_flag, 1);
	{
		u32 data[5];
		memset(data, 0, sizeof(data));
		gf_bs_write_data(bs, (char*)data, sizeof(data));
	}

	return GF_OK;