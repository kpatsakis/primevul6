static int cbs_av1_write_obu(CodedBitstreamContext *ctx,
                             CodedBitstreamUnit *unit,
                             PutBitContext *pbc)
{
    CodedBitstreamAV1Context *priv = ctx->priv_data;
    AV1RawOBU *obu = unit->content;
    PutBitContext pbc_tmp;
    AV1RawTileData *td;
    size_t header_size;
    int err, start_pos, end_pos, data_pos;

    // OBUs in the normal bitstream format must contain a size field
    // in every OBU (in annex B it is optional, but we don't support
    // writing that).
    obu->header.obu_has_size_field = 1;

    err = cbs_av1_write_obu_header(ctx, pbc, &obu->header);
    if (err < 0)
        return err;

    if (obu->header.obu_has_size_field) {
        pbc_tmp = *pbc;
        // Add space for the size field to fill later.
        put_bits32(pbc, 0);
        put_bits32(pbc, 0);
    }

    td = NULL;
    start_pos = put_bits_count(pbc);

    switch (obu->header.obu_type) {
    case AV1_OBU_SEQUENCE_HEADER:
        {
            err = cbs_av1_write_sequence_header_obu(ctx, pbc,
                                                    &obu->obu.sequence_header);
            if (err < 0)
                return err;

            av_buffer_unref(&priv->sequence_header_ref);
            priv->sequence_header = NULL;

            priv->sequence_header_ref = av_buffer_ref(unit->content_ref);
            if (!priv->sequence_header_ref)
                return AVERROR(ENOMEM);
            priv->sequence_header = &obu->obu.sequence_header;
        }
        break;
    case AV1_OBU_TEMPORAL_DELIMITER:
        {
            err = cbs_av1_write_temporal_delimiter_obu(ctx, pbc);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_FRAME_HEADER:
    case AV1_OBU_REDUNDANT_FRAME_HEADER:
        {
            err = cbs_av1_write_frame_header_obu(ctx, pbc,
                                                 &obu->obu.frame_header,
                                                 obu->header.obu_type ==
                                                 AV1_OBU_REDUNDANT_FRAME_HEADER,
                                                 NULL);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_TILE_GROUP:
        {
            err = cbs_av1_write_tile_group_obu(ctx, pbc,
                                               &obu->obu.tile_group);
            if (err < 0)
                return err;

            td = &obu->obu.tile_group.tile_data;
        }
        break;
    case AV1_OBU_FRAME:
        {
            err = cbs_av1_write_frame_obu(ctx, pbc, &obu->obu.frame, NULL);
            if (err < 0)
                return err;

            td = &obu->obu.frame.tile_group.tile_data;
        }
        break;
    case AV1_OBU_TILE_LIST:
        {
            err = cbs_av1_write_tile_list_obu(ctx, pbc, &obu->obu.tile_list);
            if (err < 0)
                return err;

            td = &obu->obu.tile_list.tile_data;
        }
        break;
    case AV1_OBU_METADATA:
        {
            err = cbs_av1_write_metadata_obu(ctx, pbc, &obu->obu.metadata);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_PADDING:
    default:
        return AVERROR(ENOSYS);
    }

    end_pos = put_bits_count(pbc);
    header_size = (end_pos - start_pos + 7) / 8;
    if (td) {
        obu->obu_size = header_size + td->data_size;
    } else if (header_size > 0) {
        // Add trailing bits and recalculate.
        err = cbs_av1_write_trailing_bits(ctx, pbc, 8 - end_pos % 8);
        if (err < 0)
            return err;
        end_pos = put_bits_count(pbc);
        obu->obu_size = header_size = (end_pos - start_pos + 7) / 8;
    } else {
        // Empty OBU.
        obu->obu_size = 0;
    }

    end_pos = put_bits_count(pbc);
    // Must now be byte-aligned.
    av_assert0(end_pos % 8 == 0);
    flush_put_bits(pbc);
    start_pos /= 8;
    end_pos   /= 8;

    *pbc = pbc_tmp;
    err = cbs_av1_write_leb128(ctx, pbc, "obu_size", obu->obu_size);
    if (err < 0)
        return err;

    data_pos = put_bits_count(pbc) / 8;
    flush_put_bits(pbc);
    av_assert0(data_pos <= start_pos);

    if (8 * obu->obu_size > put_bits_left(pbc))
        return AVERROR(ENOSPC);

    if (obu->obu_size > 0) {
        memmove(priv->write_buffer + data_pos,
                priv->write_buffer + start_pos, header_size);
        skip_put_bytes(pbc, header_size);

        if (td) {
            memcpy(priv->write_buffer + data_pos + header_size,
                   td->data, td->data_size);
            skip_put_bytes(pbc, td->data_size);
        }
    }

    return 0;
}