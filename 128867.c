libxlDomainStartRestore(libxlDriverPrivate *driver,
                        virDomainObj *vm,
                        bool start_paused,
                        int restore_fd,
                        uint32_t restore_ver)
{
    return libxlDomainStart(driver, vm, start_paused,
                            restore_fd, restore_ver);
}