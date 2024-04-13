mono_image_create_pefile (MonoReflectionModuleBuilder *mb, HANDLE file)
{
	MonoMSDOSHeader *msdos;
	MonoDotNetHeader *header;
	MonoSectionTable *section;
	MonoCLIHeader *cli_header;
	guint32 size, image_size, virtual_base, text_offset;
	guint32 header_start, section_start, file_offset, virtual_offset;
	MonoDynamicImage *assembly;
	MonoReflectionAssemblyBuilder *assemblyb;
	MonoDynamicStream pefile_stream = {0};
	MonoDynamicStream *pefile = &pefile_stream;
	int i, nsections;
	guint32 *rva, value;
	guchar *p;
	static const unsigned char msheader[] = {
		0x4d, 0x5a, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00,  0x04, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
		0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
		0x0e, 0x1f, 0xba, 0x0e, 0x00, 0xb4, 0x09, 0xcd,  0x21, 0xb8, 0x01, 0x4c, 0xcd, 0x21, 0x54, 0x68,
		0x69, 0x73, 0x20, 0x70, 0x72, 0x6f, 0x67, 0x72,  0x61, 0x6d, 0x20, 0x63, 0x61, 0x6e, 0x6e, 0x6f,
		0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6e,  0x20, 0x69, 0x6e, 0x20, 0x44, 0x4f, 0x53, 0x20,
		0x6d, 0x6f, 0x64, 0x65, 0x2e, 0x0d, 0x0d, 0x0a,  0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	assemblyb = mb->assemblyb;

	mono_image_basic_init (assemblyb);
	assembly = mb->dynamic_image;

	assembly->pe_kind = assemblyb->pe_kind;
	assembly->machine = assemblyb->machine;
	((MonoDynamicImage*)assemblyb->dynamic_assembly->assembly.image)->pe_kind = assemblyb->pe_kind;
	((MonoDynamicImage*)assemblyb->dynamic_assembly->assembly.image)->machine = assemblyb->machine;
	
	mono_image_build_metadata (mb);

	if (mb->is_main && assemblyb->resources) {
		int len = mono_array_length (assemblyb->resources);
		for (i = 0; i < len; ++i)
			assembly_add_resource (mb, assembly, (MonoReflectionResource*)mono_array_addr (assemblyb->resources, MonoReflectionResource, i));
	}

	if (mb->resources) {
		int len = mono_array_length (mb->resources);
		for (i = 0; i < len; ++i)
			assembly_add_resource (mb, assembly, (MonoReflectionResource*)mono_array_addr (mb->resources, MonoReflectionResource, i));
	}

	build_compressed_metadata (assembly);

	if (mb->is_main)
		assembly_add_win32_resources (assembly, assemblyb);

	nsections = calc_section_size (assembly);
	
	/* The DOS header and stub */
	g_assert (sizeof (MonoMSDOSHeader) == sizeof (msheader));
	mono_image_add_stream_data (pefile, (char*)msheader, sizeof (msheader));

	/* the dotnet header */
	header_start = mono_image_add_stream_zero (pefile, sizeof (MonoDotNetHeader));

	/* the section tables */
	section_start = mono_image_add_stream_zero (pefile, sizeof (MonoSectionTable) * nsections);

	file_offset = section_start + sizeof (MonoSectionTable) * nsections;
	virtual_offset = VIRT_ALIGN;
	image_size = 0;

	for (i = 0; i < MONO_SECTION_MAX; ++i) {
		if (!assembly->sections [i].size)
			continue;
		/* align offsets */
		file_offset += FILE_ALIGN - 1;
		file_offset &= ~(FILE_ALIGN - 1);
		virtual_offset += VIRT_ALIGN - 1;
		virtual_offset &= ~(VIRT_ALIGN - 1);

		assembly->sections [i].offset = file_offset;
		assembly->sections [i].rva = virtual_offset;

		file_offset += assembly->sections [i].size;
		virtual_offset += assembly->sections [i].size;
		image_size += (assembly->sections [i].size + VIRT_ALIGN - 1) & ~(VIRT_ALIGN - 1);
	}

	file_offset += FILE_ALIGN - 1;
	file_offset &= ~(FILE_ALIGN - 1);

	image_size += section_start + sizeof (MonoSectionTable) * nsections;

	/* back-patch info */
	msdos = (MonoMSDOSHeader*)pefile->data;
	msdos->pe_offset = GUINT32_FROM_LE (sizeof (MonoMSDOSHeader));

	header = (MonoDotNetHeader*)(pefile->data + header_start);
	header->pesig [0] = 'P';
	header->pesig [1] = 'E';
	
	header->coff.coff_machine = GUINT16_FROM_LE (assemblyb->machine);
	header->coff.coff_sections = GUINT16_FROM_LE (nsections);
	header->coff.coff_time = GUINT32_FROM_LE (time (NULL));
	header->coff.coff_opt_header_size = GUINT16_FROM_LE (sizeof (MonoDotNetHeader) - sizeof (MonoCOFFHeader) - 4);
	if (assemblyb->pekind == 1) {
		/* it's a dll */
		header->coff.coff_attributes = GUINT16_FROM_LE (0x210e);
	} else {
		/* it's an exe */
		header->coff.coff_attributes = GUINT16_FROM_LE (0x010e);
	}

	virtual_base = 0x400000; /* FIXME: 0x10000000 if a DLL */

	header->pe.pe_magic = GUINT16_FROM_LE (0x10B);
	header->pe.pe_major = 6;
	header->pe.pe_minor = 0;
	size = assembly->sections [MONO_SECTION_TEXT].size;
	size += FILE_ALIGN - 1;
	size &= ~(FILE_ALIGN - 1);
	header->pe.pe_code_size = GUINT32_FROM_LE(size);
	size = assembly->sections [MONO_SECTION_RSRC].size;
	size += FILE_ALIGN - 1;
	size &= ~(FILE_ALIGN - 1);
	header->pe.pe_data_size = GUINT32_FROM_LE(size);
	g_assert (START_TEXT_RVA == assembly->sections [MONO_SECTION_TEXT].rva);
	header->pe.pe_rva_code_base = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_TEXT].rva);
	header->pe.pe_rva_data_base = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_RSRC].rva);
	/* pe_rva_entry_point always at the beginning of the text section */
	header->pe.pe_rva_entry_point = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_TEXT].rva);

	header->nt.pe_image_base = GUINT32_FROM_LE (virtual_base);
	header->nt.pe_section_align = GUINT32_FROM_LE (VIRT_ALIGN);
	header->nt.pe_file_alignment = GUINT32_FROM_LE (FILE_ALIGN);
	header->nt.pe_os_major = GUINT16_FROM_LE (4);
	header->nt.pe_os_minor = GUINT16_FROM_LE (0);
	header->nt.pe_subsys_major = GUINT16_FROM_LE (4);
	size = section_start;
	size += FILE_ALIGN - 1;
	size &= ~(FILE_ALIGN - 1);
	header->nt.pe_header_size = GUINT32_FROM_LE (size);
	size = image_size;
	size += VIRT_ALIGN - 1;
	size &= ~(VIRT_ALIGN - 1);
	header->nt.pe_image_size = GUINT32_FROM_LE (size);

	/*
	// Translate the PEFileKind value to the value expected by the Windows loader
	*/
	{
		short kind;

		/*
		// PEFileKinds.Dll == 1
		// PEFileKinds.ConsoleApplication == 2
		// PEFileKinds.WindowApplication == 3
		//
		// need to get:
		//     IMAGE_SUBSYSTEM_WINDOWS_GUI 2 // Image runs in the Windows GUI subsystem.
                //     IMAGE_SUBSYSTEM_WINDOWS_CUI 3 // Image runs in the Windows character subsystem.
		*/
		if (assemblyb->pekind == 3)
			kind = 2;
		else
			kind = 3;
		
		header->nt.pe_subsys_required = GUINT16_FROM_LE (kind);
	}    
	header->nt.pe_stack_reserve = GUINT32_FROM_LE (0x00100000);
	header->nt.pe_stack_commit = GUINT32_FROM_LE (0x00001000);
	header->nt.pe_heap_reserve = GUINT32_FROM_LE (0x00100000);
	header->nt.pe_heap_commit = GUINT32_FROM_LE (0x00001000);
	header->nt.pe_loader_flags = GUINT32_FROM_LE (0);
	header->nt.pe_data_dir_count = GUINT32_FROM_LE (16);

	/* fill data directory entries */

	header->datadir.pe_resource_table.size = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_RSRC].size);
	header->datadir.pe_resource_table.rva = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_RSRC].rva);

	header->datadir.pe_reloc_table.size = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_RELOC].size);
	header->datadir.pe_reloc_table.rva = GUINT32_FROM_LE (assembly->sections [MONO_SECTION_RELOC].rva);

	header->datadir.pe_cli_header.size = GUINT32_FROM_LE (72);
	header->datadir.pe_cli_header.rva = GUINT32_FROM_LE (assembly->text_rva + assembly->cli_header_offset);
	header->datadir.pe_iat.size = GUINT32_FROM_LE (8);
	header->datadir.pe_iat.rva = GUINT32_FROM_LE (assembly->text_rva + assembly->iat_offset);
	/* patch entrypoint name */
	if (assemblyb->pekind == 1)
		memcpy (assembly->code.data + assembly->imp_names_offset + 2, "_CorDllMain", 12);
	else
		memcpy (assembly->code.data + assembly->imp_names_offset + 2, "_CorExeMain", 12);
	/* patch imported function RVA name */
	rva = (guint32*)(assembly->code.data + assembly->iat_offset);
	*rva = GUINT32_FROM_LE (assembly->text_rva + assembly->imp_names_offset);

	/* the import table */
	header->datadir.pe_import_table.size = GUINT32_FROM_LE (79); /* FIXME: magic number? */
	header->datadir.pe_import_table.rva = GUINT32_FROM_LE (assembly->text_rva + assembly->idt_offset);
	/* patch imported dll RVA name and other entries in the dir */
	rva = (guint32*)(assembly->code.data + assembly->idt_offset + G_STRUCT_OFFSET (MonoIDT, name_rva));
	*rva = GUINT32_FROM_LE (assembly->text_rva + assembly->imp_names_offset + 14); /* 14 is hint+strlen+1 of func name */
	rva = (guint32*)(assembly->code.data + assembly->idt_offset + G_STRUCT_OFFSET (MonoIDT, import_address_table_rva));
	*rva = GUINT32_FROM_LE (assembly->text_rva + assembly->iat_offset);
	rva = (guint32*)(assembly->code.data + assembly->idt_offset + G_STRUCT_OFFSET (MonoIDT, import_lookup_table));
	*rva = GUINT32_FROM_LE (assembly->text_rva + assembly->ilt_offset);

	p = (guchar*)(assembly->code.data + assembly->ilt_offset);
	value = (assembly->text_rva + assembly->imp_names_offset);
	*p++ = (value) & 0xff;
	*p++ = (value >> 8) & (0xff);
	*p++ = (value >> 16) & (0xff);
	*p++ = (value >> 24) & (0xff);

	/* the CLI header info */
	cli_header = (MonoCLIHeader*)(assembly->code.data + assembly->cli_header_offset);
	cli_header->ch_size = GUINT32_FROM_LE (72);
	cli_header->ch_runtime_major = GUINT16_FROM_LE (2);
	cli_header->ch_runtime_minor = GUINT16_FROM_LE (5);
	cli_header->ch_flags = GUINT32_FROM_LE (assemblyb->pe_kind);
	if (assemblyb->entry_point) {
		guint32 table_idx = 0;
		if (!strcmp (assemblyb->entry_point->object.vtable->klass->name, "MethodBuilder")) {
			MonoReflectionMethodBuilder *methodb = (MonoReflectionMethodBuilder*)assemblyb->entry_point;
			table_idx = methodb->table_idx;
		} else {
			table_idx = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->method_to_table_idx, assemblyb->entry_point->method));
		}
		cli_header->ch_entry_point = GUINT32_FROM_LE (table_idx | MONO_TOKEN_METHOD_DEF);
	} else {
		cli_header->ch_entry_point = GUINT32_FROM_LE (0);
	}
	/* The embedded managed resources */
	text_offset = assembly->text_rva + assembly->code.index;
	cli_header->ch_resources.rva = GUINT32_FROM_LE (text_offset);
	cli_header->ch_resources.size = GUINT32_FROM_LE (assembly->resources.index);
	text_offset += assembly->resources.index;
	cli_header->ch_metadata.rva = GUINT32_FROM_LE (text_offset);
	cli_header->ch_metadata.size = GUINT32_FROM_LE (assembly->meta_size);
	text_offset += assembly->meta_size;
	if (assembly->strong_name_size) {
		cli_header->ch_strong_name.rva = GUINT32_FROM_LE (text_offset);
		cli_header->ch_strong_name.size = GUINT32_FROM_LE (assembly->strong_name_size);
		text_offset += assembly->strong_name_size;
	}

	/* write the section tables and section content */
	section = (MonoSectionTable*)(pefile->data + section_start);
	for (i = 0; i < MONO_SECTION_MAX; ++i) {
		static const char section_names [][7] = {
			".text", ".rsrc", ".reloc"
		};
		if (!assembly->sections [i].size)
			continue;
		strcpy (section->st_name, section_names [i]);
		/*g_print ("output section %s (%d), size: %d\n", section->st_name, i, assembly->sections [i].size);*/
		section->st_virtual_address = GUINT32_FROM_LE (assembly->sections [i].rva);
		section->st_virtual_size = GUINT32_FROM_LE (assembly->sections [i].size);
		section->st_raw_data_size = GUINT32_FROM_LE (GUINT32_TO_LE (section->st_virtual_size) + (FILE_ALIGN - 1));
		section->st_raw_data_size &= GUINT32_FROM_LE (~(FILE_ALIGN - 1));
		section->st_raw_data_ptr = GUINT32_FROM_LE (assembly->sections [i].offset);
		section->st_flags = GUINT32_FROM_LE (assembly->sections [i].attrs);
		section ++;
	}
	
	checked_write_file (file, pefile->data, pefile->index);
	
	mono_dynamic_stream_reset (pefile);
	
	for (i = 0; i < MONO_SECTION_MAX; ++i) {
		if (!assembly->sections [i].size)
			continue;
		
		if (SetFilePointer (file, assembly->sections [i].offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			g_error ("SetFilePointer returned %d\n", GetLastError ());
		
		switch (i) {
		case MONO_SECTION_TEXT:
			/* patch entry point */
			p = (guchar*)(assembly->code.data + 2);
			value = (virtual_base + assembly->text_rva + assembly->iat_offset);
			*p++ = (value) & 0xff;
			*p++ = (value >> 8) & 0xff;
			*p++ = (value >> 16) & 0xff;
			*p++ = (value >> 24) & 0xff;
		
			checked_write_file (file, assembly->code.data, assembly->code.index);
			checked_write_file (file, assembly->resources.data, assembly->resources.index);
			checked_write_file (file, assembly->image.raw_metadata, assembly->meta_size);
			checked_write_file (file, assembly->strong_name, assembly->strong_name_size);
				

			g_free (assembly->image.raw_metadata);
			break;
		case MONO_SECTION_RELOC: {
			struct {
				guint32 page_rva;
				guint32 block_size;
				guint16 type_and_offset;
				guint16 term;
			} reloc;
			
			g_assert (sizeof (reloc) == 12);
			
			reloc.page_rva = GUINT32_FROM_LE (assembly->text_rva);
			reloc.block_size = GUINT32_FROM_LE (12);
			
			/* 
			 * the entrypoint is always at the start of the text section 
			 * 3 is IMAGE_REL_BASED_HIGHLOW
			 * 2 is patch_size_rva - text_rva
			 */
			reloc.type_and_offset = GUINT16_FROM_LE ((3 << 12) + (2));
			reloc.term = 0;
			
			checked_write_file (file, &reloc, sizeof (reloc));
			
			break;
		}
		case MONO_SECTION_RSRC:
			if (assembly->win32_res) {

				/* Fixup the offsets in the IMAGE_RESOURCE_DATA_ENTRY structures */
				fixup_resource_directory (assembly->win32_res, assembly->win32_res, assembly->sections [i].rva);
				checked_write_file (file, assembly->win32_res, assembly->win32_res_size);
			}
			break;
		default:
			g_assert_not_reached ();
		}
	}
	
	/* check that the file is properly padded */
	if (SetFilePointer (file, file_offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		g_error ("SetFilePointer returned %d\n", GetLastError ());
	if (! SetEndOfFile (file))
		g_error ("SetEndOfFile returned %d\n", GetLastError ());
	
	mono_dynamic_stream_reset (&assembly->code);
	mono_dynamic_stream_reset (&assembly->us);
	mono_dynamic_stream_reset (&assembly->blob);
	mono_dynamic_stream_reset (&assembly->guid);
	mono_dynamic_stream_reset (&assembly->sheap);

	g_hash_table_foreach (assembly->blob_cache, (GHFunc)g_free, NULL);
	g_hash_table_destroy (assembly->blob_cache);
	assembly->blob_cache = NULL;
}