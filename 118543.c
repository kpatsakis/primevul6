void PackLinuxElf64::updateLoader(OutputFile * /*fo*/)
{
    if (xct_off) {
        return;  // FIXME elfout has no values at all
    }
    upx_uint64_t const vbase = get_te64(&elfout.phdr[0].p_vaddr);
    unsigned start = linker->getSymbolOffset("_start");

    if (get_te16(&elfout.ehdr.e_machine)==Elf64_Ehdr::EM_PPC64
    &&  elfout.ehdr.e_ident[Elf64_Ehdr::EI_DATA]==Elf64_Ehdr::ELFDATA2MSB) {
        unsigned descr = linker->getSymbolOffset("entry_descr");

        // External relocation of PPC64 function descriptor.
        upx_uint64_t dot_entry = start + sz_pack2 + vbase;
        upx_byte *p = getLoader();

        set_te64(&p[descr], dot_entry);
        set_te64(&elfout.ehdr.e_entry, descr + sz_pack2 + vbase);
    }
    else {
        set_te64(&elfout.ehdr.e_entry, start + sz_pack2 + vbase);
    }
}