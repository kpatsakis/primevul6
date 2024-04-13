static int midi_setup_patch(struct _mdi *mdi, uint8_t channel, uint8_t patch) {
    MIDI_EVENT_DEBUG(__FUNCTION__,channel, patch);
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = *_WM_do_patch;
    mdi->events[mdi->event_count].event_data.channel = channel;
    mdi->events[mdi->event_count].event_data.data.value = patch;
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;

    if (mdi->channel[channel].isdrum) {
        mdi->channel[channel].bank = patch;
    } else {
        _WM_load_patch(mdi, ((mdi->channel[channel].bank << 8) | patch));
        mdi->channel[channel].patch = _WM_get_patch_data(mdi,
                                                     ((mdi->channel[channel].bank << 8) | patch));
    }
    return (0);
}