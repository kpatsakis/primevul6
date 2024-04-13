static int midi_setup_control(struct _mdi *mdi, uint8_t channel,
                              uint8_t controller, uint8_t setting) {
    void (*tmp_event)(struct _mdi *mdi, struct _event_data *data) = NULL;
    MIDI_EVENT_DEBUG(__FUNCTION__,channel, controller);

    switch (controller) {
        /*
         **********************************************************************
         FIXME: Need to add dummy events for MIDI events we don't support.
         There is no reason not to store unsupported events in light of our
         out to midi option.
         **********************************************************************
         */
        case 0:
            tmp_event = *_WM_do_control_bank_select;
            mdi->channel[channel].bank = setting;
            break;
        case 6:
            tmp_event = *_WM_do_control_data_entry_course;
            break;
        case 7:
            tmp_event = *_WM_do_control_channel_volume;
            mdi->channel[channel].volume = setting;
            break;
        case 8:
            tmp_event = *_WM_do_control_channel_balance;
            break;
        case 10:
            tmp_event = *_WM_do_control_channel_pan;
            break;
        case 11:
            tmp_event = *_WM_do_control_channel_expression;
            break;
        case 38:
            tmp_event = *_WM_do_control_data_entry_fine;
            break;
        case 64:
            tmp_event = *_WM_do_control_channel_hold;
            break;
        case 96:
            tmp_event = *_WM_do_control_data_increment;
            break;
        case 97:
            tmp_event = *_WM_do_control_data_decrement;
            break;
        case 98:
            tmp_event = *_WM_do_control_non_registered_param_fine;
            break;
        case 99:
            tmp_event = *_WM_do_control_non_registered_param_course;
            break;
        case 100:
            tmp_event = *_WM_do_control_registered_param_fine;
            break;
        case 101:
            tmp_event = *_WM_do_control_registered_param_course;
            break;
        case 120:
            tmp_event = *_WM_do_control_channel_sound_off;
            break;
        case 121:
            tmp_event = *_WM_do_control_channel_controllers_off;
            break;
        case 123:
            tmp_event = *_WM_do_control_channel_notes_off;
            break;
        default:
            tmp_event = *_WM_do_control_dummy;
            break;
    }

    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = tmp_event;
    mdi->events[mdi->event_count].event_data.channel = channel;
    if (tmp_event != *_WM_do_control_dummy) {
        mdi->events[mdi->event_count].event_data.data.value = setting;
    } else {
        mdi->events[mdi->event_count].event_data.data.value = (controller << 8) | setting;
    }
    mdi->events[mdi->event_count].samples_to_next = 0;
    mdi->event_count++;
    return (0);
}