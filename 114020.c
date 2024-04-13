static bool ok_jpg_read_exif(ok_jpg_decoder *decoder) {
    static const char exif_magic[] = {'E', 'x', 'i', 'f', 0, 0};
    static const char tiff_magic_little_endian[] = {0x49, 0x49, 0x2a, 0x00};
    static const char tiff_magic_big_endian[] = {0x4d, 0x4d, 0x00, 0x2a};

    uint8_t buffer[2];
    if (!ok_read(decoder, buffer, sizeof(buffer))) {
        return false;
    }
    int length = readBE16(buffer) - 2;

    // Check for Exif header
    uint8_t exif_header[6];
    const int exif_header_length = sizeof(exif_header);
    if (length < exif_header_length) {
        return ok_seek(decoder, length);
    }

    if (!ok_read(decoder, exif_header, exif_header_length)) {
        return false;
    }
    length -= exif_header_length;
    if (memcmp(exif_header, exif_magic, exif_header_length) != 0) {
        return ok_seek(decoder, length);
    }

    // Check for TIFF header
    bool little_endian;
    uint8_t tiff_header[4];
    const int tiff_header_length = sizeof(tiff_header);
    if (length < tiff_header_length) {
        return ok_seek(decoder, length);
    }
    if (!ok_read(decoder, tiff_header, tiff_header_length)) {
        return false;
    }
    length -= tiff_header_length;
    if (memcmp(tiff_header, tiff_magic_little_endian, tiff_header_length) == 0) {
        little_endian = true;
    } else if (memcmp(tiff_header, tiff_magic_big_endian, tiff_header_length) == 0) {
        little_endian = false;
    } else {
        return ok_seek(decoder, length);
    }

    // Get start offset
    if (length < 4) {
        return ok_seek(decoder, length);
    }
    int32_t offset;
    uint8_t offset_buffer[4];
    if (!ok_read(decoder, offset_buffer, sizeof(offset_buffer))) {
        return false;
    }
    offset = (int32_t)(little_endian ? readLE32(offset_buffer) : readBE32(offset_buffer));
    length -= 4;
    offset -= 8; // Ignore tiff header, offset
    if (offset < 0 || offset > length) {
        return ok_seek(decoder, length);
    }
    if (!ok_seek(decoder, offset)) {
        return false;
    }
    length -= offset;

    // Get number of tags
    if (length < 2) {
        return ok_seek(decoder, length);
    }
    if (!ok_read(decoder, buffer, sizeof(buffer))) {
        return false;
    }
    length -= 2;
    int num_tags = little_endian ? readLE16(buffer) : readBE16(buffer);

    // Read tags, searching for orientation (0x112)
    uint8_t tag_buffer[12];
    const int tag_length = sizeof(tag_buffer);
    for (int i = 0; i < num_tags; i++) {
        if (length < tag_length) {
            return ok_seek(decoder, length);
        }
        if (!ok_read(decoder, tag_buffer, tag_length)) {
            return false;
        }
        length -= tag_length;

        int tag = little_endian ? readLE16(tag_buffer) : readBE16(tag_buffer);
        if (tag == 0x112) {
            int orientation = little_endian ? readLE16(tag_buffer + 8) : readBE16(tag_buffer + 8);
            switch (orientation) {
                case 1:
                default: // top-left
                    break;
                case 2: // top-right
                    decoder->flip_x = true;
                    break;
                case 3: // bottom-right
                    decoder->flip_x = true;
                    decoder->flip_y = !decoder->flip_y;
                    break;
                case 4: // bottom-left
                    decoder->flip_y = !decoder->flip_y;
                    break;
                case 5: // left-top
                    decoder->rotate = true;
                    break;
                case 6: // right-top
                    decoder->rotate = true;
                    decoder->flip_x = true;
                    break;
                case 7: // right-bottom
                    decoder->rotate = true;
                    decoder->flip_x = true;
                    decoder->flip_y = !decoder->flip_y;
                    break;
                case 8: // left-bottom
                    decoder->rotate = true;
                    decoder->flip_y = !decoder->flip_y;
                    break;
            }
            break;
        }
    }
    return ok_seek(decoder, length);
}