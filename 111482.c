void _WM_do_control_dummy(struct _mdi *mdi, struct _event_data *data) {
#ifdef DEBUG_MIDI
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__, ch, data->data.value);
#else
    UNUSED(data);
#endif
    UNUSED(mdi);
}