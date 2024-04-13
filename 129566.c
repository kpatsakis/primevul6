crypt_derive_key_sha1(const void *p, int size, unsigned char *key,
    int key_size)
{
#define MD_SIZE 20
	archive_sha1_ctx ctx;
	unsigned char md1[MD_SIZE];
	unsigned char md2[MD_SIZE * 2];
	unsigned char mkb[64];
	int i;

	archive_sha1_init(&ctx);
	archive_sha1_update(&ctx, p, size);
	archive_sha1_final(&ctx, md1);

	memset(mkb, 0x36, sizeof(mkb));
	for (i = 0; i < MD_SIZE; i++)
		mkb[i] ^= md1[i];
	archive_sha1_init(&ctx);
	archive_sha1_update(&ctx, mkb, sizeof(mkb));
	archive_sha1_final(&ctx, md2);

	memset(mkb, 0x5C, sizeof(mkb));
	for (i = 0; i < MD_SIZE; i++)
		mkb[i] ^= md1[i];
	archive_sha1_init(&ctx);
	archive_sha1_update(&ctx, mkb, sizeof(mkb));
	archive_sha1_final(&ctx, md2 + MD_SIZE);

	if (key_size > 32)
		key_size = 32;
	memcpy(key, md2, key_size);
#undef MD_SIZE
}