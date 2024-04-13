static int midi_setup_sysex_gm_reset(struct _mdi *mdi) {
    MIDI_EVENT_DEBUG(__FUNCTION__,0,0);

    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_sysex_roland_reset;
    mdi->events[mdi->event_count].event_data.channel = 0;
    mdi->events[mdi->event_count].event_data.data.value = 0;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}