void _WM_do_patch(struct _mdi *mdi, struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);
    if (!mdi->channel[ch].isdrum) {
        mdi->channel[ch].patch = _WM_get_patch_data(mdi,
                                                ((mdi->channel[ch].bank << 8) | data->data.value));
    } else {
        mdi->channel[ch].bank = data->data.value;
    }
}