static int write_simple_text(unsigned short *text, int count, gx_device_txtwrite_t *tdev)
{
    switch(tdev->TextFormat) {
        case 2:
            gp_fwrite(text, sizeof (unsigned short), count, tdev->file);
            break;
        case 3:
            {
                int i;
                unsigned short *UTF16 = (unsigned short *)text;
                unsigned char UTF8[3];

                for (i=0;i<count;i++) {
                    if (*UTF16 < 0x80) {
                        UTF8[0] = *UTF16 & 0xff;
                        gp_fwrite (UTF8, sizeof(unsigned char), 1, tdev->file);
                    } else {
                        if (*UTF16 < 0x800) {
                            UTF8[0] = (*UTF16 >> 6) + 0xC0;
                            UTF8[1] = (*UTF16 & 0x3F) + 0x80;
                            gp_fwrite (UTF8, sizeof(unsigned char), 2, tdev->file);
                        } else {
                            UTF8[0] = (*UTF16 >> 12) + 0xE0;
                            UTF8[1] = ((*UTF16 >> 6) & 0x3F) + 0x80;
                            UTF8[2] = (*UTF16 & 0x3F) + 0x80;
                            gp_fwrite (UTF8, sizeof(unsigned char), 3, tdev->file);
                        }
                    }
                    UTF16++;
                }
            }
            break;
        default:
            return gs_note_error(gs_error_rangecheck);
            break;
    }
    return 0;
}