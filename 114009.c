static bool ok_jpg_read_sof(ok_jpg_decoder *decoder) {
    // JPEG spec: Table B.2
    ok_jpg *jpg = decoder->jpg;
    uint8_t buffer[3 * 3];
    if (!ok_read(decoder, buffer, 8)) {
        return false;
    }
    int length = readBE16(buffer) - 8;
    int P = buffer[2];
    if (P != 8) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid component size");
        return false;
    }
    decoder->in_height = readBE16(buffer + 3);
    decoder->in_width = readBE16(buffer + 5);
    if (decoder->in_width == 0 || decoder->in_height == 0) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid image dimensions");
        return false;
    }
    jpg->width = decoder->rotate ? decoder->in_height : decoder->in_width;
    jpg->height = decoder->rotate ? decoder->in_width : decoder->in_height;
    jpg->bpp = 4; // Always decoding to 32-bit color
    decoder->num_components = buffer[7];
    
    uint64_t stride = (uint64_t)jpg->width * jpg->bpp;
    if (stride > INT32_MAX) {
        // Stride must fit in a singed int, see ok_jpg_convert_data_unit
        ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Width too large");
        return false;
    }
    jpg->stride = (uint32_t)stride;
    
    if (decoder->num_components == 4) {
        ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Unsupported format (CMYK)");
        return false;
    }
    if (decoder->num_components != 1 && decoder->num_components != 3) {
        ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Invalid component count");
        return false;
    }

    if (length < 3 * decoder->num_components) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "SOF segment too short");
        return false;
    }
    if (!ok_read(decoder, buffer, 3 * (size_t)decoder->num_components)) {
        return false;
    }

    int maxH = 1;
    int maxV = 1;
    int minH = 4;
    int minV = 4;
    for (int i = 0; i < decoder->num_components; i++) {
        ok_jpg_component *c = decoder->components + i;
        c->id = buffer[i * 3 + 0];
        c->H = buffer[i * 3 + 1] >> 4;
        c->V = buffer[i * 3 + 1] & 0x0F;
        c->Tq = buffer[i * 3 + 2];

        if (c->H == 0 || c->V == 0 || c->H > 4 || c->V > 4 || c->Tq > 3) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Bad component");
            return false;
        }

        if (c->H > MAX_SAMPLING_FACTOR || c->V > MAX_SAMPLING_FACTOR) {
            ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Unsupported sampling factor");
            return false;
        }

        maxH = max(maxH, c->H);
        maxV = max(maxV, c->V);
        minH = min(minH, c->H);
        minV = min(minV, c->V);
        length -= 3;
    }
    if (minH > 1 || minV > 1) {
        maxH = 1;
        maxV = 1;
        for (int i = 0; i < decoder->num_components; i++) {
            ok_jpg_component *c = decoder->components + i;
            c->H /= minH;
            c->V /= minV;
            maxH = max(maxH, c->H);
            maxV = max(maxV, c->V);
        }
    }
    decoder->data_units_x = intDivCeil(decoder->in_width, maxH * 8);
    decoder->data_units_y = intDivCeil(decoder->in_height, maxV * 8);

    // Skip remaining length, if any
    if (length > 0) {
        if (!ok_seek(decoder, length)) {
            return false;
        }
    }

    // Setup idct
    for (int i = 0; i < decoder->num_components; i++) {
        ok_jpg_component *c = decoder->components + i;
        c->blocks_h = intDivCeil(decoder->in_width, (maxH / c->H) * 8);
        c->blocks_v = intDivCeil(decoder->in_height, (maxV / c->V) * 8);
        if (c->H == maxH && c->V == maxV) {
            c->idct = ok_jpg_idct_8x8;
        } else if (c->H * 2 == maxH && c->V * 2 == maxV) {
            c->idct = ok_jpg_idct_16x16;
        } else if (c->H == maxH && c->V * 2 == maxV) {
            c->idct = ok_jpg_idct_8x16;
        } else if (c->H * 2 == maxH && c->V == maxV) {
            c->idct = ok_jpg_idct_16x8;
        } else {
            ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Unsupported IDCT sampling factor");
            return false;
        }
    }

    // Allocate data
    if (!decoder->info_only) {
        if (decoder->sof_found) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid JPEG (Multiple SOF markers)");
            return false;
        }
        decoder->sof_found = true;

        if (decoder->progressive) {
            for (int i = 0; i < decoder->num_components; i++) {
                ok_jpg_component *c = decoder->components + i;
                size_t num_blocks = (size_t)(decoder->data_units_x * c->H *
                                             decoder->data_units_y * c->V);
                size_t size = (num_blocks * 64 + OK_JPG_BLOCK_EXTRA_SPACE) * sizeof(*c->blocks);
                c->blocks = decoder->allocator.alloc(decoder->allocator_user_data, size);
                if (!c->blocks) {
                    ok_jpg_error(jpg, OK_JPG_ERROR_ALLOCATION,
                                 "Couldn't allocate internal block memory for image");
                    return false;
                }
            }
        }

        if (!jpg->data) {
            if (decoder->allocator.image_alloc) {
                decoder->allocator.image_alloc(decoder->allocator_user_data,
                                               jpg->width, jpg->height, jpg->bpp,
                                               &jpg->data, &jpg->stride);
            } else {
                uint64_t size = (uint64_t)jpg->stride * jpg->height;
                size_t platform_size = (size_t)size;
                if (platform_size == size) {
                    jpg->data = decoder->allocator.alloc(decoder->allocator_user_data, platform_size);
                }
            }
            if (!jpg->data) {
                ok_jpg_error(jpg, OK_JPG_ERROR_ALLOCATION, "Couldn't allocate memory for image");
                return false;
            }
            if (jpg->stride < jpg->width * jpg->bpp) {
                ok_jpg_error(jpg, OK_JPG_ERROR_API, "Invalid stride");
                return false;
            }
        }
    }

#if 0
    printf("SOF: Components %i (", decoder->num_components);
    for (int i = 0; i < decoder->num_components; i++) {
        ok_jpg_component *c = decoder->components + i;
        if (i != 0) {
            printf(",");
        }
        printf("%ix%i", maxH/c->H, maxV/c->V);
    }
    printf(")\n");
#endif
    return true;
}