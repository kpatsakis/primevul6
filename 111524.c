uc_err uc_emu_stop(uc_engine *uc)
{
    UC_INIT(uc);

    if (uc->emulation_done) {
        return UC_ERR_OK;
    }

    uc->stop_request = true;
    // TODO: make this atomic somehow?
    if (uc->cpu) {
        // exit the current TB
        cpu_exit(uc->cpu);
    }

    return UC_ERR_OK;
}