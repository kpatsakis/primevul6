static int midi_setup_pitch(struct _mdi *mdi, uint8_t channel, uint16_t pitch) {
    MIDI_EVENT_DEBUG(__FUNCTION__,channel, pitch);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_pitch;
    mdi->events[mdi->event_count].event_data.channel = channel;
    mdi->events[mdi->event_count].event_data.data.value = pitch;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}