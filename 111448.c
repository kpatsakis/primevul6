void _WM_do_meta_endoftrack(struct _mdi *mdi, struct _event_data *data) {
/* placeholder function so we can record eot in the event stream
 * for conversion function _WM_Event2Midi */
#ifdef DEBUG_MIDI
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__, ch, data->data.value);
#else
    UNUSED(data);
#endif

    _WM_Release_Allowance(mdi);
    return;
}