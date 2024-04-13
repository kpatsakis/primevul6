bool qemu_ram_is_shared(RAMBlock *rb)
{
    return rb->flags & RAM_SHARED;
}