
GF_Err dvcC_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox *)s;

	//GF_DOVIDecoderConfigurationRecord
	ptr->DOVIConfig.dv_version_major = gf_bs_read_u8(bs);
	ptr->DOVIConfig.dv_version_minor = gf_bs_read_u8(bs);
	ptr->DOVIConfig.dv_profile = gf_bs_read_int(bs, 7);
	ptr->DOVIConfig.dv_level = gf_bs_read_int(bs, 6);
	ptr->DOVIConfig.rpu_present_flag = gf_bs_read_int(bs, 1);
	ptr->DOVIConfig.el_present_flag = gf_bs_read_int(bs, 1);
	ptr->DOVIConfig.bl_present_flag = gf_bs_read_int(bs, 1);
	{
		int i = 0;
		u32 data[5];
		memset(data, 0, sizeof(data));
		gf_bs_read_data(bs, (char*)data, 20);
		for (i = 0; i < 5; ++i) {
			if (data[i] != 0) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] dvcC reserved bytes are not zero\n"));
				//return GF_ISOM_INVALID_FILE;
			}
		}
	}

	return GF_OK;