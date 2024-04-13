static uint8_t *copy_region(struct uc_struct *uc, MemoryRegion *mr)
{
    uint8_t *block = (uint8_t *)g_malloc0((size_t)int128_get64(mr->size));
    if (block != NULL) {
        uc_err err =
            uc_mem_read(uc, mr->addr, block, (size_t)int128_get64(mr->size));
        if (err != UC_ERR_OK) {
            free(block);
            block = NULL;
        }
    }

    return block;
}