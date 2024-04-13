GF_Err hinf_AddBox(GF_Box *s, GF_Box *a)
{
	GF_MAXRBox *maxR;
	GF_HintInfoBox *hinf = (GF_HintInfoBox *)s;
	u32 i;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_MAXR:
		i=0;
		while ((maxR = (GF_MAXRBox *)gf_list_enum(hinf->other_boxes, &i))) {
			if ((maxR->type==GF_ISOM_BOX_TYPE_MAXR) && (maxR->granularity == ((GF_MAXRBox *)a)->granularity))
				return GF_ISOM_INVALID_FILE;
		}
		break;
	}
	return gf_isom_box_add_default(s, a);
}