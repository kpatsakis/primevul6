static int mov_write_audio_tag(AVFormatContext *s, AVIOContext *pb, MOVMuxContext *mov, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int version = 0;
    uint32_t tag = track->tag;

    if (track->mode == MODE_MOV) {
        if (track->timescale > UINT16_MAX || !track->par->channels) {
            if (mov_get_lpcm_flags(track->par->codec_id))
                tag = AV_RL32("lpcm");
            version = 2;
        } else if (track->audio_vbr || mov_pcm_le_gt16(track->par->codec_id) ||
                   mov_pcm_be_gt16(track->par->codec_id) ||
                   track->par->codec_id == AV_CODEC_ID_ADPCM_MS ||
                   track->par->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV ||
                   track->par->codec_id == AV_CODEC_ID_QDM2) {
            version = 1;
        }
    }

    avio_wb32(pb, 0); /* size */
    if (mov->encryption_scheme != MOV_ENC_NONE) {
        ffio_wfourcc(pb, "enca");
    } else {
        avio_wl32(pb, tag); // store it byteswapped
    }
    avio_wb32(pb, 0); /* Reserved */
    avio_wb16(pb, 0); /* Reserved */
    avio_wb16(pb, 1); /* Data-reference index, XXX  == 1 */

    /* SoundDescription */
    avio_wb16(pb, version); /* Version */
    avio_wb16(pb, 0); /* Revision level */
    avio_wb32(pb, 0); /* Reserved */

    if (version == 2) {
        avio_wb16(pb, 3);
        avio_wb16(pb, 16);
        avio_wb16(pb, 0xfffe);
        avio_wb16(pb, 0);
        avio_wb32(pb, 0x00010000);
        avio_wb32(pb, 72);
        avio_wb64(pb, av_double2int(track->par->sample_rate));
        avio_wb32(pb, track->par->channels);
        avio_wb32(pb, 0x7F000000);
        avio_wb32(pb, av_get_bits_per_sample(track->par->codec_id));
        avio_wb32(pb, mov_get_lpcm_flags(track->par->codec_id));
        avio_wb32(pb, track->sample_size);
        avio_wb32(pb, get_samples_per_packet(track));
    } else {
        if (track->mode == MODE_MOV) {
            avio_wb16(pb, track->par->channels);
            if (track->par->codec_id == AV_CODEC_ID_PCM_U8 ||
                track->par->codec_id == AV_CODEC_ID_PCM_S8)
                avio_wb16(pb, 8); /* bits per sample */
            else if (track->par->codec_id == AV_CODEC_ID_ADPCM_G726)
                avio_wb16(pb, track->par->bits_per_coded_sample);
            else
                avio_wb16(pb, 16);
            avio_wb16(pb, track->audio_vbr ? -2 : 0); /* compression ID */
        } else { /* reserved for mp4/3gp */
            avio_wb16(pb, 2);
            avio_wb16(pb, 16);
            avio_wb16(pb, 0);
        }

        avio_wb16(pb, 0); /* packet size (= 0) */
        avio_wb16(pb, track->par->sample_rate <= UINT16_MAX ?
                      track->par->sample_rate : 0);
        avio_wb16(pb, 0); /* Reserved */
    }

    if (version == 1) { /* SoundDescription V1 extended info */
        if (mov_pcm_le_gt16(track->par->codec_id) ||
            mov_pcm_be_gt16(track->par->codec_id))
            avio_wb32(pb, 1); /*  must be 1 for  uncompressed formats */
        else
            avio_wb32(pb, track->par->frame_size); /* Samples per packet */
        avio_wb32(pb, track->sample_size / track->par->channels); /* Bytes per packet */
        avio_wb32(pb, track->sample_size); /* Bytes per frame */
        avio_wb32(pb, 2); /* Bytes per sample */
    }

    if (track->mode == MODE_MOV &&
        (track->par->codec_id == AV_CODEC_ID_AAC           ||
         track->par->codec_id == AV_CODEC_ID_AC3           ||
         track->par->codec_id == AV_CODEC_ID_EAC3          ||
         track->par->codec_id == AV_CODEC_ID_AMR_NB        ||
         track->par->codec_id == AV_CODEC_ID_ALAC          ||
         track->par->codec_id == AV_CODEC_ID_ADPCM_MS      ||
         track->par->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV ||
         track->par->codec_id == AV_CODEC_ID_QDM2          ||
         (mov_pcm_le_gt16(track->par->codec_id) && version==1) ||
         (mov_pcm_be_gt16(track->par->codec_id) && version==1)))
        mov_write_wave_tag(s, pb, track);
    else if (track->tag == MKTAG('m','p','4','a'))
        mov_write_esds_tag(pb, track);
    else if (track->par->codec_id == AV_CODEC_ID_AMR_NB)
        mov_write_amr_tag(pb, track);
    else if (track->par->codec_id == AV_CODEC_ID_AC3)
        mov_write_ac3_tag(pb, track);
    else if (track->par->codec_id == AV_CODEC_ID_EAC3)
        mov_write_eac3_tag(pb, track);
    else if (track->par->codec_id == AV_CODEC_ID_ALAC)
        mov_write_extradata_tag(pb, track);
    else if (track->par->codec_id == AV_CODEC_ID_WMAPRO)
        mov_write_wfex_tag(s, pb, track);
    else if (track->vos_len > 0)
        mov_write_glbl_tag(pb, track);

    if (track->mode == MODE_MOV && track->par->codec_type == AVMEDIA_TYPE_AUDIO)
        mov_write_chan_tag(s, pb, track);

    if (mov->encryption_scheme != MOV_ENC_NONE) {
        ff_mov_cenc_write_sinf_tag(track, pb, mov->encryption_kid);
    }

    return update_size(pb, pos);
}