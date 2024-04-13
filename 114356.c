GF_Err gf_isom_get_sample_cenc_info_internal(GF_TrackBox *trak, void *traf, GF_SampleEncryptionBox *senc, u32 sample_number, Bool *IsEncrypted, u8 *crypt_byte_block, u8 *skip_byte_block, const u8 **key_info, u32 *key_info_size)
#endif
{
	GF_SampleGroupBox *sample_group;
	u32 j, group_desc_index;
	GF_SampleGroupDescriptionBox *sgdesc;
	u32 i, count;
	u32 descIndex, chunkNum;
	u64 offset;
	u32 first_sample_in_entry, last_sample_in_entry;
	GF_CENCSampleEncryptionGroupEntry *entry;

	if (IsEncrypted) *IsEncrypted = GF_FALSE;
	if (crypt_byte_block) *crypt_byte_block = 0;
	if (skip_byte_block) *skip_byte_block = 0;
	if (key_info) *key_info = NULL;
	if (key_info_size) *key_info_size = 0;
	
	if (!trak) return GF_BAD_PARAM;

#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (traf)
		return GF_NOT_SUPPORTED;
#else
	sample_number -= trak->sample_count_at_seg_start;
#endif

	if (trak->Media->information->sampleTable->SampleSize && trak->Media->information->sampleTable->SampleSize->sampleCount>=sample_number) {
		stbl_GetSampleInfos(trak->Media->information->sampleTable, sample_number, &offset, &chunkNum, &descIndex, NULL);
	} else {
		//this is dump mode of fragments, we haven't merged tables yet - use current stsd idx indicated in trak
		descIndex = trak->current_traf_stsd_idx;
		if (!descIndex) descIndex = 1;
	}

	gf_isom_cenc_get_default_info_internal(trak, descIndex, NULL, IsEncrypted, crypt_byte_block, skip_byte_block, key_info, key_info_size);

	sample_group = NULL;
	group_desc_index = 0;
	if (trak->Media->information->sampleTable->sampleGroups) {
		count = gf_list_count(trak->Media->information->sampleTable->sampleGroups);
		for (i=0; i<count; i++) {
			sample_group = (GF_SampleGroupBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroups, i);
			if (sample_group->grouping_type ==  GF_ISOM_SAMPLE_GROUP_SEIG)
				break;
			sample_group = NULL;
		}
		if (sample_group) {
			first_sample_in_entry = 1;
			for (j=0; j<sample_group->entry_count; j++) {
				last_sample_in_entry = first_sample_in_entry + sample_group->sample_entries[j].sample_count - 1;
				if ((sample_number<first_sample_in_entry) || (sample_number>last_sample_in_entry)) {
					first_sample_in_entry = last_sample_in_entry+1;
					continue;
				}
				/*we found our sample*/
				group_desc_index = sample_group->sample_entries[j].group_description_index;
				break;
			}
		}
	}
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (!group_desc_index && traf && traf->sampleGroups) {
		count = gf_list_count(traf->sampleGroups);
		for (i=0; i<count; i++) {
			group_desc_index = 0;
			sample_group = (GF_SampleGroupBox*)gf_list_get(traf->sampleGroups, i);
			if (sample_group->grouping_type ==  GF_ISOM_SAMPLE_GROUP_SEIG)
				break;
			sample_group = NULL;
		}
		if (sample_group) {
			first_sample_in_entry = 1;
			for (j=0; j<sample_group->entry_count; j++) {
				last_sample_in_entry = first_sample_in_entry + sample_group->sample_entries[j].sample_count - 1;
				if ((sample_number<first_sample_in_entry) || (sample_number>last_sample_in_entry)) {
					first_sample_in_entry = last_sample_in_entry+1;
					continue;
				}
				/*we found our sample*/
				group_desc_index = sample_group->sample_entries[j].group_description_index;
				break;
			}
		}
	}
#endif
	/*no sampleGroup info associated*/
	if (!group_desc_index) goto exit;

	sgdesc = NULL;

	if (group_desc_index<=0x10000) {
		for (j=0; j<gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription); j++) {
			sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, j);
			if (sgdesc->grouping_type==sample_group->grouping_type) break;
			sgdesc = NULL;
		}
	}
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	else if (traf) {
		group_desc_index -= 0x10000;
		for (j=0; j<gf_list_count(traf->sampleGroupsDescription); j++) {
			sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(traf->sampleGroupsDescription, j);
			if (sgdesc->grouping_type==sample_group->grouping_type) break;
			sgdesc = NULL;
		}
	}
#endif
	/*no sampleGroup description found for this group (invalid file)*/
	if (!sgdesc) return GF_ISOM_INVALID_FILE;

	entry = (GF_CENCSampleEncryptionGroupEntry *) gf_list_get(sgdesc->group_descriptions, group_desc_index - 1);
	if (!entry) return GF_ISOM_INVALID_FILE;

	if (IsEncrypted) *IsEncrypted = entry->IsProtected;
	if (crypt_byte_block) *crypt_byte_block = entry->crypt_byte_block;
	if (skip_byte_block) *skip_byte_block = entry->skip_byte_block;

	if (key_info) *key_info = entry->key_info;
	if (key_info_size) *key_info_size = entry->key_info_size;

exit:
	//in PIFF we may have default values if no TENC is present: 8 bytes for IV size
	if (( (senc && senc->piff_type==1) || (trak->moov && trak->moov->mov->is_smooth) ) && key_info && ! (*key_info) ) {
		if (!senc) {
			if (IsEncrypted) *IsEncrypted = GF_TRUE;
			if (key_info_size) *key_info_size = 8;
		} else {
			if (!senc->piff_type) {
				senc->piff_type = 2;
				senc->IV_size = 8;
			}
			assert(senc->IV_size);
			if (IsEncrypted) *IsEncrypted = GF_TRUE;
			if (key_info_size) *key_info_size = senc->IV_size;
		}
	}

	return GF_OK;
}