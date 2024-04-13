pcx16_print_page(gx_device_printer * pdev, gp_file * file)
{
    pcx_header header;

    header = pcx_header_prototype;
    header.version = version_2_8_with_palette;
    header.bpp = 1;
    header.nplanes = 4;
    /* Fill the EGA palette appropriately. */
    memcpy((byte *) header.palette, pcx_ega_palette,
           sizeof(pcx_ega_palette));
    return pcx_write_page(pdev, file, &header, true);
}