static inline unsigned long *frame_vector_pfns(struct frame_vector *vec)
{
	if (!vec->is_pfns)
		frame_vector_to_pfns(vec);
	return (unsigned long *)(vec->ptrs);
}