static inline bool desc_equal(const struct desc_struct *d1,
			      const struct desc_struct *d2)
{
	return !memcmp(d1, d2, sizeof(*d1));
}