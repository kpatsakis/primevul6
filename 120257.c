
static void *sgpd_parse_entry(u32 grouping_type, GF_BitStream *bs, u32 entry_size, u32 *total_bytes)
{
	Bool null_size_ok = GF_FALSE;

	GF_DefaultSampleGroupDescriptionEntry *ptr;
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
	{
		GF_RollRecoveryEntry *ptr;
		GF_SAFEALLOC(ptr, GF_RollRecoveryEntry);
		if (!ptr) return NULL;
		ptr->roll_distance = gf_bs_read_int(bs, 16);
		*total_bytes = 2;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_RAP:
	{
		GF_VisualRandomAccessEntry *ptr;
		GF_SAFEALLOC(ptr, GF_VisualRandomAccessEntry);
		if (!ptr) return NULL;
		ptr->num_leading_samples_known = gf_bs_read_int(bs, 1);
		ptr->num_leading_samples = gf_bs_read_int(bs, 7);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SAP:
	{
		GF_SAPEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SAPEntry);
		if (!ptr) return NULL;
		ptr->dependent_flag = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 3);
		ptr->SAP_type = gf_bs_read_int(bs, 4);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SYNC:
	{
		GF_SYNCEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SYNCEntry);
		if (!ptr) return NULL;
		gf_bs_read_int(bs, 2);
		ptr->NALU_type = gf_bs_read_int(bs, 6);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_TELE:
	{
		GF_TemporalLevelEntry *ptr;
		GF_SAFEALLOC(ptr, GF_TemporalLevelEntry);
		if (!ptr) return NULL;
		ptr->level_independently_decodable = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 7);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	{
		GF_CENCSampleEncryptionGroupEntry *ptr;
		GF_SAFEALLOC(ptr, GF_CENCSampleEncryptionGroupEntry);
		if (!ptr) return NULL;
		gf_bs_read_u8(bs); //reserved
		ptr->crypt_byte_block = gf_bs_read_int(bs, 4);
		ptr->skip_byte_block = gf_bs_read_int(bs, 4);
		ptr->IsProtected = gf_bs_read_u8(bs);
		ptr->Per_Sample_IV_size = gf_bs_read_u8(bs);
		gf_bs_read_data(bs, (char *)ptr->KID, 16);
		*total_bytes = 20;
		if ((ptr->IsProtected == 1) && !ptr->Per_Sample_IV_size) {
			ptr->constant_IV_size = gf_bs_read_u8(bs);
			if ((ptr->constant_IV_size != 8) && (ptr->constant_IV_size != 16)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] seig sample group have invalid constant_IV size\n"));
				gf_free(ptr);
				return NULL;
			}
			gf_bs_read_data(bs, (char *)ptr->constant_IV, ptr->constant_IV_size);
			*total_bytes += 1 + ptr->constant_IV_size;
		}
		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] seig sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_OINF:
	{
		GF_OperatingPointsInformation *ptr = gf_isom_oinf_new_entry();
		u32 s = (u32) gf_bs_get_position(bs);
		gf_isom_oinf_read_entry(ptr, bs);
		*total_bytes = (u32) gf_bs_get_position(bs) - s;
		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] oinf sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_LINF:
	{
		GF_LHVCLayerInformation *ptr = gf_isom_linf_new_entry();
		u32 s = (u32) gf_bs_get_position(bs);
		gf_isom_linf_read_entry(ptr, bs);
		*total_bytes = (u32) gf_bs_get_position(bs) - s;
		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] linf sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}

	case GF_ISOM_SAMPLE_GROUP_TRIF:
		if (! entry_size) {
			u32 flags = gf_bs_peek_bits(bs, 24, 0);
			if (flags & 0x10000) entry_size=3;
			else {
				if (flags & 0x80000) entry_size=7;
				else entry_size=11;
				//have dependency list
				if (flags & 0x200000) {
					u32 nb_entries = gf_bs_peek_bits(bs, 16, entry_size);
					entry_size += 2 + 2*nb_entries;
				}
			}
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] trif sample group does not indicate entry size, deprecated in spec\n"));
		}
		break;
	case GF_ISOM_SAMPLE_GROUP_NALM:
		if (! entry_size) {
			u64 start = gf_bs_get_position(bs);
			Bool rle, large_size;
			u32 entry_count;
			gf_bs_read_int(bs, 6);
			large_size = gf_bs_read_int(bs, 1);
			rle = gf_bs_read_int(bs, 1);
			entry_count = gf_bs_read_int(bs, large_size ? 16 : 8);
			gf_bs_seek(bs, start);
			entry_size = 1 + large_size ? 2 : 1;
			entry_size += entry_count * 2;
			if (rle) entry_size += entry_count * (large_size ? 2 : 1);
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] nalm sample group does not indicate entry size, deprecated in spec\n"));
		}
		break;

	case GF_ISOM_SAMPLE_GROUP_TSAS:
	case GF_ISOM_SAMPLE_GROUP_STSA:
		null_size_ok = GF_TRUE;
		break;
	//TODO, add support for these ones ?
	case GF_ISOM_SAMPLE_GROUP_TSCL:
		entry_size = 20;
		break;
	case GF_ISOM_SAMPLE_GROUP_LBLI:
		entry_size = 2;
		break;
	default:
		break;
	}

	if (!entry_size && !null_size_ok) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] %s sample group does not indicate entry size and is not implemented, cannot parse!\n", gf_4cc_to_str( grouping_type) ));
		return NULL;
	}
	GF_SAFEALLOC(ptr, GF_DefaultSampleGroupDescriptionEntry);
	if (!ptr) return NULL;
	if (entry_size) {
		ptr->length = entry_size;
		ptr->data = (u8 *) gf_malloc(sizeof(u8)*ptr->length);
		gf_bs_read_data(bs, (char *) ptr->data, ptr->length);
		*total_bytes = entry_size;
	}
	return ptr;