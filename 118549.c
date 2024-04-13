void PackLinuxElf32::pack4(OutputFile *fo, Filter &ft)
{
    overlay_offset = sz_elf_hdrs + sizeof(linfo);

    if (opt->o_unix.preserve_build_id) {
        // calc e_shoff here and write shdrout, then o_shstrtab
        //NOTE: these are pushed last to ensure nothing is stepped on
        //for the UPX structure.
        unsigned const len = fpad4(fo);
        set_te32(&elfout.ehdr.e_shoff,len);

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
    set_te32(&elfout.phdr[0].p_filesz, sz_pack2 + lsize);
              elfout.phdr[0].p_memsz = elfout.phdr[0].p_filesz;
    super::pack4(fo, ft);  // write PackHeader and overlay_offset

    fo->seek(0, SEEK_SET);
    if (0!=xct_off) {  // shared library
        fo->rewrite(&lowmem[0], sizeof(ehdri) + e_phnum * sizeof(*phdri));
        fo->seek(sz_elf_hdrs, SEEK_SET);
        fo->rewrite(&linfo, sizeof(linfo));

        if (jni_onload_va) {
            unsigned tmp = sz_pack2 + get_te32(&elfout.phdr[0].p_vaddr);
            tmp |= (Elf32_Ehdr::EM_ARM==e_machine);  // THUMB mode
            set_te32(&tmp, tmp);
            fo->seek(ptr_udiff(&jni_onload_sym->st_value, file_image), SEEK_SET);
            fo->rewrite(&tmp, sizeof(tmp));
        }
    }
    else {
        unsigned const reloc = get_te32(&elfout.phdr[0].p_vaddr);
        Elf32_Phdr *phdr = &elfout.phdr[2];
        unsigned const o_phnum = get_te16(&elfout.ehdr.e_phnum);
        for (unsigned j = 2; j < o_phnum; ++j, ++phdr) {
            if (PT_NOTE32 == get_te32(&phdr->p_type)) {
                set_te32(            &phdr->p_vaddr,
                    reloc + get_te32(&phdr->p_vaddr));
                set_te32(            &phdr->p_paddr,
                    reloc + get_te32(&phdr->p_paddr));
            }
        }
        fo->rewrite(&elfout, sizeof(Elf32_Phdr) * o_phnum + sizeof(Elf32_Ehdr));
        fo->seek(sz_elf_hdrs, SEEK_SET);  // skip over PT_NOTE bodies, if any
        fo->rewrite(&linfo, sizeof(linfo));
    }
}