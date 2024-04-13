void PackLinuxElf64::pack4(OutputFile *fo, Filter &ft)
{
    overlay_offset = sz_elf_hdrs + sizeof(linfo);

    if (opt->o_unix.preserve_build_id) {
        // calc e_shoff here and write shdrout, then o_shstrtab
        //NOTE: these are pushed last to ensure nothing is stepped on
        //for the UPX structure.
        unsigned const len = fpad4(fo);
        set_te64(&elfout.ehdr.e_shoff,len);

        int const ssize = sizeof(shdrout);

        shdrout.shdr[2].sh_offset = len+ssize;
        shdrout.shdr[1].sh_offset = shdrout.shdr[2].sh_offset+shdrout.shdr[2].sh_size;

        fo->write(&shdrout, ssize);

        fo->write(o_shstrtab,shdrout.shdr[2].sh_size);
        fo->write(buildid_data,shdrout.shdr[1].sh_size);
    }

    // Cannot pre-round .p_memsz.  If .p_filesz < .p_memsz, then kernel
    // tries to make .bss, which requires PF_W.
    // But strict SELinux (or PaX, grSecurity) disallows PF_W with PF_X.
    set_te64(&elfout.phdr[0].p_filesz, sz_pack2 + lsize);
              elfout.phdr[0].p_memsz = elfout.phdr[0].p_filesz;
    super::pack4(fo, ft);  // write PackHeader and overlay_offset

    fo->seek(0, SEEK_SET);
    if (0!=xct_off) {  // shared library
        fo->rewrite(&lowmem[0], sizeof(ehdri) + e_phnum * sizeof(Elf64_Phdr));
        fo->seek(sz_elf_hdrs, SEEK_SET);
        fo->rewrite(&linfo, sizeof(linfo));
    }
    else {
        if (PT_NOTE64 == get_te64(&elfout.phdr[2].p_type)) {
            upx_uint64_t const reloc = get_te64(&elfout.phdr[0].p_vaddr);
            set_te64(            &elfout.phdr[2].p_vaddr,
                reloc + get_te64(&elfout.phdr[2].p_vaddr));
            set_te64(            &elfout.phdr[2].p_paddr,
                reloc + get_te64(&elfout.phdr[2].p_paddr));
            fo->rewrite(&elfout, sz_elf_hdrs);
            // FIXME   fo->rewrite(&elfnote, sizeof(elfnote));
        }
        else {
            fo->rewrite(&elfout, sz_elf_hdrs);
        }
        fo->rewrite(&linfo, sizeof(linfo));
    }
}