static inline void pcnet_rmd_load(PCNetState *s, struct pcnet_RMD *rmd,
                                  hwaddr addr)
{
    if (!BCR_SSIZE32(s)) {
        struct {
            uint32_t rbadr;
            int16_t buf_length;
            int16_t msg_length;
	} rda;
        s->phys_mem_read(s->dma_opaque, addr, (void *)&rda, sizeof(rda), 0);
        rmd->rbadr = le32_to_cpu(rda.rbadr) & 0xffffff;
        rmd->buf_length = le16_to_cpu(rda.buf_length);
        rmd->status = (le32_to_cpu(rda.rbadr) >> 16) & 0xff00;
        rmd->msg_length = le16_to_cpu(rda.msg_length);
        rmd->res = 0;
    } else {
        s->phys_mem_read(s->dma_opaque, addr, (void *)rmd, sizeof(*rmd), 0);
        le32_to_cpus(&rmd->rbadr);
        le16_to_cpus((uint16_t *)&rmd->buf_length);
        le16_to_cpus((uint16_t *)&rmd->status);
        le32_to_cpus(&rmd->msg_length);
        le32_to_cpus(&rmd->res);
        if (BCR_SWSTYLE(s) == 3) {
            uint32_t tmp = rmd->rbadr;
            rmd->rbadr = rmd->msg_length;
            rmd->msg_length = tmp;
        }
    }
}