void _WM_do_control_data_increment(struct _mdi *mdi,
                                      struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if ((mdi->channel[ch].reg_non == 0)
        && (mdi->channel[ch].reg_data == 0x0000)) { /* Pitch Bend Range */
        if (mdi->channel[ch].pitch_range < 0x3FFF)
            mdi->channel[ch].pitch_range++;
    }
}