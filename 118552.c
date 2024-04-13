PackLinuxElf32::generateElfHdr(
    OutputFile *fo,
    void const *proto,
    unsigned const brka
)
{
    cprElfHdr2 *const h2 = (cprElfHdr2 *)(void *)&elfout;
    cprElfHdr3 *const h3 = (cprElfHdr3 *)(void *)&elfout;
    memcpy(h3, proto, sizeof(*h3));  // reads beyond, but OK
    h3->ehdr.e_type = ehdri.e_type;  // ET_EXEC vs ET_DYN (gcc -pie -fPIC)
    h3->ehdr.e_ident[Elf32_Ehdr::EI_OSABI] = ei_osabi;
    unsigned phnum_o = get_te16(&h2->ehdr.e_phnum);
    if (Elf32_Ehdr::EM_MIPS==e_machine) { // MIPS R3000  FIXME
        h3->ehdr.e_ident[Elf32_Ehdr::EI_OSABI] = Elf32_Ehdr::ELFOSABI_NONE;
        h3->ehdr.e_flags = ehdri.e_flags;
    }

    assert(get_te32(&h2->ehdr.e_phoff)     == sizeof(Elf32_Ehdr));
                         h2->ehdr.e_shoff = 0;
    assert(get_te16(&h2->ehdr.e_ehsize)    == sizeof(Elf32_Ehdr));
    assert(get_te16(&h2->ehdr.e_phentsize) == sizeof(Elf32_Phdr));
           set_te16(&h2->ehdr.e_shentsize, sizeof(Elf32_Shdr));
    if (o_elf_shnum) {
                        h2->ehdr.e_shnum = o_elf_shnum;
                        h2->ehdr.e_shstrndx = o_elf_shnum - 1;
    }
    else {
                        h2->ehdr.e_shnum = 0;
                        h2->ehdr.e_shstrndx = 0;
    }

    sz_elf_hdrs = sizeof(*h2) - sizeof(linfo);  // default
    if (gnu_stack) {
        sz_elf_hdrs += sizeof(Elf32_Phdr);
        memcpy(&h2->phdr[phnum_o++], gnu_stack, sizeof(*gnu_stack));
        set_te16(&h2->ehdr.e_phnum, phnum_o);
    }
    o_binfo =  sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr)*phnum_o + sizeof(l_info) + sizeof(p_info);
    set_te32(&h2->phdr[0].p_filesz, sizeof(*h2));  // + identsize;
              h2->phdr[0].p_memsz = h2->phdr[0].p_filesz;

    for (unsigned j=0; j < phnum_o; ++j) {
        if (PT_LOAD32==get_te32(&h3->phdr[j].p_type)) {
            set_te32(&h3->phdr[j].p_align, page_size);
        }
    }

    // Info for OS kernel to set the brk()
    if (brka) {
        // linux-2.6.14 binfmt_elf.c: SIGKILL if (0==.p_memsz) on a page boundary
        upx_uint32_t lo_va_user = ~0u;  // infinity
        upx_uint32_t memsz(0);
        for (int j= e_phnum; --j>=0; ) {
            if (PT_LOAD32 == get_te32(&phdri[j].p_type)) {
                upx_uint32_t const vaddr = get_te32(&phdri[j].p_vaddr);
                lo_va_user = umin(lo_va_user, vaddr);
                if (vaddr == lo_va_user) {
                    memsz = get_te32(&phdri[j].p_memsz);
                }
            }
        }
        set_te32(&h2->phdr[0].p_paddr, lo_va_user);
        set_te32(&h2->phdr[0].p_vaddr, lo_va_user);
        unsigned const brkb = page_mask & (~page_mask +
            get_te32(&h2->phdr[0].p_vaddr) + memsz);
        set_te32(&h2->phdr[1].p_type, PT_LOAD32);  // be sure
        h2->phdr[1].p_offset = 0;
        set_te32(&h2->phdr[1].p_vaddr, brkb);
        set_te32(&h2->phdr[1].p_paddr, brkb);
        h2->phdr[1].p_filesz = 0;
        set_te32(&h2->phdr[1].p_memsz, brka - brkb);
        set_te32(&h2->phdr[1].p_flags, Elf32_Phdr::PF_R | Elf32_Phdr::PF_W);
    }
    if (ph.format==getFormat()) {
        assert((2u+ !!gnu_stack) == phnum_o);
        set_te32(&h2->phdr[0].p_flags, ~Elf32_Phdr::PF_W & get_te32(&h2->phdr[0].p_flags));
        if (!gnu_stack) {
            memset(&h2->linfo, 0, sizeof(h2->linfo));
            fo->write(h2, sizeof(*h2));
        }
        else {
            memset(&h3->linfo, 0, sizeof(h3->linfo));
            fo->write(h3, sizeof(*h3));
        }
    }
    else {
        assert(false);  // unknown ph.format, PackLinuxElf32
    }
}