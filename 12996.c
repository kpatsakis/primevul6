drill_update_image_info_min_max_from_bbox(gerbv_image_info_t *info,
		const gerbv_render_size_t *bbox)
{
    info->min_x = MIN(info->min_x, bbox->left);
    info->min_y = MIN(info->min_y, bbox->bottom);
    info->max_x = MAX(info->max_x, bbox->right);
    info->max_y = MAX(info->max_y, bbox->top);
}