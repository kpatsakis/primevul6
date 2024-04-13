int rom_add_elf_program(const char *name, void *data, size_t datasize,
                        size_t romsize, hwaddr addr, AddressSpace *as)
{
    Rom *rom;

    rom           = g_malloc0(sizeof(*rom));
    rom->name     = g_strdup(name);
    rom->addr     = addr;
    rom->datasize = datasize;
    rom->romsize  = romsize;
    rom->data     = data;
    rom->as       = as;
    rom_insert(rom);
    return 0;
}