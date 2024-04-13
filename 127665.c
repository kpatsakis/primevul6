static inline void pcnet_tmd_store(PCNetState *s, const struct pcnet_TMD *tmd,
                                   hwaddr addr)
{
    if (!BCR_SSIZE32(s)) {
        struct {
            uint32_t tbadr;
            int16_t length;
            int16_t status;
        } xda;
        xda.tbadr = cpu_to_le32((tmd->tbadr & 0xffffff) |
                                ((tmd->status & 0xff00) << 16));
        xda.length = cpu_to_le16(tmd->length);
        xda.status = cpu_to_le16(tmd->misc >> 16);
        s->phys_mem_write(s->dma_opaque, addr, (void *)&xda, sizeof(xda), 0);
    } else {
        struct {
            uint32_t tbadr;
            int16_t length;
            int16_t status;
            uint32_t misc;
            uint32_t res;
        } xda;
        xda.tbadr = cpu_to_le32(tmd->tbadr);
        xda.length = cpu_to_le16(tmd->length);
        xda.status = cpu_to_le16(tmd->status);
        xda.misc = cpu_to_le32(tmd->misc);
        xda.res = cpu_to_le32(tmd->res);
        if (BCR_SWSTYLE(s) == 3) {
            uint32_t tmp = xda.tbadr;
            xda.tbadr = xda.misc;
            xda.misc = tmp;
        }
        s->phys_mem_write(s->dma_opaque, addr, (void *)&xda, sizeof(xda), 0);
    }
}