CotpConnection_setTpduSize(CotpConnection* self, int tpduSize /* in byte */)
{
    int newTpduSize = 1;

    if (tpduSize > COTP_MAX_TPDU_SIZE)
        tpduSize = COTP_MAX_TPDU_SIZE;

    while ((1 << newTpduSize) < tpduSize)
        newTpduSize++;

    if ((1 << newTpduSize) > tpduSize)
        newTpduSize--;

    self->options.tpduSize = newTpduSize;
}