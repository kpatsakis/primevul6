
void sgpd_write_entry(u32 grouping_type, void *entry, GF_BitStream *bs)
{
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
		gf_bs_write_int(bs, ((GF_RollRecoveryEntry*)entry)->roll_distance, 16);
		return;
	case GF_ISOM_SAMPLE_GROUP_RAP:
		gf_bs_write_int(bs, ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples_known, 1);
		gf_bs_write_int(bs, ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples, 7);
		return;
	case GF_ISOM_SAMPLE_GROUP_SAP:
		gf_bs_write_int(bs, ((GF_SAPEntry*)entry)->dependent_flag, 1);
		gf_bs_write_int(bs, 0, 3);
		gf_bs_write_int(bs, ((GF_SAPEntry*)entry)->SAP_type, 4);
		return;
	case GF_ISOM_SAMPLE_GROUP_SYNC:
		gf_bs_write_int(bs, 0, 2);
		gf_bs_write_int(bs, ((GF_SYNCEntry*)entry)->NALU_type, 6);
		return;
	case GF_ISOM_SAMPLE_GROUP_TELE:
		gf_bs_write_int(bs, ((GF_TemporalLevelEntry*)entry)->level_independently_decodable, 1);
		gf_bs_write_int(bs, 0, 7);
		return;
	case GF_ISOM_SAMPLE_GROUP_SEIG:
		gf_bs_write_u8(bs, 0x0);
		gf_bs_write_int(bs, ((GF_CENCSampleEncryptionGroupEntry*)entry)->crypt_byte_block, 4);
		gf_bs_write_int(bs, ((GF_CENCSampleEncryptionGroupEntry*)entry)->skip_byte_block, 4);
		gf_bs_write_u8(bs, ((GF_CENCSampleEncryptionGroupEntry *)entry)->IsProtected);
		gf_bs_write_u8(bs, ((GF_CENCSampleEncryptionGroupEntry *)entry)->Per_Sample_IV_size);
		gf_bs_write_data(bs, (char *)((GF_CENCSampleEncryptionGroupEntry *)entry)->KID, 16);
		if ((((GF_CENCSampleEncryptionGroupEntry *)entry)->IsProtected == 1) && !((GF_CENCSampleEncryptionGroupEntry *)entry)->Per_Sample_IV_size) {
			gf_bs_write_u8(bs, ((GF_CENCSampleEncryptionGroupEntry *)entry)->constant_IV_size);
			gf_bs_write_data(bs, (char *)((GF_CENCSampleEncryptionGroupEntry *)entry)->constant_IV, ((GF_CENCSampleEncryptionGroupEntry *)entry)->constant_IV_size);
		}
		return;
	case GF_ISOM_SAMPLE_GROUP_OINF:
		gf_isom_oinf_write_entry(entry, bs);
		return;
	case GF_ISOM_SAMPLE_GROUP_LINF:
		gf_isom_linf_write_entry(entry, bs);
		return;
	default:
	{
		GF_DefaultSampleGroupDescriptionEntry *ptr = (GF_DefaultSampleGroupDescriptionEntry *)entry;
		if (ptr->length)
			gf_bs_write_data(bs, (char *) ptr->data, ptr->length);
	}
	}