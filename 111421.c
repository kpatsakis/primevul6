void _WM_ResetToStart(struct _mdi *mdi) {
    struct _event * event = NULL;

    mdi->current_event = mdi->events;
    mdi->samples_to_mix = 0;
    mdi->extra_info.current_sample = 0;

    _WM_do_sysex_gm_reset(mdi, NULL);

    /* Ensure last event is NULL */
    _WM_CheckEventMemoryPool(mdi);
    mdi->events[mdi->event_count].do_event = NULL;
    mdi->events[mdi->event_count].event_data.channel = 0;
    mdi->events[mdi->event_count].event_data.data.value = 0;
    mdi->events[mdi->event_count].samples_to_next = 0;

    if (_WM_MixerOptions & WM_MO_STRIPSILENCE) {
        event = mdi->events;
        /* Scan for first note on removing any samples as we go */
        if (event->do_event != *_WM_do_note_on) {
            do {
                if (event->samples_to_next != 0) {
                    mdi->extra_info.approx_total_samples -= event->samples_to_next;
                    event->samples_to_next = 0;
                }
                event++;
                if (event == NULL) break;
            } while (event->do_event != *_WM_do_note_on);
        }

        /* Reverse scan for last note off removing any samples as we go */
        event = &mdi->events[mdi->event_count - 1];
        if (event->do_event != *_WM_do_note_off) {
            do {
                mdi->extra_info.approx_total_samples -= event->samples_to_next;
                event->samples_to_next = 0;
                if (event == mdi->events) break; /* just to be safe */
                event--;
            } while (event->do_event != *_WM_do_note_off);
        }
        mdi->extra_info.approx_total_samples -= event->samples_to_next;
        event->samples_to_next = 0;
    }
}