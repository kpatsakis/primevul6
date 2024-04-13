Elf32_Sym const *PackLinuxElf32::elf_lookup(char const *name) const
{
    if (hashtab && dynsym && dynstr) {
        unsigned const nbucket = get_te32(&hashtab[0]);
        unsigned const *const buckets = &hashtab[2];
        unsigned const *const chains = &buckets[nbucket];
        unsigned const m = elf_hash(name) % nbucket;
        unsigned si;
        for (si= get_te32(&buckets[m]); 0!=si; si= get_te32(&chains[si])) {
            char const *const p= get_dynsym_name(si, (unsigned)-1);
            if (0==strcmp(name, p)) {
                return &dynsym[si];
            }
        }
    }
    if (gashtab && dynsym && dynstr) {
        unsigned const n_bucket = get_te32(&gashtab[0]);
        unsigned const symbias  = get_te32(&gashtab[1]);
        unsigned const n_bitmask = get_te32(&gashtab[2]);
        unsigned const gnu_shift = get_te32(&gashtab[3]);
        unsigned const *const bitmask = &gashtab[4];
        unsigned const *const buckets = &bitmask[n_bitmask];
        unsigned const *const hasharr = &buckets[n_bucket];

        unsigned const h = gnu_hash(name);
        unsigned const hbit1 = 037& h;
        unsigned const hbit2 = 037& (h>>gnu_shift);
        unsigned const w = get_te32(&bitmask[(n_bitmask -1) & (h>>5)]);

        if (1& (w>>hbit1) & (w>>hbit2)) {
            unsigned bucket = get_te32(&buckets[h % n_bucket]);
            if (0!=bucket) {
                Elf32_Sym const *dsp = &dynsym[bucket];
                unsigned const *hp = &hasharr[bucket - symbias];

                do if (0==((h ^ get_te32(hp))>>1)) {
                    unsigned st_name = get_te32(&dsp->st_name);
                    char const *const p = get_str_name(st_name, (unsigned)-1);
                    if (0==strcmp(name, p)) {
                        return dsp;
                    }
                } while (++dsp, 0==(1u& get_te32(hp++)));
            }
        }
    }
    return 0;

}