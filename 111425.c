void _WM_do_control_channel_sound_off(struct _mdi *mdi,
                                      struct _event_data *data) {
    struct _note *note_data = mdi->note;
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if (note_data) {
        do {
            if ((note_data->noteid >> 8) == ch) {
                note_data->active = 0;
                if (note_data->replay) {
                    note_data->replay = NULL;
                }
            }
            note_data = note_data->next;
        } while (note_data);
    }
}