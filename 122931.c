GF_Err gf_isom_vvc_config_update(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_VVCConfig *cfg)
{
	return gf_isom_vvc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, GF_ISOM_VVCC_UPDATE, GF_FALSE);
}