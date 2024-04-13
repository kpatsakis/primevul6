libxlConsoleCallback(libxl_ctx *ctx, libxl_event *ev, void *for_callback)
{
    virDomainObj *vm = for_callback;
    size_t i;
    virDomainChrDef *chr;
    char *console = NULL;
    int ret;

    virObjectLock(vm);
    for (i = 0; i < vm->def->nconsoles; i++) {
        chr = vm->def->consoles[i];

        if (i == 0 &&
            chr->targetType == VIR_DOMAIN_CHR_CONSOLE_TARGET_TYPE_SERIAL)
            chr = vm->def->serials[0];

        if (chr->source->type == VIR_DOMAIN_CHR_TYPE_PTY) {
            libxl_console_type console_type;

            console_type =
                (chr->deviceType == VIR_DOMAIN_CHR_DEVICE_TYPE_SERIAL ?
                 LIBXL_CONSOLE_TYPE_SERIAL : LIBXL_CONSOLE_TYPE_PV);
            ret = libxl_console_get_tty(ctx, ev->domid,
                                        chr->target.port, console_type,
                                        &console);
            if (!ret) {
                VIR_FREE(chr->source->data.file.path);
                if (console && console[0] != '\0')
                    chr->source->data.file.path = g_strdup(console);
            }
            VIR_FREE(console);
        }
    }
    for (i = 0; i < vm->def->nserials; i++) {
        chr = vm->def->serials[i];

        chr->info.alias = g_strdup_printf("serial%zd", i);
        if (chr->source->type == VIR_DOMAIN_CHR_TYPE_PTY) {
            if (chr->source->data.file.path)
                continue;
            ret = libxl_console_get_tty(ctx, ev->domid,
                                        chr->target.port,
                                        LIBXL_CONSOLE_TYPE_SERIAL,
                                        &console);
            if (!ret) {
                VIR_FREE(chr->source->data.file.path);
                if (console && console[0] != '\0')
                    chr->source->data.file.path = g_strdup(console);
            }
            VIR_FREE(console);
        }
    }
    virObjectUnlock(vm);
    libxl_event_free(ctx, ev);
}