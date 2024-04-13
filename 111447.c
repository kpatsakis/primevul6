void _WM_do_sysex_roland_reset(struct _mdi *mdi, struct _event_data *data) {
#ifdef DEBUG_MIDI
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__, ch, data->data.value);
#else
    UNUSED(data);
#endif
    _WM_do_sysex_gm_reset(mdi,data);
}