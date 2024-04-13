static void _WM_CheckEventMemoryPool(struct _mdi *mdi) {
    if ((mdi->event_count + 1) >= mdi->events_size) {
        mdi->events_size += MEM_CHUNK;
        mdi->events = realloc(mdi->events,
                              (mdi->events_size * sizeof(struct _event)));
    }
}