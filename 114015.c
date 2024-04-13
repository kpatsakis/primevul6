static void ok_jpg_decode_restart(ok_jpg_decoder *decoder) {
    decoder->restart_intervals_remaining = decoder->restart_intervals;
    for (int i = 0; i < decoder->num_scan_components; i++) {
        ok_jpg_component *c = decoder->components + decoder->scan_components[i];
        c->pred = 0;
        c->eob_run = 0;
    }
}