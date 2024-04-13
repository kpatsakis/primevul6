void phys_mem_set_alloc(void *(*alloc)(size_t, uint64_t *align))
{
    phys_mem_alloc = alloc;
}