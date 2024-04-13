static bool ok_jpg_read_sos(ok_jpg_decoder *decoder) {
    // JPEG spec: Table B.3
    ok_jpg *jpg = decoder->jpg;
    uint8_t buffer[16];
    const size_t header_size = 3;
    if (!ok_read(decoder, buffer, header_size)) {
        return false;
    }
    uint16_t length = readBE16(buffer);
    decoder->num_scan_components = buffer[2];
    if (decoder->num_scan_components > decoder->num_components) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (Ns)");
        return false;
    }
    const size_t expected_size = 3 + (size_t)decoder->num_scan_components * 2;
    if (length != expected_size + header_size) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (L)");
        return false;
    }
    if (sizeof(buffer) < expected_size) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Too many components for buffer");
        return false;
    }
    if (!ok_read(decoder, buffer, expected_size)) {
        return false;
    }

    uint8_t *src = buffer;
    for (int i = 0; i < decoder->num_scan_components; i++, src += 2) {
        int C = src[0];
        bool component_found = false;
        for (int j = 0; j < decoder->num_components; j++) {
            ok_jpg_component *c = decoder->components + j;
            if (c->id == C) {
                decoder->scan_components[i] = j;
                component_found = true;
            }
        }
        if (!component_found) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (C)");
            return false;
        }
        
        // Make sure component is unique
        for (int j = 0; j < i; j++) {
            if (decoder->scan_components[i] == decoder->scan_components[j]) {
                ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (component ids)");
                return false;
            }
        }

        ok_jpg_component *comp = decoder->components + decoder->scan_components[i];
        comp->Td = src[1] >> 4;
        comp->Ta = src[1] & 0x0f;
        if (comp->Td > 3 || comp->Ta > 3) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (Td/Ta)");
            return false;
        }
    }

    decoder->scan_start = src[0];
    decoder->scan_end = src[1];
    decoder->scan_prev_scale = src[2] >> 4;
    decoder->scan_scale = src[2] & 0x0f;

    if (decoder->progressive) {
        if (decoder->scan_start < 0 || decoder->scan_start > 63 ||
            decoder->scan_end < decoder->scan_start || decoder->scan_end > 63 ||
            decoder->scan_prev_scale < 0 || decoder->scan_prev_scale > 13 ||
            decoder->scan_scale < 0 || decoder->scan_scale > 13) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid progressive SOS segment (Ss/Se/Ah/Al)");
            return false;
        }
    } else {
        // Sequential
        if (decoder->scan_start != 0 || decoder->scan_end != 63 ||
            decoder->scan_prev_scale != 0 || decoder->scan_scale != 0) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid SOS segment (Ss/Se/Ah/Al)");
            return false;
        }
    }

#if 0
    printf("SOS: Scan components: (");
    for (int i = 0; i < decoder->num_scan_components; i++) {
        ok_jpg_component *comp = decoder->components + decoder->scan_components[i];
        if (i != 0) {
            printf(",");
        }
        printf("%i", comp->id);

    }
    printf(") ");
    for (int i = decoder->num_scan_components; i < decoder->num_components; i++) {
        printf("  ");
    }
    printf("\trange: %i...%i \tah: %i al: %i\n",
           decoder->scan_start, decoder->scan_end,
           decoder->scan_prev_scale, decoder->scan_scale);
#endif

    return ok_jpg_decode_scan(decoder);
}