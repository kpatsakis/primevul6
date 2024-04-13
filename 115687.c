static void rom_reset(void *unused)
{
    Rom *rom;

    QTAILQ_FOREACH(rom, &roms, next) {
        if (rom->fw_file) {
            continue;
        }
        if (rom->data == NULL) {
            continue;
        }
        if (rom->mr) {
            void *host = memory_region_get_ram_ptr(rom->mr);
            memcpy(host, rom->data, rom->datasize);
        } else {
            address_space_write_rom(rom->as, rom->addr, MEMTXATTRS_UNSPECIFIED,
                                    rom->data, rom->datasize);
        }
        if (rom->isrom) {
            /* rom needs to be written only once */
            g_free(rom->data);
            rom->data = NULL;
        }
        /*
         * The rom loader is really on the same level as firmware in the guest
         * shadowing a ROM into RAM. Such a shadowing mechanism needs to ensure
         * that the instruction cache for that new region is clear, so that the
         * CPU definitely fetches its instructions from the just written data.
         */
        cpu_flush_icache_range(rom->addr, rom->datasize);
    }
}