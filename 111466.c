void _WM_do_control_data_entry_fine(struct _mdi *mdi,
                                       struct _event_data *data) {
    uint8_t ch = data->channel;
    int data_tmp;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if ((mdi->channel[ch].reg_non == 0)
      && (mdi->channel[ch].reg_data == 0x0000)) { /* Pitch Bend Range */
        data_tmp = mdi->channel[ch].pitch_range / 100;
        mdi->channel[ch].pitch_range = (data_tmp * 100) + data->data.value;
    /*  printf("Data Entry Fine: pitch_range: %i\n\r",mdi->channel[ch].pitch_range);*/
    /*  printf("Data Entry Fine: data: %li\n\r", data->data.value);*/
    }
}