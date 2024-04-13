void PackLinuxElf32::ARM_updateLoader(OutputFile * /*fo*/)
{
    set_te32(&elfout.ehdr.e_entry, sz_pack2 +
        linker->getSymbolOffset("_start") +
        get_te32(&elfout.phdr[0].p_vaddr));
}