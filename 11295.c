pcx24b_print_page(gx_device_printer * pdev, gp_file * file)
{
    pcx_header header;

    header = pcx_header_prototype;
    header.version = version_3_0;
    header.bpp = 8;
    header.nplanes = 3;
    assign_ushort(header.palinfo, palinfo_color);
    return pcx_write_page(pdev, file, &header, true);
}