_WM_initMDI(void) {
    struct _mdi *mdi;

    mdi = malloc(sizeof(struct _mdi));
    memset(mdi, 0, (sizeof(struct _mdi)));

    mdi->extra_info.copyright = NULL;
    mdi->extra_info.mixer_options = _WM_MixerOptions;

    _WM_load_patch(mdi, 0x0000);

    mdi->events_size = MEM_CHUNK;
    mdi->events = malloc(mdi->events_size * sizeof(struct _event));
    mdi->event_count = 0;
    mdi->current_event = mdi->events;

    mdi->samples_to_mix = 0;
    mdi->extra_info.current_sample = 0;
    mdi->extra_info.total_midi_time = 0;
    mdi->extra_info.approx_total_samples = 0;

    mdi->dyn_vol = 1.0;
    mdi->dyn_vol_adjust = 0.0;
    mdi->dyn_vol_peak = 0;
    mdi->dyn_vol_to_reach = 1.0;

    mdi->is_type2 = 0;

    mdi->lyric = NULL;

    _WM_do_sysex_gm_reset(mdi, NULL);

    return (mdi);
}