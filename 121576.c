BaseDecoder *makeDecoder(bool threaded, bool start_workers, bool ans) {
    if (ans) {
#ifdef ENABLE_ANS_EXPERIMENTAL
        return makeBoth<ANSBoolReader>(threaded, start_workers);
#else
        always_assert(false && "ANS compile flag not selected");
#endif
    }
    return makeBoth<VPXBoolReader>(threaded, start_workers);
}