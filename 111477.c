void _WM_do_control_non_registered_param_course(struct _mdi *mdi,
                                     struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);
    mdi->channel[ch].reg_data = (mdi->channel[ch].reg_data & 0x7F)
                                | (data->data.value << 7);
    mdi->channel[ch].reg_non = 1;
}