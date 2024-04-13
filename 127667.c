static inline void pcnet_rmd_store(PCNetState *s, struct pcnet_RMD *rmd,
                                   hwaddr addr)
{
    if (!BCR_SSIZE32(s)) {
        struct {
            uint32_t rbadr;
            int16_t buf_length;
            int16_t msg_length;
        } rda;
        rda.rbadr = cpu_to_le32((rmd->rbadr & 0xffffff) |
                                ((rmd->status & 0xff00) << 16));
        rda.buf_length = cpu_to_le16(rmd->buf_length);
        rda.msg_length = cpu_to_le16(rmd->msg_length);
        s->phys_mem_write(s->dma_opaque, addr, (void *)&rda, sizeof(rda), 0);
    } else {
        struct {
            uint32_t rbadr;
            int16_t buf_length;
            int16_t status;
            uint32_t msg_length;
            uint32_t res;
        } rda;
        rda.rbadr = cpu_to_le32(rmd->rbadr);
        rda.buf_length = cpu_to_le16(rmd->buf_length);
        rda.status = cpu_to_le16(rmd->status);
        rda.msg_length = cpu_to_le32(rmd->msg_length);
        rda.res = cpu_to_le32(rmd->res);
        if (BCR_SWSTYLE(s) == 3) {
            uint32_t tmp = rda.rbadr;
            rda.rbadr = rda.msg_length;
            rda.msg_length = tmp;
        }
        s->phys_mem_write(s->dma_opaque, addr, (void *)&rda, sizeof(rda), 0);
    }
}