static int midi_setup_text(struct _mdi *mdi, char * text) {
    MIDI_EVENT_SDEBUG(__FUNCTION__,0, text);
    strip_text(text);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_meta_text;
    mdi->events[mdi->event_count].event_data.channel = 0;
    mdi->events[mdi->event_count].event_data.data.string = text;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}