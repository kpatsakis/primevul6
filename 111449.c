void _WM_AdjustNoteVolumes(struct _mdi *mdi, uint8_t ch, struct _note *nte) {
    float premix_dBm;
    float premix_lin;
    uint8_t pan_ofs;
    float premix_dBm_left;
    float premix_dBm_right;
    float premix_left;
    float premix_right;
    float volume_adj;
    uint32_t vol_ofs;

    /*
     Pointless CPU heating checks to shoosh up a compiler
     */
    if (ch > 0x0f) ch = 0x0f;

    if (nte->ignore_chan_events) return;

    pan_ofs = mdi->channel[ch].balance + mdi->channel[ch].pan - 64;

    vol_ofs = (nte->velocity * ((mdi->channel[ch].expression * mdi->channel[ch].volume) / 127)) / 127;

    /*
     This value is to reduce the chance of clipping.
     Higher value means lower overall volume,
     Lower value means higher overall volume.
     NOTE: The lower the value the higher the chance of clipping.
     FIXME: Still needs tuning. Clipping heard at a value of 3.75
     */
#define VOL_DIVISOR 4.0
    volume_adj = ((float)_WM_MasterVolume / 1024.0) / VOL_DIVISOR;

    MIDI_EVENT_DEBUG(__FUNCTION__,ch, 0);

    if (pan_ofs > 127) pan_ofs = 127;
    premix_dBm_left = dBm_pan_volume[(127-pan_ofs)];
    premix_dBm_right = dBm_pan_volume[pan_ofs];

    if (mdi->extra_info.mixer_options & WM_MO_LOG_VOLUME) {
        premix_dBm = dBm_volume[vol_ofs];

        premix_dBm_left += premix_dBm;
        premix_dBm_right += premix_dBm;

        premix_left = (powf(10.0,(premix_dBm_left / 20.0))) * volume_adj;
        premix_right = (powf(10.0,(premix_dBm_right / 20.0))) * volume_adj;
    } else {
        premix_lin = (float)(_WM_lin_volume[vol_ofs]) / 1024.0;

        premix_left = premix_lin * powf(10.0, (premix_dBm_left / 20)) * volume_adj;
        premix_right = premix_lin * powf(10.0, (premix_dBm_right / 20)) * volume_adj;
    }
    nte->left_mix_volume = (int32_t)(premix_left * 1024.0);
    nte->right_mix_volume = (int32_t)(premix_right * 1024.0);
}