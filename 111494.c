int _WM_midi_setup_endoftrack(struct _mdi *mdi) {
    MIDI_EVENT_DEBUG(__FUNCTION__,0,0);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_meta_endoftrack;
    mdi->events[mdi->event_count].event_data.channel = 0;
    mdi->events[mdi->event_count].event_data.data.value = 0;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}