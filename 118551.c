PackLinuxElf32::PackLinuxElf32help1(InputFile *f)
{
    e_type  = get_te16(&ehdri.e_type);
    e_phnum = get_te16(&ehdri.e_phnum);
    e_shnum = get_te16(&ehdri.e_shnum);
    unsigned const e_phentsize = get_te16(&ehdri.e_phentsize);
    if (ehdri.e_ident[Elf32_Ehdr::EI_CLASS]!=Elf32_Ehdr::ELFCLASS32
    || sizeof(Elf32_Phdr) != e_phentsize
    || (Elf32_Ehdr::ELFDATA2MSB == ehdri.e_ident[Elf32_Ehdr::EI_DATA]
            && &N_BELE_RTP::be_policy != bele)
    || (Elf32_Ehdr::ELFDATA2LSB == ehdri.e_ident[Elf32_Ehdr::EI_DATA]
            && &N_BELE_RTP::le_policy != bele)) {
        e_phoff = 0;
        e_shoff = 0;
        sz_phdrs = 0;
        return;
    }
    if (0==e_phnum) throwCantUnpack("0==e_phnum");
    e_phoff = get_te32(&ehdri.e_phoff);
    unsigned const last_Phdr = e_phoff + e_phnum * sizeof(Elf32_Phdr);
    if (last_Phdr < e_phoff || (unsigned long)file_size < last_Phdr) {
        throwCantUnpack("bad e_phoff");
    }
    e_shoff = get_te32(&ehdri.e_shoff);
    unsigned const last_Shdr = e_shoff + e_shnum * sizeof(Elf32_Shdr);
    if (last_Shdr < e_shoff || (unsigned long)file_size < last_Shdr) {
        if (opt->cmd == CMD_COMPRESS) {
            throwCantUnpack("bad e_shoff");
        }
    }
    sz_phdrs = e_phnum * e_phentsize;

    if (f && Elf32_Ehdr::ET_DYN!=e_type) {
        unsigned const len = sz_phdrs + e_phoff;
        alloc_file_image(file_image, len);
        f->seek(0, SEEK_SET);
        f->readx(file_image, len);
        phdri= (Elf32_Phdr       *)(e_phoff + file_image);  // do not free() !!
    }
    if (f && Elf32_Ehdr::ET_DYN==e_type) {
        // The DT_SYMTAB has no designated length.  Read the whole file.
        alloc_file_image(file_image, file_size);
        f->seek(0, SEEK_SET);
        f->readx(file_image, file_size);
        phdri= (Elf32_Phdr *)(e_phoff + file_image);  // do not free() !!
        shdri= (Elf32_Shdr *)(e_shoff + file_image);  // do not free() !!
        if (opt->cmd != CMD_COMPRESS) {
            shdri = NULL;
        }
        sec_dynsym = elf_find_section_type(Elf32_Shdr::SHT_DYNSYM);
        if (sec_dynsym) {
            unsigned t = get_te32(&sec_dynsym->sh_link);
            if (e_shnum <= t)
                throwCantPack("bad dynsym->sh_link");
            sec_dynstr = &shdri[t];
        }

        Elf32_Phdr const *phdr= phdri;
        for (int j = e_phnum; --j>=0; ++phdr)
        if (Elf32_Phdr::PT_DYNAMIC==get_te32(&phdr->p_type)) {
            dynseg= (Elf32_Dyn const *)(check_pt_dynamic(phdr) + file_image);
            invert_pt_dynamic(dynseg);
        }
        else if (PT_LOAD32==get_te32(&phdr->p_type)) {
            check_pt_load(phdr);
        }
        // elf_find_dynamic() returns 0 if 0==dynseg.
        dynstr =      (char const *)elf_find_dynamic(Elf32_Dyn::DT_STRTAB);
        dynsym = (Elf32_Sym const *)elf_find_dynamic(Elf32_Dyn::DT_SYMTAB);
        gashtab = (unsigned const *)elf_find_dynamic(Elf32_Dyn::DT_GNU_HASH);
        hashtab = (unsigned const *)elf_find_dynamic(Elf32_Dyn::DT_HASH);
        jni_onload_sym = elf_lookup("JNI_OnLoad");
        if (jni_onload_sym) {
            jni_onload_va = get_te32(&jni_onload_sym->st_value);
            jni_onload_va = 0;  // FIXME not understood; need example
        }
    }
}