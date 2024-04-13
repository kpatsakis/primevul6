static void pcnet_update_irq(PCNetState *s)
{
    int isr = 0;
    s->csr[0] &= ~0x0080;

#if 1
    if (((s->csr[0] & ~s->csr[3]) & 0x5f00) ||
        (((s->csr[4]>>1) & ~s->csr[4]) & 0x0115) ||
        (((s->csr[5]>>1) & s->csr[5]) & 0x0048))
#else
    if ((!(s->csr[3] & 0x4000) && !!(s->csr[0] & 0x4000)) /* BABL */ ||
        (!(s->csr[3] & 0x1000) && !!(s->csr[0] & 0x1000)) /* MISS */ ||
        (!(s->csr[3] & 0x0100) && !!(s->csr[0] & 0x0100)) /* IDON */ ||
        (!(s->csr[3] & 0x0200) && !!(s->csr[0] & 0x0200)) /* TINT */ ||
        (!(s->csr[3] & 0x0400) && !!(s->csr[0] & 0x0400)) /* RINT */ ||
        (!(s->csr[3] & 0x0800) && !!(s->csr[0] & 0x0800)) /* MERR */ ||
        (!(s->csr[4] & 0x0001) && !!(s->csr[4] & 0x0002)) /* JAB */ ||
        (!(s->csr[4] & 0x0004) && !!(s->csr[4] & 0x0008)) /* TXSTRT */ ||
        (!(s->csr[4] & 0x0010) && !!(s->csr[4] & 0x0020)) /* RCVO */ ||
        (!(s->csr[4] & 0x0100) && !!(s->csr[4] & 0x0200)) /* MFCO */ ||
        (!!(s->csr[5] & 0x0040) && !!(s->csr[5] & 0x0080)) /* EXDINT */ ||
        (!!(s->csr[5] & 0x0008) && !!(s->csr[5] & 0x0010)) /* MPINT */)
#endif
    {

        isr = CSR_INEA(s);
        s->csr[0] |= 0x0080;
    }

    if (!!(s->csr[4] & 0x0080) && CSR_INEA(s)) { /* UINT */
        s->csr[4] &= ~0x0080;
        s->csr[4] |= 0x0040;
        s->csr[0] |= 0x0080;
        isr = 1;
        trace_pcnet_user_int(s);
    }

#if 1
    if (((s->csr[5]>>1) & s->csr[5]) & 0x0500)
#else
    if ((!!(s->csr[5] & 0x0400) && !!(s->csr[5] & 0x0800)) /* SINT */ ||
        (!!(s->csr[5] & 0x0100) && !!(s->csr[5] & 0x0200)) /* SLPINT */ )
#endif
    {
        isr = 1;
        s->csr[0] |= 0x0080;
    }

    if (isr != s->isr) {
        trace_pcnet_isr_change(s, isr, s->isr);
    }
    qemu_set_irq(s->irq, isr);
    s->isr = isr;
}