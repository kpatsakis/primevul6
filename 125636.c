static void cbs_av1_free_obu(void *unit, uint8_t *content)
{
    AV1RawOBU *obu = (AV1RawOBU*)content;

    switch (obu->header.obu_type) {
    case AV1_OBU_TILE_GROUP:
        cbs_av1_free_tile_data(&obu->obu.tile_group.tile_data);
        break;
    case AV1_OBU_FRAME:
        cbs_av1_free_tile_data(&obu->obu.frame.tile_group.tile_data);
        break;
    case AV1_OBU_TILE_LIST:
        cbs_av1_free_tile_data(&obu->obu.tile_list.tile_data);
        break;
    case AV1_OBU_METADATA:
        cbs_av1_free_metadata(&obu->obu.metadata);
        break;
    }

    av_freep(&obu);
}