void fractalheapFree(struct FRACTALHEAP *fractalheap) {
	free(fractalheap->filter_information);
}