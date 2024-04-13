template <class BoolDecoder>VP8ComponentDecoder<BoolDecoder> *makeBoth(bool threaded, bool start_workers) {
    VP8ComponentDecoder<BoolDecoder> *retval = new VP8ComponentDecoder<BoolDecoder>(threaded);
    TimingHarness::timing[0][TimingHarness::TS_MODEL_INIT] = TimingHarness::get_time_us();
    if (start_workers) {
        retval->registerWorkers(get_worker_threads(
                                    NUM_THREADS
                                    ),
                                NUM_THREADS
            );
    }
    return retval;
}