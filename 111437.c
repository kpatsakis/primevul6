void _WM_do_meta_text(struct _mdi *mdi, struct _event_data *data) {
/* placeholder function so we can record tempo in the event stream
 * for conversion function _WM_Event2Midi */
#ifdef DEBUG_MIDI
    uint8_t ch = data->channel;
    MIDI_EVENT_SDEBUG(__FUNCTION__, ch, data->data.string);
#endif
    if (mdi->extra_info.mixer_options & WM_MO_TEXTASLYRIC) {
        mdi->lyric = data->data.string;
    }

    return;
}