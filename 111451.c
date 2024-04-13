void _WM_do_control_channel_controllers_off(struct _mdi *mdi,
                                            struct _event_data *data) {
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    mdi->channel[ch].expression = 127;
    mdi->channel[ch].pressure = 127;
    mdi->channel[ch].reg_data = 0xffff;
    mdi->channel[ch].pitch_range = 200;
    mdi->channel[ch].pitch = 0;
    mdi->channel[ch].pitch_adjust = 0;
    mdi->channel[ch].hold = 0;

    _WM_AdjustChannelVolumes(mdi, ch);
}