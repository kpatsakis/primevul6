GF_ESD *gf_m2ts_get_esd(GF_M2TS_ES *es)
{
	GF_ESD *esd;
	u32 k, esd_count;

	esd = NULL;
	if (es->program->pmt_iod && es->program->pmt_iod->ESDescriptors) {
		esd_count = gf_list_count(es->program->pmt_iod->ESDescriptors);
		for (k = 0; k < esd_count; k++) {
			GF_ESD *esd_tmp = (GF_ESD *)gf_list_get(es->program->pmt_iod->ESDescriptors, k);
			if (esd_tmp->ESID != es->mpeg4_es_id) continue;
			esd = esd_tmp;
			break;
		}
	}

	if (!esd && es->program->additional_ods) {
		u32 od_count, od_index;
		od_count = gf_list_count(es->program->additional_ods);
		for (od_index = 0; od_index < od_count; od_index++) {
			GF_ObjectDescriptor *od = (GF_ObjectDescriptor *)gf_list_get(es->program->additional_ods, od_index);
			esd_count = gf_list_count(od->ESDescriptors);
			for (k = 0; k < esd_count; k++) {
				GF_ESD *esd_tmp = (GF_ESD *)gf_list_get(od->ESDescriptors, k);
				if (esd_tmp->ESID != es->mpeg4_es_id) continue;
				esd = esd_tmp;
				break;
			}
		}
	}

	return esd;
}