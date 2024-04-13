libxlDomainCreateChannelPTY(virDomainDef *def, libxl_ctx *ctx)
{
    libxl_device_channel *x_channels;
    virDomainChrDef *chr;
    size_t i;
    int nchannels;

    x_channels = libxl_device_channel_list(ctx, def->id, &nchannels);
    if (!x_channels)
        return;

    for (i = 0; i < def->nchannels; i++) {
        libxl_channelinfo channelinfo;
        int ret;

        chr = def->channels[i];
        if (chr->source->type != VIR_DOMAIN_CHR_TYPE_PTY)
            continue;

        ret = libxl_device_channel_getinfo(ctx, def->id, &x_channels[i],
                                           &channelinfo);

        if (!ret && channelinfo.u.pty.path &&
            *channelinfo.u.pty.path != '\0') {
                VIR_FREE(chr->source->data.file.path);
                chr->source->data.file.path = g_strdup(channelinfo.u.pty.path);
            }
    }

    for (i = 0; i < nchannels; i++)
        libxl_device_channel_dispose(&x_channels[i]);
}