template <class BoolDecoder>BaseEncoder *makeEncoder(bool threaded, bool start_workers) {
    TimingHarness::timing[0][TimingHarness::TS_MODEL_INIT_BEGIN] = TimingHarness::get_time_us();
    VP8ComponentEncoder<BoolDecoder> * retval = new VP8ComponentEncoder<BoolDecoder>(threaded, IsDecoderAns<BoolDecoder>::IS_ANS);
    TimingHarness::timing[0][TimingHarness::TS_MODEL_INIT] = TimingHarness::get_time_us();
    if (start_workers) {
        retval->registerWorkers(get_worker_threads(NUM_THREADS - 1), NUM_THREADS - 1);
    }
    return retval;
}