CotpConnection_getTpduSize(CotpConnection* self)
{
    return (1 << self->options.tpduSize);
}