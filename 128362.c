size_t qemu_ram_pagesize(RAMBlock *rb)
{
    return rb->page_size;
}