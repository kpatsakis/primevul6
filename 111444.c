void _WM_do_control_channel_pan(struct _mdi *mdi, struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    mdi->channel[ch].pan = data->data.value;
    _WM_AdjustChannelVolumes(mdi, ch);
}