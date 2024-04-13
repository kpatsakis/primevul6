void page_size_init(void)
{
    /* NOTE: we can always suppose that qemu_host_page_size >=
       TARGET_PAGE_SIZE */
    qemu_real_host_page_size = getpagesize();
    qemu_real_host_page_mask = -(intptr_t)qemu_real_host_page_size;
    if (qemu_host_page_size == 0) {
        qemu_host_page_size = qemu_real_host_page_size;
    }
    if (qemu_host_page_size < TARGET_PAGE_SIZE) {
        qemu_host_page_size = TARGET_PAGE_SIZE;
    }
    qemu_host_page_mask = -(intptr_t)qemu_host_page_size;
}