void qemu_flush_coalesced_mmio_buffer(void)
{
    if (kvm_enabled())
        kvm_flush_coalesced_mmio_buffer();
}