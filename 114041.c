static inline const char *htx_blk_type_str(enum htx_blk_type type)
{
	switch (type) {
		case HTX_BLK_REQ_SL: return "HTX_BLK_REQ_SL";
		case HTX_BLK_RES_SL: return "HTX_BLK_RES_SL";
		case HTX_BLK_HDR:    return "HTX_BLK_HDR";
		case HTX_BLK_EOH:    return "HTX_BLK_EOH";
		case HTX_BLK_DATA:   return "HTX_BLK_DATA";
		case HTX_BLK_TLR:    return "HTX_BLK_TLR";
		case HTX_BLK_EOT:    return "HTX_BLK_EOT";
		case HTX_BLK_UNUSED: return "HTX_BLK_UNUSED";
		default:             return "HTX_BLK_???";
	};
}