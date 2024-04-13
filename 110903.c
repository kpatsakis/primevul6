static u32 vorbis_book_maptype1_quantvals(u32 entries, u32 dim)
{
	u32 vals = (u32)floor(pow(entries, 1.0 / dim));
	while (1) {
		u32 acc = 1;
		u32 acc1 = 1;
		u32 i;
		for (i = 0; i < dim; i++) {
			acc *= vals;
			acc1 *= vals + 1;
		}
		if (acc <= entries && acc1 > entries) return (vals);
		else {
			if (acc > entries) vals--;
			else vals++;
		}
	}
}