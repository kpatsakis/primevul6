static void gf_isom_check_sample_desc(GF_TrackBox *trak)
{
	GF_BitStream *bs;
	GF_UnknownBox *a;
	u32 i;

	if (!trak->Media || !trak->Media->information) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no media box !\n" ));
		return;
	}
	if (!trak->Media->information->sampleTable) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no sample table !\n" ));
		trak->Media->information->sampleTable = (GF_SampleTableBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STBL);
		gf_isom_box_add_for_dump_mode((GF_Box *)trak->Media->information, (GF_Box *)trak->Media->information->sampleTable);
	}

	if (!trak->Media->information->sampleTable->SampleDescription) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no sample description box !\n" ));
		trak->Media->information->sampleTable->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STSD);
		gf_isom_box_add_for_dump_mode((GF_Box *)trak->Media->information->sampleTable, (GF_Box *)trak->Media->information->sampleTable->SampleDescription);
		return;
	}

	i=0;
	while ((a = (GF_UnknownBox*)gf_list_enum(trak->Media->information->sampleTable->SampleDescription->other_boxes, &i))) {
		switch (a->type) {
		case GF_ISOM_BOX_TYPE_MP4S:
		case GF_ISOM_BOX_TYPE_ENCS:
		case GF_ISOM_BOX_TYPE_MP4A:
		case GF_ISOM_BOX_TYPE_ENCA:
		case GF_ISOM_BOX_TYPE_MP4V:
		case GF_ISOM_BOX_TYPE_ENCV:
		case GF_ISOM_BOX_TYPE_RESV:
		case GF_ISOM_SUBTYPE_3GP_AMR:
		case GF_ISOM_SUBTYPE_3GP_AMR_WB:
		case GF_ISOM_SUBTYPE_3GP_EVRC:
		case GF_ISOM_SUBTYPE_3GP_QCELP:
		case GF_ISOM_SUBTYPE_3GP_SMV:
		case GF_ISOM_SUBTYPE_3GP_H263:
		case GF_ISOM_BOX_TYPE_GHNT:
		case GF_ISOM_BOX_TYPE_RTP_STSD:
		case GF_ISOM_BOX_TYPE_SRTP_STSD:
		case GF_ISOM_BOX_TYPE_FDP_STSD:
		case GF_ISOM_BOX_TYPE_RRTP_STSD:
		case GF_ISOM_BOX_TYPE_RTCP_STSD:
		case GF_ISOM_BOX_TYPE_METX:
		case GF_ISOM_BOX_TYPE_METT:
		case GF_ISOM_BOX_TYPE_STXT:
		case GF_ISOM_BOX_TYPE_AVC1:
		case GF_ISOM_BOX_TYPE_AVC2:
		case GF_ISOM_BOX_TYPE_AVC3:
		case GF_ISOM_BOX_TYPE_AVC4:
		case GF_ISOM_BOX_TYPE_SVC1:
		case GF_ISOM_BOX_TYPE_MVC1:
		case GF_ISOM_BOX_TYPE_HVC1:
		case GF_ISOM_BOX_TYPE_HEV1:
		case GF_ISOM_BOX_TYPE_HVC2:
		case GF_ISOM_BOX_TYPE_HEV2:
		case GF_ISOM_BOX_TYPE_HVT1:
		case GF_ISOM_BOX_TYPE_LHV1:
		case GF_ISOM_BOX_TYPE_LHE1:
		case GF_ISOM_BOX_TYPE_AV01:
		case GF_ISOM_BOX_TYPE_VP08:
		case GF_ISOM_BOX_TYPE_VP09:
		case GF_ISOM_BOX_TYPE_AV1C:
		case GF_ISOM_BOX_TYPE_TX3G:
		case GF_ISOM_BOX_TYPE_TEXT:
		case GF_ISOM_BOX_TYPE_ENCT:
		case GF_ISOM_BOX_TYPE_DIMS:
		case GF_ISOM_BOX_TYPE_OPUS:
		case GF_ISOM_BOX_TYPE_AC3:
		case GF_ISOM_BOX_TYPE_EC3:
		case GF_ISOM_BOX_TYPE_LSR1:
		case GF_ISOM_BOX_TYPE_WVTT:
		case GF_ISOM_BOX_TYPE_STPP:
		case GF_ISOM_BOX_TYPE_SBTT:
		case GF_ISOM_BOX_TYPE_MP3:
		case GF_ISOM_BOX_TYPE_JPEG:
		case GF_ISOM_BOX_TYPE_PNG:
		case GF_ISOM_BOX_TYPE_JP2K:
		case GF_ISOM_BOX_TYPE_MHA1:
		case GF_ISOM_BOX_TYPE_MHA2:
		case GF_ISOM_BOX_TYPE_MHM1:
		case GF_ISOM_BOX_TYPE_MHM2:
		case GF_QT_BOX_TYPE_AUDIO_RAW:
		case GF_QT_BOX_TYPE_AUDIO_TWOS:
		case GF_QT_BOX_TYPE_AUDIO_SOWT:
		case GF_QT_BOX_TYPE_AUDIO_FL32:
		case GF_QT_BOX_TYPE_AUDIO_FL64:
		case GF_QT_BOX_TYPE_AUDIO_IN24:
		case GF_QT_BOX_TYPE_AUDIO_IN32:
		case GF_QT_BOX_TYPE_AUDIO_ULAW:
		case GF_QT_BOX_TYPE_AUDIO_ALAW:
		case GF_QT_BOX_TYPE_AUDIO_ADPCM:
		case GF_QT_BOX_TYPE_AUDIO_IMA_ADPCM:
		case GF_QT_BOX_TYPE_AUDIO_DVCA:
		case GF_QT_BOX_TYPE_AUDIO_QDMC:
		case GF_QT_BOX_TYPE_AUDIO_QDMC2:
		case GF_QT_BOX_TYPE_AUDIO_QCELP:
		case GF_QT_BOX_TYPE_AUDIO_kMP3:
			continue;

		case GF_ISOM_BOX_TYPE_UNKNOWN:
			break;
		default:
			if (gf_box_valid_in_parent((GF_Box *) a, "stsd")) {
				continue;
			}
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Unexpected box %s in stsd!\n", gf_4cc_to_str(a->type)));
			continue;
		}
		//we are sure to have an unknown box here
		assert(a->type==GF_ISOM_BOX_TYPE_UNKNOWN);

		if (!a->data || (a->dataSize<8) ) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Sample description %s does not have at least 8 bytes!\n", gf_4cc_to_str(a->original_4cc) ));
			continue;
		}
		else if (a->dataSize > a->size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Sample description %s has wrong data size %d!\n", gf_4cc_to_str(a->original_4cc), a->dataSize));
			continue;
		}

#define STSD_SWITCH_BOX(_box) \
		if (gf_bs_available(bs)) { \
			u64 pos = gf_bs_get_position(bs); \
			u32 count_subb = 0; \
			GF_Err e;\
			gf_bs_set_cookie(bs, 1);\
			e = gf_isom_box_array_read((GF_Box *) _box, bs, gf_isom_box_add_default); \
			count_subb = _box->other_boxes ? gf_list_count(_box->other_boxes) : 0; \
			if (!count_subb || e) { \
				gf_bs_seek(bs, pos); \
				_box->data_size = (u32) gf_bs_available(bs); \
				if (_box->data_size) { \
					_box->data = a->data; \
					a->data = NULL; \
					memmove(_box->data, _box->data + pos, _box->data_size); \
				} \
			} else { \
				_box->data_size = 0; \
			} \
		} \
		gf_bs_del(bs); \
		if (!_box->data_size && _box->data) { \
			gf_free(_box->data); \
			_box->data = NULL; \
		} \
		_box->size = 0; \
		_box->EntryType = a->original_4cc; \
		gf_list_rem(trak->Media->information->sampleTable->SampleDescription->other_boxes, i-1); \
		gf_isom_box_del((GF_Box *)a); \
		gf_list_insert(trak->Media->information->sampleTable->SampleDescription->other_boxes, _box, i-1); \


		/*only process visual or audio*/
		switch (trak->Media->handler->handlerType) {
        case GF_ISOM_MEDIA_VISUAL:
		case GF_ISOM_MEDIA_AUXV:
		case GF_ISOM_MEDIA_PICT:
		{
			GF_GenericVisualSampleEntryBox *genv = (GF_GenericVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRV);
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);
			genv->size = a->size-8;
			gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *) genv, bs);

			STSD_SWITCH_BOX(genv)

		}
		break;
		case GF_ISOM_MEDIA_AUDIO:
		{
			GF_GenericAudioSampleEntryBox *gena = (GF_GenericAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRA);
			gena->size = a->size-8;
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);
			gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox *) gena, bs);

			STSD_SWITCH_BOX(gena)

		}
		break;

		default:
		{
			GF_Err e;
			GF_GenericSampleEntryBox *genm = (GF_GenericSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRM);
			genm->size = a->size-8;
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);

			e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)genm, bs);
			if (e) return;

			STSD_SWITCH_BOX(genm)
		}
		break;
		}

	}