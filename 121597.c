GenericWorker * get_worker_threads(unsigned int num_workers) {
    // in this case decoding is asymmetric to encoding, just forget the assert
    if (NUM_THREADS < 2) {
        return NULL;
    }
    GenericWorker* retval = GenericWorker::get_n_worker_threads(num_workers);
    TimingHarness::timing[0][TimingHarness::TS_THREAD_STARTED] = TimingHarness::get_time_us();

    return retval;
}