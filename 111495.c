void _WM_freeMDI(struct _mdi *mdi) {
    struct _sample *tmp_sample;
    uint32_t i;

    if (mdi->patch_count != 0) {
        _WM_Lock(&_WM_patch_lock);
        for (i = 0; i < mdi->patch_count; i++) {
            mdi->patches[i]->inuse_count--;
            if (mdi->patches[i]->inuse_count == 0) {
                /* free samples here */
                while (mdi->patches[i]->first_sample) {
                    tmp_sample = mdi->patches[i]->first_sample->next;
                    free(mdi->patches[i]->first_sample->data);
                    free(mdi->patches[i]->first_sample);
                    mdi->patches[i]->first_sample = tmp_sample;
                }
                mdi->patches[i]->loaded = 0;
            }
        }
        _WM_Unlock(&_WM_patch_lock);
        free(mdi->patches);
    }

    if (mdi->event_count != 0) {
        for (i = 0; i < mdi->event_count; i++) {
            /* Free up the string event storage */
            if (mdi->events[i].do_event == _WM_do_meta_text) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_text) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_copyright) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_trackname) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_instrumentname) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_lyric) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_marker) {
                free(mdi->events[i].event_data.data.string);
            } else if (mdi->events[i].do_event == _WM_do_meta_cuepoint) {
                free(mdi->events[i].event_data.data.string);
            }
        }
    }

    free(mdi->events);
    _WM_free_reverb(mdi->reverb);
    free(mdi->mix_buffer);
    free(mdi);
}