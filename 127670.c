static void pcnet_s_reset(PCNetState *s)
{
    trace_pcnet_s_reset(s);

    s->rdra = 0;
    s->tdra = 0;
    s->rap = 0;

    s->bcr[BCR_BSBC] &= ~0x0080;

    s->csr[0]   = 0x0004;
    s->csr[3]   = 0x0000;
    s->csr[4]   = 0x0115;
    s->csr[5]   = 0x0000;
    s->csr[6]   = 0x0000;
    s->csr[8]   = 0;
    s->csr[9]   = 0;
    s->csr[10]  = 0;
    s->csr[11]  = 0;
    s->csr[12]  = le16_to_cpu(((uint16_t *)&s->prom[0])[0]);
    s->csr[13]  = le16_to_cpu(((uint16_t *)&s->prom[0])[1]);
    s->csr[14]  = le16_to_cpu(((uint16_t *)&s->prom[0])[2]);
    s->csr[15] &= 0x21c4;
    s->csr[72]  = 1;
    s->csr[74]  = 1;
    s->csr[76]  = 1;
    s->csr[78]  = 1;
    s->csr[80]  = 0x1410;
    s->csr[88]  = 0x1003;
    s->csr[89]  = 0x0262;
    s->csr[94]  = 0x0000;
    s->csr[100] = 0x0200;
    s->csr[103] = 0x0105;
    s->csr[112] = 0x0000;
    s->csr[114] = 0x0000;
    s->csr[122] = 0x0000;
    s->csr[124] = 0x0000;

    s->tx_busy = 0;
}