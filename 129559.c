int extract_one(struct archive* a, struct archive_entry* ae, uint32_t crc)
{
    la_ssize_t fsize, bytes_read;
    uint8_t* buf;
    int ret = 1;
    uint32_t computed_crc;

    fsize = archive_entry_size(ae);
    buf = malloc(fsize);
    if(buf == NULL)
        return 1;

    bytes_read = archive_read_data(a, buf, fsize);
    if(bytes_read != fsize) {
        assertEqualInt(bytes_read, fsize);
        goto fn_exit;
    }

    computed_crc = crc32(0, buf, fsize);
    assertEqualInt(computed_crc, crc);
    ret = 0;

fn_exit:
    free(buf);
    return ret;
}