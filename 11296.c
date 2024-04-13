pcxmono_print_page(gx_device_printer * pdev, gp_file * file)
{
    pcx_header header;

    header = pcx_header_prototype;
    header.version = version_2_8_with_palette;
    header.bpp = 1;
    header.nplanes = 1;
    assign_ushort(header.palinfo, palinfo_gray);
    /* Set the first two entries of the short palette. */
    memcpy((byte *) header.palette, "\000\000\000\377\377\377", 6);
    return pcx_write_page(pdev, file, &header, false);
}