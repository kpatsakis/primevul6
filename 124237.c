PackLinuxElf64::invert_pt_dynamic(Elf64_Dyn const *dynp)
{
    if (dt_table[Elf64_Dyn::DT_NULL]) {
        return;  // not 1st time; do not change upx_dt_init
    }
    Elf64_Dyn const *const dynp0 = dynp;
    unsigned ndx = 1+ 0;
    if (dynp)
    for (; ; ++ndx, ++dynp) {
        upx_uint64_t const d_tag = get_te64(&dynp->d_tag);
        if (d_tag>>32) { // outrageous
            char msg[50]; snprintf(msg, sizeof(msg),
                "bad Elf64_Dyn[%d].d_tag %#lx", -1+ ndx, (long unsigned)d_tag);
            throwCantPack(msg);
        }
        if (d_tag < DT_NUM) {
            if (Elf64_Dyn::DT_NEEDED != d_tag
            &&  dt_table[d_tag]
            &&    get_te64(&dynp->d_val)
               != get_te64(&dynp0[-1+ dt_table[d_tag]].d_val)) {
                char msg[50]; snprintf(msg, sizeof(msg),
                    "duplicate DT_%#x: [%#x] [%#x]",
                    (unsigned)d_tag, -1+ dt_table[d_tag], -1+ ndx);
                throwCantPack(msg);
            }
            dt_table[d_tag] = ndx;
        }
        if (Elf64_Dyn::DT_NULL == d_tag) {
            break;  // check here so that dt_table[DT_NULL] is set
        }
    }
    upx_dt_init = 0;
         if (dt_table[Elf64_Dyn::DT_INIT])          upx_dt_init = Elf64_Dyn::DT_INIT;
    else if (dt_table[Elf64_Dyn::DT_PREINIT_ARRAY]) upx_dt_init = Elf64_Dyn::DT_PREINIT_ARRAY;
    else if (dt_table[Elf64_Dyn::DT_INIT_ARRAY])    upx_dt_init = Elf64_Dyn::DT_INIT_ARRAY;

    unsigned const z_str = dt_table[Elf64_Dyn::DT_STRSZ];
    if (z_str) {
        strtab_end = get_te64(&dynp0[-1+ z_str].d_val);
        if ((u64_t)file_size <= strtab_end) { // FIXME: weak
            char msg[50]; snprintf(msg, sizeof(msg),
                "bad DT_STRSZ %#x", strtab_end);
            throwCantPack(msg);
        }
    }
    // DT_SYMTAB has no designated length.
    // End it when next area else starts; often DT_STRTAB.  (FIXME)
    unsigned const x_sym = dt_table[Elf64_Dyn::DT_SYMTAB];
    unsigned const x_str = dt_table[Elf64_Dyn::DT_STRTAB];
    if (x_sym && x_str) {
        upx_uint64_t const v_sym = get_te64(&dynp0[-1+ x_sym].d_val);
        upx_uint64_t const v_str = get_te64(&dynp0[-1+ x_str].d_val);
        unsigned const  z_sym = dt_table[Elf64_Dyn::DT_SYMENT];
        unsigned const sz_sym = !z_sym ? sizeof(Elf64_Sym)
            : get_te64(&dynp0[-1+ z_sym].d_val);
        if (sz_sym < sizeof(Elf64_Sym)) {
            char msg[50]; snprintf(msg, sizeof(msg),
                "bad DT_SYMENT %x", sz_sym);
            throwCantPack(msg);
        }
        if (v_sym < v_str) {
            symnum_end = (v_str - v_sym) / sz_sym;
        }
        if (symnum_end < 1) {
            throwCantPack("bad DT_SYMTAB");
        }
    }
    // DT_HASH often ends at DT_SYMTAB
    unsigned const v_hsh = elf_unsigned_dynamic(Elf64_Dyn::DT_HASH);
    if (v_hsh && file_image) {
        hashtab = (unsigned const *)elf_find_dynamic(Elf64_Dyn::DT_HASH);
        if (!hashtab) {
            char msg[40]; snprintf(msg, sizeof(msg),
               "bad DT_HASH %#x", v_hsh);
            throwCantPack(msg);
        }
        unsigned const nbucket = get_te32(&hashtab[0]);
        unsigned const *const buckets = &hashtab[2];
        unsigned const *const chains = &buckets[nbucket]; (void)chains;

        unsigned const v_sym = !x_sym ? 0 : get_te32(&dynp0[-1+ x_sym].d_val);
        if (!nbucket || !v_sym
        || (nbucket>>31) || (file_size/sizeof(unsigned)) <= (2*nbucket)  // FIXME: weak
        || ((v_hsh < v_sym) && (v_sym - v_hsh) < (sizeof(unsigned)*2  // headers
                + sizeof(*buckets)*nbucket  // buckets
                + sizeof(*chains) *nbucket  // chains
           ))
        ) {
            char msg[90]; snprintf(msg, sizeof(msg),
                "bad DT_HASH nbucket=%#x  len=%#x",
                nbucket, (v_sym - v_hsh));
            throwCantPack(msg);
        }
    }
    // DT_GNU_HASH often ends at DT_SYMTAB;  FIXME: not for Android?
    unsigned const v_gsh = elf_unsigned_dynamic(Elf64_Dyn::DT_GNU_HASH);
    if (v_gsh && file_image) {
        gashtab = (unsigned const *)elf_find_dynamic(Elf64_Dyn::DT_GNU_HASH);
        if (!gashtab) {
            char msg[40]; snprintf(msg, sizeof(msg),
               "bad DT_GNU_HASH %#x", v_gsh);
            throwCantPack(msg);
        }
        unsigned const n_bucket = get_te32(&gashtab[0]);
        unsigned const n_bitmask = get_te32(&gashtab[2]);
        unsigned const gnu_shift = get_te32(&gashtab[3]);
        upx_uint64_t const *const bitmask = (upx_uint64_t const *)(void const *)&gashtab[4];
        unsigned     const *const buckets = (unsigned const *)&bitmask[n_bitmask];
        unsigned     const *const hasharr = &buckets[n_bucket]; (void)hasharr;
      //unsigned     const *const gashend = &hasharr[n_bucket];  // minimum

        upx_uint64_t const v_sym = !x_sym ? 0 : get_te64(&dynp0[-1+ x_sym].d_val);
        if (!n_bucket || !n_bitmask || !v_sym
        || (-1+ n_bitmask) & n_bitmask  // not a power of 2
        || 8*sizeof(upx_uint64_t) <= gnu_shift  // shifted result always == 0
        || (n_bucket>>30)  // fie on fuzzers
        || (n_bitmask>>30)
        || (file_size/sizeof(unsigned)) <= ((sizeof(*bitmask)/sizeof(unsigned))*n_bitmask + 2*n_bucket)  // FIXME: weak
        // FIXME: next test does work for Android?
        || ((v_gsh < v_sym) && (v_sym - v_gsh) < (sizeof(unsigned)*4  // headers
                + sizeof(*bitmask)*n_bitmask  // bitmask
                + sizeof(*buckets)*n_bucket  // buckets
                + sizeof(*hasharr)*n_bucket  // hasharr
            ))
        ) {
            char msg[90]; snprintf(msg, sizeof(msg),
                "bad DT_GNU_HASH n_bucket=%#x  n_bitmask=%#x  len=%#lx",
                n_bucket, n_bitmask, (long unsigned)(v_sym - v_gsh));
            throwCantPack(msg);
        }
    }
    unsigned const e_shstrndx = get_te16(&ehdri.e_shstrndx);
    if (e_shnum <= e_shstrndx
    &&  !(0==e_shnum && 0==e_shstrndx) ) {
        char msg[40]; snprintf(msg, sizeof(msg),
            "bad .e_shstrndx %d >= .e_shnum %d", e_shstrndx, e_shnum);
        throwCantPack(msg);
    }
}