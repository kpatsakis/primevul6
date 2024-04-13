pcxcmyk_print_page(gx_device_printer * pdev, gp_file * file)
{
    pcx_header header;

    header = pcx_header_prototype;
    header.version = 2;
    header.bpp = 4;
    header.nplanes = 1;
    /* Fill the palette appropriately. */
    memcpy((byte *) header.palette, pcx_cmyk_palette,
           sizeof(pcx_cmyk_palette));
    return pcx_write_page(pdev, file, &header, false);
}