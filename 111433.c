static int midi_setup_sequenceno(struct _mdi *mdi, uint32_t setting) {
    MIDI_EVENT_DEBUG(__FUNCTION__,0, setting);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_meta_sequenceno;
    mdi->events[mdi->event_count].event_data.channel = 0;
    mdi->events[mdi->event_count].event_data.data.value = setting;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}