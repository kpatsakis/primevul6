static int cbs_av1_read_unit(CodedBitstreamContext *ctx,
                             CodedBitstreamUnit *unit)
{
    CodedBitstreamAV1Context *priv = ctx->priv_data;
    AV1RawOBU *obu;
    GetBitContext gbc;
    int err, start_pos, end_pos;

    err = ff_cbs_alloc_unit_content(ctx, unit, sizeof(*obu),
                                    &cbs_av1_free_obu);
    if (err < 0)
        return err;
    obu = unit->content;

    err = init_get_bits(&gbc, unit->data, 8 * unit->data_size);
    if (err < 0)
        return err;

    err = cbs_av1_read_obu_header(ctx, &gbc, &obu->header);
    if (err < 0)
        return err;
    av_assert0(obu->header.obu_type == unit->type);

    if (obu->header.obu_has_size_field) {
        uint64_t obu_size;
        err = cbs_av1_read_leb128(ctx, &gbc, "obu_size", &obu_size);
        if (err < 0)
            return err;
        obu->obu_size = obu_size;
    } else {
        if (unit->data_size < 1 + obu->header.obu_extension_flag) {
            av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid OBU length: "
                   "unit too short (%"SIZE_SPECIFIER").\n", unit->data_size);
            return AVERROR_INVALIDDATA;
        }
        obu->obu_size = unit->data_size - 1 - obu->header.obu_extension_flag;
    }

    start_pos = get_bits_count(&gbc);

    if (obu->header.obu_extension_flag) {
        priv->temporal_id = obu->header.temporal_id;
        priv->spatial_id  = obu->header.temporal_id;

        if (obu->header.obu_type != AV1_OBU_SEQUENCE_HEADER &&
            obu->header.obu_type != AV1_OBU_TEMPORAL_DELIMITER &&
            priv->operating_point_idc) {
            int in_temporal_layer =
                (priv->operating_point_idc >>  priv->temporal_id    ) & 1;
            int in_spatial_layer  =
                (priv->operating_point_idc >> (priv->spatial_id + 8)) & 1;
            if (!in_temporal_layer || !in_spatial_layer) {
                // Decoding will drop this OBU at this operating point.
            }
        }
    } else {
        priv->temporal_id = 0;
        priv->spatial_id  = 0;
    }

    switch (obu->header.obu_type) {
    case AV1_OBU_SEQUENCE_HEADER:
        {
            err = cbs_av1_read_sequence_header_obu(ctx, &gbc,
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
            err = cbs_av1_read_temporal_delimiter_obu(ctx, &gbc);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_FRAME_HEADER:
    case AV1_OBU_REDUNDANT_FRAME_HEADER:
        {
            err = cbs_av1_read_frame_header_obu(ctx, &gbc,
                                                &obu->obu.frame_header,
                                                obu->header.obu_type ==
                                                AV1_OBU_REDUNDANT_FRAME_HEADER,
                                                unit->data_ref);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_TILE_GROUP:
        {
            err = cbs_av1_read_tile_group_obu(ctx, &gbc,
                                              &obu->obu.tile_group);
            if (err < 0)
                return err;

            err = cbs_av1_ref_tile_data(ctx, unit, &gbc,
                                        &obu->obu.tile_group.tile_data);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_FRAME:
        {
            err = cbs_av1_read_frame_obu(ctx, &gbc, &obu->obu.frame,
                                         unit->data_ref);
            if (err < 0)
                return err;

            err = cbs_av1_ref_tile_data(ctx, unit, &gbc,
                                        &obu->obu.frame.tile_group.tile_data);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_TILE_LIST:
        {
            err = cbs_av1_read_tile_list_obu(ctx, &gbc,
                                             &obu->obu.tile_list);
            if (err < 0)
                return err;

            err = cbs_av1_ref_tile_data(ctx, unit, &gbc,
                                        &obu->obu.tile_list.tile_data);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_METADATA:
        {
            err = cbs_av1_read_metadata_obu(ctx, &gbc, &obu->obu.metadata);
            if (err < 0)
                return err;
        }
        break;
    case AV1_OBU_PADDING:
    default:
        return AVERROR(ENOSYS);
    }

    end_pos = get_bits_count(&gbc);
    av_assert0(end_pos <= unit->data_size * 8);

    if (obu->obu_size > 0 &&
        obu->header.obu_type != AV1_OBU_TILE_GROUP &&
        obu->header.obu_type != AV1_OBU_FRAME) {
        err = cbs_av1_read_trailing_bits(ctx, &gbc,
                                         obu->obu_size * 8 + start_pos - end_pos);
        if (err < 0)
            return err;
    }

    return 0;
}