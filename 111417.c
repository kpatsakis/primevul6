void _WM_do_sysex_gm_reset(struct _mdi *mdi, struct _event_data *data) {
    int i;

    if (data != NULL) {
        MIDI_EVENT_DEBUG(__FUNCTION__,data->channel, data->data.value);
    } else {
        MIDI_EVENT_DEBUG(__FUNCTION__,0, 0);
    }

    for (i = 0; i < 16; i++) {
        mdi->channel[i].bank = 0;
        if (i != 9) {
            mdi->channel[i].patch = _WM_get_patch_data(mdi, 0);
        } else {
            mdi->channel[i].patch = NULL;
        }
        mdi->channel[i].hold = 0;
        mdi->channel[i].volume = 100;
        mdi->channel[i].pressure = 127;
        mdi->channel[i].expression = 127;
        mdi->channel[i].balance = 64;
        mdi->channel[i].pan = 64;
        mdi->channel[i].pitch = 0;
        mdi->channel[i].pitch_range = 200;
        mdi->channel[i].reg_data = 0xFFFF;
        mdi->channel[i].isdrum = 0;
    }
    /* I would not expect notes to be active when this event
     triggers but we'll adjust active notes as well just in case */
    _WM_AdjustChannelVolumes(mdi,16); // A setting > 15 adjusts all channels

    mdi->channel[9].isdrum = 1;
}