static u32 gf_isom_probe_type(u32 type)
{
	switch (type) {
	case GF_ISOM_BOX_TYPE_FTYP:
	case GF_ISOM_BOX_TYPE_MOOV:
		return 2;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	case GF_ISOM_BOX_TYPE_MOOF:
	case GF_ISOM_BOX_TYPE_STYP:
	case GF_ISOM_BOX_TYPE_SIDX:
	case GF_ISOM_BOX_TYPE_EMSG:
	case GF_ISOM_BOX_TYPE_PRFT:
    //we map free as segment when it is first in the file - a regular file shall start with ftyp or a file sig, not free
    //since our route stack may patch boxes to free for incomplete segments, we must map this to free
    case GF_ISOM_BOX_TYPE_FREE:
		return 3;
#ifndef GPAC_DISABLE_ISOM_ADOBE
	/*Adobe specific*/
	case GF_ISOM_BOX_TYPE_AFRA:
	case GF_ISOM_BOX_TYPE_ABST:
#endif
#endif
	case GF_ISOM_BOX_TYPE_MDAT:
	case GF_ISOM_BOX_TYPE_SKIP:
	case GF_ISOM_BOX_TYPE_UDTA:
	case GF_ISOM_BOX_TYPE_META:
	case GF_ISOM_BOX_TYPE_VOID:
	case GF_ISOM_BOX_TYPE_JP:
	case GF_QT_BOX_TYPE_WIDE:
		return 1;
	default:
		return 0;
	}
}