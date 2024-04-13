getOptionsLength(CotpConnection* self)
{
    int optionsLength = 0;

    if (self->options.tpduSize != 0)
        optionsLength += 3;

    if (self->options.tSelDst.size != 0)
        optionsLength += (2 + self->options.tSelDst.size);

    if (self->options.tSelSrc.size != 0)
        optionsLength += (2 + self->options.tSelSrc.size);

    return optionsLength;
}