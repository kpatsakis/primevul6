void _WM_do_control_bank_select(struct _mdi *mdi, struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);
    mdi->channel[ch].bank = data->data.value;
}