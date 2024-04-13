static int pcnet_tdte_poll(PCNetState *s)
{
    s->csr[34] = s->csr[35] = 0;
    if (s->tdra) {
        hwaddr cxda = s->tdra +
            (CSR_XMTRL(s) - CSR_XMTRC(s)) *
            (BCR_SWSTYLE(s) ? 16 : 8);
        int bad = 0;
        CHECK_TMD(cxda, bad);
        if (!bad) {
            if (CSR_CXDA(s) != cxda) {
                s->csr[60] = s->csr[34];
                s->csr[61] = s->csr[35];
                s->csr[62] = CSR_CXBC(s);
                s->csr[63] = CSR_CXST(s);
            }
            s->csr[34] = cxda & 0xffff;
            s->csr[35] = cxda >> 16;
#ifdef PCNET_DEBUG_X
            printf("pcnet: BAD TMD XDA=0x%08x\n", cxda);
#endif
        }
    }

    if (CSR_CXDA(s)) {
        struct pcnet_TMD tmd;

        TMDLOAD(&tmd, PHYSADDR(s,CSR_CXDA(s)));

        CSR_CXBC(s) = GET_FIELD(tmd.length, TMDL, BCNT);
        CSR_CXST(s) = tmd.status;
    } else {
        CSR_CXBC(s) = CSR_CXST(s) = 0;
    }

    return !!(CSR_CXST(s) & 0x8000);
}