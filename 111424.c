static int midi_setup_sysex_roland_drum_track(struct _mdi *mdi,
                                              uint8_t channel, uint16_t setting) {
    MIDI_EVENT_DEBUG(__FUNCTION__,channel, setting);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = _WM_do_sysex_roland_drum_track;
    mdi->events[mdi->event_count].event_data.channel = channel;
    mdi->events[mdi->event_count].event_data.data.value = setting;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;

    if (setting > 0) {
        mdi->channel[channel].isdrum = 1;
    } else {
        mdi->channel[channel].isdrum = 0;
    }
    return (0);
}