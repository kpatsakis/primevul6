void _WM_do_sysex_roland_drum_track(struct _mdi *mdi, struct _event_data *data) {
    uint8_t ch = data->channel;

    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if (data->data.value > 0) {
        mdi->channel[ch].isdrum = 1;
        mdi->channel[ch].patch = NULL;
    } else {
        mdi->channel[ch].isdrum = 0;
        mdi->channel[ch].patch = _WM_get_patch_data(mdi, 0);
    }
}