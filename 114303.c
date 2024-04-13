GF_Err gf_isom_get_sample_for_media_time(GF_ISOFile *the_file, u32 trackNumber, u64 desiredTime, u32 *StreamDescriptionIndex, GF_ISOSearchMode SearchMode, GF_ISOSample **sample, u32 *SampleNum, u64 *data_offset)
{
	GF_Err e;
	u32 sampleNumber, prevSampleNumber, syncNum, shadowSync;
	GF_TrackBox *trak;
	GF_ISOSample *shadow;
	GF_SampleTableBox *stbl;
	Bool static_sample = GF_FALSE;
	u8 useShadow, IsSync;

	if (SampleNum) *SampleNum = 0;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (desiredTime < trak->dts_at_seg_start) {
		desiredTime = 0;
	} else {
		desiredTime -= trak->dts_at_seg_start;
	}
#endif

	e = stbl_findEntryForTime(stbl, desiredTime, 0, &sampleNumber, &prevSampleNumber);
	if (e) return e;

	//if no shadow table, reset to sync only
	useShadow = 0;
	if (!stbl->ShadowSync && (SearchMode == GF_ISOM_SEARCH_SYNC_SHADOW))
		SearchMode = GF_ISOM_SEARCH_SYNC_BACKWARD;

	//if no syncTable, disable syncSearching, as all samples ARE sync
	if (! trak->Media->information->sampleTable->SyncSample) {
		if (SearchMode == GF_ISOM_SEARCH_SYNC_FORWARD) SearchMode = GF_ISOM_SEARCH_FORWARD;
		if (SearchMode == GF_ISOM_SEARCH_SYNC_BACKWARD) SearchMode = GF_ISOM_SEARCH_BACKWARD;
	}

	//not found, return EOF or browse backward
	if (!sampleNumber && !prevSampleNumber) {
		if (SearchMode == GF_ISOM_SEARCH_SYNC_BACKWARD || SearchMode == GF_ISOM_SEARCH_BACKWARD) {
			sampleNumber = trak->Media->information->sampleTable->SampleSize->sampleCount;
		}
		if (!sampleNumber) return GF_EOS;
	}

	//check in case we have the perfect sample
	IsSync = 0;

	//according to the direction adjust the sampleNum value
	switch (SearchMode) {
	case GF_ISOM_SEARCH_SYNC_FORWARD:
		IsSync = 1;
	case GF_ISOM_SEARCH_FORWARD:
		//not the exact one
		if (!sampleNumber) {
			if (prevSampleNumber != stbl->SampleSize->sampleCount) {
				sampleNumber = prevSampleNumber + 1;
			} else {
				sampleNumber = prevSampleNumber;
			}
		}
		break;

	//if dummy mode, reset to default browsing
	case GF_ISOM_SEARCH_SYNC_BACKWARD:
		IsSync = 1;
	case GF_ISOM_SEARCH_SYNC_SHADOW:
	case GF_ISOM_SEARCH_BACKWARD:
	default:
		//first case, not found....
		if (!sampleNumber && !prevSampleNumber) {
			sampleNumber = stbl->SampleSize->sampleCount;
		} else if (!sampleNumber) {
			sampleNumber = prevSampleNumber;
		}
		break;
	}

	//get the sync sample num
	if (IsSync) {
		//get the SyncNumber
		e = Media_FindSyncSample(trak->Media->information->sampleTable,
		                         sampleNumber, &syncNum, SearchMode);
		if (e) return e;
		if (syncNum) sampleNumber = syncNum;
		syncNum = 0;
	}
	//if we are in shadow mode, get the previous sync sample
	//in case we can't find a good SyncShadow
	else if (SearchMode == GF_ISOM_SEARCH_SYNC_SHADOW) {
		//get the SyncNumber
		e = Media_FindSyncSample(trak->Media->information->sampleTable,
		                         sampleNumber, &syncNum, GF_ISOM_SEARCH_SYNC_BACKWARD);
		if (e) return e;
	}


	//OK sampleNumber is exactly the sample we need (except for shadow)

	if (sample) {
		if (*sample) {
			static_sample = GF_TRUE;
		} else {
			*sample = gf_isom_sample_new();
			if (*sample == NULL) return GF_OUT_OF_MEM;
		}
	}
	//we are in shadow mode, we need to browse both SyncSample and ShadowSyncSample to get
	//the desired sample...
	if (SearchMode == GF_ISOM_SEARCH_SYNC_SHADOW) {
		//get the shadowing number
		stbl_GetSampleShadow(stbl->ShadowSync, &sampleNumber, &shadowSync);
		//now sampleNumber is the closest previous shadowed sample.
		//1- If we have a closer sync sample, use it.
		//2- if the shadowSync is 0, we don't have any shadowing, use syncNum
		if ((sampleNumber < syncNum) || (!shadowSync)) {
			sampleNumber = syncNum;
		} else {
			//otherwise, we have a better alternate sample in the shadowSync for this sample
			useShadow = 1;
		}
	}

	e = Media_GetSample(trak->Media, sampleNumber, sample, StreamDescriptionIndex, GF_FALSE, data_offset);
	if (e) {
		if (!static_sample)
			gf_isom_sample_del(sample);
		else if (! (*sample)->alloc_size && (*sample)->data && (*sample)->dataLength )
		 	(*sample)->alloc_size =  (*sample)->dataLength;

		return e;
	}
	if (sample && ! (*sample)->IsRAP) {
		Bool is_rap;
		GF_ISOSampleRollType roll_type;
		e = gf_isom_get_sample_rap_roll_info(the_file, trackNumber, sampleNumber, &is_rap, &roll_type, NULL);
		if (e) return e;
		if (is_rap) (*sample)->IsRAP = SAP_TYPE_3;
	}
	//optionally get the sample number
	if (SampleNum) {
		*SampleNum = sampleNumber;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		*SampleNum += trak->sample_count_at_seg_start;
#endif
	}

	//in shadow mode, we only get the data of the shadowing sample !
	if (sample && useShadow) {
		//we have to use StreamDescriptionIndex in case the sample data is in another desc
		//though this is unlikely as non optimized...
		shadow = gf_isom_get_sample(the_file, trackNumber, shadowSync, StreamDescriptionIndex);
		//if no sample, the shadowSync is broken, return the sample
		if (!shadow) return GF_OK;
		(*sample)->IsRAP = RAP;
		gf_free((*sample)->data);
		(*sample)->dataLength = shadow->dataLength;
		(*sample)->data = shadow->data;
		//set data length to 0 to keep the buffer alive...
		shadow->dataLength = 0;
		gf_isom_sample_del(&shadow);
	}
	if (static_sample && ! (*sample)->alloc_size )
		 (*sample)->alloc_size =  (*sample)->dataLength;
		 
	return GF_OK;
}