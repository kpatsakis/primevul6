static inline int padr_match(PCNetState *s, const uint8_t *buf, int size)
{
    struct qemu_ether_header *hdr = (void *)buf;
    uint8_t padr[6] = {
        s->csr[12] & 0xff, s->csr[12] >> 8,
        s->csr[13] & 0xff, s->csr[13] >> 8,
        s->csr[14] & 0xff, s->csr[14] >> 8
    };
    int result = (!CSR_DRCVPA(s)) && !memcmp(hdr->ether_dhost, padr, 6);
#ifdef PCNET_DEBUG_MATCH
    printf("packet dhost=%02x:%02x:%02x:%02x:%02x:%02x, "
           "padr=%02x:%02x:%02x:%02x:%02x:%02x\n",
           hdr->ether_dhost[0],hdr->ether_dhost[1],hdr->ether_dhost[2],
           hdr->ether_dhost[3],hdr->ether_dhost[4],hdr->ether_dhost[5],
           padr[0],padr[1],padr[2],padr[3],padr[4],padr[5]);
    printf("padr_match result=%d\n", result);
#endif
    return result;
}