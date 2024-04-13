transform_delta_inverse(const gs_point *pdelta, const gs_matrix *pmat,
                        gs_point *ppt)
{
    int code = gs_distance_transform_inverse(pdelta->x, pdelta->y, pmat, ppt);
    gs_point delta;

    if (code < 0)
        return code;
    if (ppt->y == 0)
        return 0;
    /* Check for numerical fuzz. */
    code = gs_distance_transform(ppt->x, 0.0, pmat, &delta);
    if (code < 0)
        return 0;                /* punt */
    if (fabs(delta.x - pdelta->x) < 0.01 && fabs(delta.y - pdelta->y) < 0.01) {
        /* Close enough to y == 0: device space error < 0.01 pixel. */
        ppt->y = 0;
    }
    return 0;
}