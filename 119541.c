MYSOFA_EXPORT void mysofa_free(struct MYSOFA_HRTF *hrtf) {
	if (!hrtf)
		return;

	while (hrtf->attributes) {
		struct MYSOFA_ATTRIBUTE *next = hrtf->attributes->next;
		free(hrtf->attributes->name);
		free(hrtf->attributes->value);
		free(hrtf->attributes);
		hrtf->attributes = next;
	}

	arrayFree(&hrtf->ListenerPosition);
	arrayFree(&hrtf->ReceiverPosition);
	arrayFree(&hrtf->SourcePosition);
	arrayFree(&hrtf->EmitterPosition);
	arrayFree(&hrtf->ListenerUp);
	arrayFree(&hrtf->ListenerView);
	arrayFree(&hrtf->DataIR);
	arrayFree(&hrtf->DataSamplingRate);
	arrayFree(&hrtf->DataDelay);
	free(hrtf);
}