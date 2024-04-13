pcx256_print_page(gx_device_printer * pdev, gp_file * file)
{
    pcx_header header;
    int code;

    header = pcx_header_prototype;
    header.version = version_3_0;
    header.bpp = 8;
    header.nplanes = 1;
    assign_ushort(header.palinfo,
                  (pdev->color_info.num_components > 1 ?
                   palinfo_color : palinfo_gray));
    code = pcx_write_page(pdev, file, &header, false);
    if (code >= 0) {		/* Write out the palette. */
        gp_fputc(0x0c, file);
        code = pc_write_palette((gx_device *) pdev, 256, file);
    }
    return code;
}