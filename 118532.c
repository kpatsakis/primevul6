PackOpenBSDElf32x86::generateElfHdr(
    OutputFile *fo,
    void const *proto,
    unsigned const brka
)
{
    cprElfHdr3 *const h3 = (cprElfHdr3 *)(void *)&elfout;
    memcpy(h3, proto, sizeof(*h3));  // reads beyond, but OK
    h3->ehdr.e_ident[Elf32_Ehdr::EI_OSABI] = ei_osabi;
    assert(2==get_te16(&h3->ehdr.e_phnum));
    set_te16(&h3->ehdr.e_phnum, 3);

    assert(get_te32(&h3->ehdr.e_phoff)     == sizeof(Elf32_Ehdr));
                         h3->ehdr.e_shoff = 0;
    assert(get_te16(&h3->ehdr.e_ehsize)    == sizeof(Elf32_Ehdr));
    assert(get_te16(&h3->ehdr.e_phentsize) == sizeof(Elf32_Phdr));
           set_te16(&h3->ehdr.e_shentsize, sizeof(Elf32_Shdr));
                         h3->ehdr.e_shnum = 0;
                         h3->ehdr.e_shstrndx = 0;

    struct {
        Elf32_Nhdr nhdr;
        char name[8];
        unsigned body;
    } elfnote;

    unsigned const note_offset = sizeof(*h3) - sizeof(linfo);
    sz_elf_hdrs = sizeof(elfnote) + note_offset;

    set_te32(&h3->phdr[2].p_type, PT_NOTE32);
    set_te32(&h3->phdr[2].p_offset, note_offset);
    set_te32(&h3->phdr[2].p_vaddr, note_offset);
    set_te32(&h3->phdr[2].p_paddr, note_offset);
    set_te32(&h3->phdr[2].p_filesz, sizeof(elfnote));
    set_te32(&h3->phdr[2].p_memsz,  sizeof(elfnote));
    set_te32(&h3->phdr[2].p_flags, Elf32_Phdr::PF_R);
    set_te32(&h3->phdr[2].p_align, 4);

    // Q: Same as this->note_body[0 .. this->note_size-1] ?
    set_te32(&elfnote.nhdr.namesz, 8);
    set_te32(&elfnote.nhdr.descsz, OPENBSD_DESCSZ);
    set_te32(&elfnote.nhdr.type,   NHDR_OPENBSD_TAG);
    memcpy(elfnote.name, "OpenBSD", sizeof(elfnote.name));
    elfnote.body = 0;

    set_te32(&h3->phdr[0].p_filesz, sz_elf_hdrs);
              h3->phdr[0].p_memsz = h3->phdr[0].p_filesz;

    unsigned const brkb = brka | ((0==(~page_mask & brka)) ? 0x20 : 0);
    set_te32(&h3->phdr[1].p_type, PT_LOAD32);  // be sure
    set_te32(&h3->phdr[1].p_offset, ~page_mask & brkb);
    set_te32(&h3->phdr[1].p_vaddr, brkb);
    set_te32(&h3->phdr[1].p_paddr, brkb);
    h3->phdr[1].p_filesz = 0;
    // Too many kernels have bugs when 0==.p_memsz
    set_te32(&h3->phdr[1].p_memsz, 1);
    set_te32(&h3->phdr[1].p_flags, Elf32_Phdr::PF_R | Elf32_Phdr::PF_W);

    if (ph.format==getFormat()) {
        memset(&h3->linfo, 0, sizeof(h3->linfo));
        fo->write(h3, sizeof(*h3) - sizeof(h3->linfo));
        fo->write(&elfnote, sizeof(elfnote));
        fo->write(&h3->linfo, sizeof(h3->linfo));
    }
    else {
        assert(false);  // unknown ph.format, PackLinuxElf32
    }
}