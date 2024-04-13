void PackLinuxElf32::updateLoader(OutputFile * /*fo*/)
{
    unsigned start = linker->getSymbolOffset("_start");
    unsigned vbase = get_te32(&elfout.phdr[0].p_vaddr);
    set_te32(&elfout.ehdr.e_entry, start + sz_pack2 + vbase);
}