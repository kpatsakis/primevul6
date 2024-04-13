int _WM_midi_setup_noteoff(struct _mdi *mdi, uint8_t channel,
                           uint8_t note, uint8_t velocity) {
    MIDI_EVENT_DEBUG(__FUNCTION__,channel, note);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_note_off;
    mdi->events[mdi->event_count].event_data.channel = channel;
    mdi->events[mdi->event_count].event_data.data.value = (note << 8) | velocity;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}