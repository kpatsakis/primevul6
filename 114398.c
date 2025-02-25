u32 gf_isom_guess_specification(GF_ISOFile *file)
{
	u32 count, i, nb_any, nb_m4s, nb_a, nb_v, nb_auxv,nb_scene, nb_od, nb_mp3, nb_aac, nb_m4v, nb_avc, nb_amr, nb_h263, nb_qcelp, nb_evrc, nb_smv, nb_text, nb_pict;

	nb_m4s = nb_a = nb_v = nb_auxv = nb_any = nb_scene = nb_od = nb_mp3 = nb_aac = nb_m4v = nb_avc = nb_amr = nb_h263 = nb_qcelp = nb_evrc = nb_smv = nb_text = nb_pict = 0;

	if (file->is_jp2) {
		if (file->moov) return GF_ISOM_BRAND_MJP2;
		return GF_ISOM_BRAND_JP2;
	}
	if (!file->moov) {
		if (!file->meta || !file->meta->handler) return 0;
		return file->meta->handler->handlerType;
	}

	count = gf_isom_get_track_count(file);
	for (i=0; i<count; i++) {
		u32 mtype = gf_isom_get_media_type(file, i+1);
		u32 mstype = gf_isom_get_media_subtype(file, i+1, 1);

		if (mtype==GF_ISOM_MEDIA_SCENE) {
			nb_scene++;
			/*forces non-isma*/
			if (gf_isom_get_sample_count(file, i+1)>1) nb_m4s++;
		} else if (mtype==GF_ISOM_MEDIA_OD) {
			nb_od++;
			/*forces non-isma*/
			if (gf_isom_get_sample_count(file, i+1)>1) nb_m4s++;
		}
		else if ((mtype==GF_ISOM_MEDIA_TEXT) || (mtype==GF_ISOM_MEDIA_SUBT)) nb_text++;
		else if ((mtype==GF_ISOM_MEDIA_AUDIO) || gf_isom_is_video_handler_type(mtype) ) {
			switch (mstype) {
			case GF_ISOM_SUBTYPE_3GP_AMR:
			case GF_ISOM_SUBTYPE_3GP_AMR_WB:
				nb_amr++;
				break;
			case GF_ISOM_SUBTYPE_3GP_H263:
				nb_h263++;
				break;
			case GF_ISOM_SUBTYPE_3GP_EVRC:
				nb_evrc++;
				break;
			case GF_ISOM_SUBTYPE_3GP_QCELP:
				nb_qcelp++;
				break;
			case GF_ISOM_SUBTYPE_3GP_SMV:
				nb_smv++;
				break;
			case GF_ISOM_SUBTYPE_AVC_H264:
			case GF_ISOM_SUBTYPE_AVC2_H264:
			case GF_ISOM_SUBTYPE_AVC3_H264:
			case GF_ISOM_SUBTYPE_AVC4_H264:
				nb_avc++;
				break;
			case GF_ISOM_SUBTYPE_SVC_H264:
			case GF_ISOM_SUBTYPE_MVC_H264:
				nb_avc++;
				break;
			case GF_ISOM_SUBTYPE_MPEG4:
			case GF_ISOM_SUBTYPE_MPEG4_CRYP:
			{
				GF_DecoderConfig *dcd = gf_isom_get_decoder_config(file, i+1, 1);
				if (!dcd) break;
				switch (dcd->streamType) {
				case GF_STREAM_VISUAL:
					if (dcd->objectTypeIndication==GF_CODECID_MPEG4_PART2) nb_m4v++;
					else if ((dcd->objectTypeIndication==GF_CODECID_AVC) || (dcd->objectTypeIndication==GF_CODECID_SVC) || (dcd->objectTypeIndication==GF_CODECID_MVC)) nb_avc++;
					else nb_v++;
					break;
				case GF_STREAM_AUDIO:
					switch (dcd->objectTypeIndication) {
					case GF_CODECID_AAC_MPEG2_MP:
					case GF_CODECID_AAC_MPEG2_LCP:
					case GF_CODECID_AAC_MPEG2_SSRP:
					case GF_CODECID_AAC_MPEG4:
						nb_aac++;
						break;
					case GF_CODECID_MPEG2_PART3:
					case GF_CODECID_MPEG_AUDIO:
					case GF_CODECID_MPEG_AUDIO_L1:
						nb_mp3++;
						break;
					case GF_CODECID_EVRC:
						nb_evrc++;
						break;
					case GF_CODECID_SMV:
						nb_smv++;
						break;
					case GF_CODECID_QCELP:
						nb_qcelp++;
						break;
					default:
						nb_a++;
						break;
					}
					break;
				/*SHOULD NEVER HAPPEN - IF SO, BROKEN MPEG4 FILE*/
				default:
					nb_any++;
					break;
				}
				gf_odf_desc_del((GF_Descriptor *)dcd);
			}
				break;
			default:
				if (mtype==GF_ISOM_MEDIA_VISUAL) nb_v++;
				else if (mtype==GF_ISOM_MEDIA_AUXV) nb_auxv++;
				else if (mtype==GF_ISOM_MEDIA_PICT) nb_pict++;
				else nb_a++;
				break;
			}
		} else if ((mtype==GF_ISOM_SUBTYPE_MPEG4) || (mtype==GF_ISOM_SUBTYPE_MPEG4_CRYP)) nb_m4s++;
		else nb_any++;
	}
	if (nb_any) return GF_ISOM_BRAND_ISOM;
	if (nb_qcelp || nb_evrc || nb_smv) {
		/*non std mix of streams*/
		if (nb_m4s || nb_avc || nb_scene || nb_od || nb_mp3 || nb_a || nb_v) return GF_ISOM_BRAND_ISOM;
		return GF_ISOM_BRAND_3G2A;
	}
	/*other a/v/s streams*/
	if (nb_v || nb_a || nb_m4s) return GF_ISOM_BRAND_MP42;

	nb_v = nb_m4v + nb_avc + nb_h263;
	nb_a = nb_mp3 + nb_aac + nb_amr;

	/*avc file: whatever has AVC and no systems*/
	if (nb_avc) {
		if (!nb_scene && !nb_od) return GF_ISOM_BRAND_AVC1;
		return GF_ISOM_BRAND_MP42;
	}
	/*MP3: ISMA and MPEG4*/
	if (nb_mp3) {
		if (!nb_text && (nb_v<=1) && (nb_a<=1) && (nb_scene==1) && (nb_od==1))
			return GF_ISOM_BRAND_ISMA;
		return GF_ISOM_BRAND_MP42;
	}
	/*MP4*/
	if (nb_scene || nb_od) {
		/*issue with AMR and H263 which don't have MPEG mapping: non compliant file*/
		if (nb_amr || nb_h263) return GF_ISOM_BRAND_ISOM;
		return GF_ISOM_BRAND_MP42;
	}
	/*use ISMA (3GP fine too)*/
	if (!nb_amr && !nb_h263 && !nb_text) {
		if ((nb_v<=1) && (nb_a<=1)) return GF_ISOM_BRAND_ISMA;
		return GF_ISOM_BRAND_MP42;
	}

	if ((nb_v<=1) && (nb_a<=1) && (nb_text<=1)) return nb_text ? GF_ISOM_BRAND_3GP6 : GF_ISOM_BRAND_3GP5;
	return GF_ISOM_BRAND_3GG6;
}