void ssl3_cbc_digest_record(
	const EVP_MD_CTX *ctx,
	unsigned char* md_out,
	size_t* md_out_size,
	const unsigned char header[13],
	const unsigned char *data,
	size_t data_plus_mac_size,
	size_t data_plus_mac_plus_padding_size,
	const unsigned char *mac_secret,
	unsigned mac_secret_length,
	char is_sslv3)
	{
	unsigned char md_state[sizeof(SHA512_CTX)];
	void (*md_final_raw)(void *ctx, unsigned char *md_out);
	void (*md_transform)(void *ctx, const unsigned char *block);
	unsigned md_size, md_block_size = 64;
	unsigned sslv3_pad_length = 40, header_length, variance_blocks,
		 len, max_mac_bytes, num_blocks,
		 num_starting_blocks, k, mac_end_offset, c, index_a, index_b;
	unsigned int bits;	/* at most 18 bits */
	unsigned char length_bytes[MAX_HASH_BIT_COUNT_BYTES];
	/* hmac_pad is the masked HMAC key. */
	unsigned char hmac_pad[MAX_HASH_BLOCK_SIZE];
	unsigned char first_block[MAX_HASH_BLOCK_SIZE];
	unsigned char mac_out[EVP_MAX_MD_SIZE];
	unsigned i, j, md_out_size_u;
	EVP_MD_CTX md_ctx;
	/* mdLengthSize is the number of bytes in the length field that terminates
	* the hash. */
	unsigned md_length_size = 8;

	/* This is a, hopefully redundant, check that allows us to forget about
	 * many possible overflows later in this function. */
	OPENSSL_assert(data_plus_mac_plus_padding_size < 1024*1024);

	switch (ctx->digest->type)
		{
		case NID_md5:
			MD5_Init((MD5_CTX*)md_state);
			md_final_raw = tls1_md5_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) MD5_Transform;
			md_size = 16;
			sslv3_pad_length = 48;
			break;
		case NID_sha1:
			SHA1_Init((SHA_CTX*)md_state);
			md_final_raw = tls1_sha1_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) SHA1_Transform;
			md_size = 20;
			break;
		case NID_sha224:
			SHA224_Init((SHA256_CTX*)md_state);
			md_final_raw = tls1_sha256_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) SHA256_Transform;
			md_size = 224/8;
			break;
		case NID_sha256:
			SHA256_Init((SHA256_CTX*)md_state);
			md_final_raw = tls1_sha256_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) SHA256_Transform;
			md_size = 32;
			break;
		case NID_sha384:
			SHA384_Init((SHA512_CTX*)md_state);
			md_final_raw = tls1_sha512_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) SHA512_Transform;
			md_size = 384/8;
			md_block_size = 128;
			md_length_size = 16;
			break;
		case NID_sha512:
			SHA512_Init((SHA512_CTX*)md_state);
			md_final_raw = tls1_sha512_final_raw;
			md_transform = (void(*)(void *ctx, const unsigned char *block)) SHA512_Transform;
			md_size = 64;
			md_block_size = 128;
			md_length_size = 16;
			break;
		default:
			/* ssl3_cbc_record_digest_supported should have been
			 * called first to check that the hash function is
			 * supported. */
			OPENSSL_assert(0);
			if (md_out_size)
				*md_out_size = -1;
			return;
		}

	OPENSSL_assert(md_length_size <= MAX_HASH_BIT_COUNT_BYTES);
	OPENSSL_assert(md_block_size <= MAX_HASH_BLOCK_SIZE);
	OPENSSL_assert(md_size <= EVP_MAX_MD_SIZE);

	header_length = 13;
	if (is_sslv3)
		{
		header_length =
			mac_secret_length +
			sslv3_pad_length +
			8 /* sequence number */ +
			1 /* record type */ +
			2 /* record length */;
		}

	/* variance_blocks is the number of blocks of the hash that we have to
	 * calculate in constant time because they could be altered by the
	 * padding value.
	 *
	 * In SSLv3, the padding must be minimal so the end of the plaintext
	 * varies by, at most, 15+20 = 35 bytes. (We conservatively assume that
	 * the MAC size varies from 0..20 bytes.) In case the 9 bytes of hash
	 * termination (0x80 + 64-bit length) don't fit in the final block, we
	 * say that the final two blocks can vary based on the padding.
	 *
	 * TLSv1 has MACs up to 48 bytes long (SHA-384) and the padding is not
	 * required to be minimal. Therefore we say that the final six blocks
	 * can vary based on the padding.
	 *
	 * Later in the function, if the message is short and there obviously
	 * cannot be this many blocks then variance_blocks can be reduced. */
	variance_blocks = is_sslv3 ? 2 : 6;
	/* From now on we're dealing with the MAC, which conceptually has 13
	 * bytes of `header' before the start of the data (TLS) or 71/75 bytes
	 * (SSLv3) */
	len = data_plus_mac_plus_padding_size + header_length;
	/* max_mac_bytes contains the maximum bytes of bytes in the MAC, including
	* |header|, assuming that there's no padding. */
	max_mac_bytes = len - md_size - 1;
	/* num_blocks is the maximum number of hash blocks. */
	num_blocks = (max_mac_bytes + 1 + md_length_size + md_block_size - 1) / md_block_size;
	/* In order to calculate the MAC in constant time we have to handle
	 * the final blocks specially because the padding value could cause the
	 * end to appear somewhere in the final |variance_blocks| blocks and we
	 * can't leak where. However, |num_starting_blocks| worth of data can
	 * be hashed right away because no padding value can affect whether
	 * they are plaintext. */
	num_starting_blocks = 0;
	/* k is the starting byte offset into the conceptual header||data where
	 * we start processing. */
	k = 0;
	/* mac_end_offset is the index just past the end of the data to be
	 * MACed. */
	mac_end_offset = data_plus_mac_size + header_length - md_size;
	/* c is the index of the 0x80 byte in the final hash block that
	 * contains application data. */
	c = mac_end_offset % md_block_size;
	/* index_a is the hash block number that contains the 0x80 terminating
	 * value. */
	index_a = mac_end_offset / md_block_size;
	/* index_b is the hash block number that contains the 64-bit hash
	 * length, in bits. */
	index_b = (mac_end_offset + md_length_size) / md_block_size;
	/* bits is the hash-length in bits. It includes the additional hash
	 * block for the masked HMAC key, or whole of |header| in the case of
	 * SSLv3. */

	/* For SSLv3, if we're going to have any starting blocks then we need
	 * at least two because the header is larger than a single block. */
	if (num_blocks > variance_blocks + (is_sslv3 ? 1 : 0))
		{
		num_starting_blocks = num_blocks - variance_blocks;
		k = md_block_size*num_starting_blocks;
		}

	bits = 8*mac_end_offset;
	if (!is_sslv3)
		{
		/* Compute the initial HMAC block. For SSLv3, the padding and
		 * secret bytes are included in |header| because they take more
		 * than a single block. */
		bits += 8*md_block_size;
		memset(hmac_pad, 0, md_block_size);
		OPENSSL_assert(mac_secret_length <= sizeof(hmac_pad));
		memcpy(hmac_pad, mac_secret, mac_secret_length);
		for (i = 0; i < md_block_size; i++)
			hmac_pad[i] ^= 0x36;

		md_transform(md_state, hmac_pad);
		}

	memset(length_bytes,0,md_length_size-4);
	length_bytes[md_length_size-4] = (unsigned char)(bits>>24);
	length_bytes[md_length_size-3] = (unsigned char)(bits>>16);
	length_bytes[md_length_size-2] = (unsigned char)(bits>>8);
	length_bytes[md_length_size-1] = (unsigned char)bits;

	if (k > 0)
		{
		if (is_sslv3)
			{
			/* The SSLv3 header is larger than a single block.
			 * overhang is the number of bytes beyond a single
			 * block that the header consumes: either 7 bytes
			 * (SHA1) or 11 bytes (MD5). */
			unsigned overhang = header_length-md_block_size;
			md_transform(md_state, header);
			memcpy(first_block, header + md_block_size, overhang);
			memcpy(first_block + overhang, data, md_block_size-overhang);
			md_transform(md_state, first_block);
			for (i = 1; i < k/md_block_size - 1; i++)
				md_transform(md_state, data + md_block_size*i - overhang);
			}
		else
			{
			/* k is a multiple of md_block_size. */
			memcpy(first_block, header, 13);
			memcpy(first_block+13, data, md_block_size-13);
			md_transform(md_state, first_block);
			for (i = 1; i < k/md_block_size; i++)
				md_transform(md_state, data + md_block_size*i - 13);
			}
		}

	memset(mac_out, 0, sizeof(mac_out));

	/* We now process the final hash blocks. For each block, we construct
	 * it in constant time. If the |i==index_a| then we'll include the 0x80
	 * bytes and zero pad etc. For each block we selectively copy it, in
	 * constant time, to |mac_out|. */
	for (i = num_starting_blocks; i <= num_starting_blocks+variance_blocks; i++)
		{
		unsigned char block[MAX_HASH_BLOCK_SIZE];
		unsigned char is_block_a = constant_time_eq_8(i, index_a);
		unsigned char is_block_b = constant_time_eq_8(i, index_b);
		for (j = 0; j < md_block_size; j++)
			{
			unsigned char b = 0, is_past_c, is_past_cp1;
			if (k < header_length)
				b = header[k];
			else if (k < data_plus_mac_plus_padding_size + header_length)
				b = data[k-header_length];
			k++;

			is_past_c = is_block_a & constant_time_ge(j, c);
			is_past_cp1 = is_block_a & constant_time_ge(j, c+1);
			/* If this is the block containing the end of the
			 * application data, and we are at the offset for the
			 * 0x80 value, then overwrite b with 0x80. */
			b = (b&~is_past_c) | (0x80&is_past_c);
			/* If this the the block containing the end of the
			 * application data and we're past the 0x80 value then
			 * just write zero. */
			b = b&~is_past_cp1;
			/* If this is index_b (the final block), but not
			 * index_a (the end of the data), then the 64-bit
			 * length didn't fit into index_a and we're having to
			 * add an extra block of zeros. */
			b &= ~is_block_b | is_block_a;

			/* The final bytes of one of the blocks contains the
			 * length. */
			if (j >= md_block_size - md_length_size)
				{
				/* If this is index_b, write a length byte. */
				b = (b&~is_block_b) | (is_block_b&length_bytes[j-(md_block_size-md_length_size)]);
				}
			block[j] = b;
			}

		md_transform(md_state, block);
		md_final_raw(md_state, block);
		/* If this is index_b, copy the hash value to |mac_out|. */
		for (j = 0; j < md_size; j++)
			mac_out[j] |= block[j]&is_block_b;
		}

	EVP_MD_CTX_init(&md_ctx);
	EVP_DigestInit_ex(&md_ctx, ctx->digest, NULL /* engine */);
	if (is_sslv3)
		{
		/* We repurpose |hmac_pad| to contain the SSLv3 pad2 block. */
		memset(hmac_pad, 0x5c, sslv3_pad_length);

		EVP_DigestUpdate(&md_ctx, mac_secret, mac_secret_length);
		EVP_DigestUpdate(&md_ctx, hmac_pad, sslv3_pad_length);
		EVP_DigestUpdate(&md_ctx, mac_out, md_size);
		}
	else
		{
		/* Complete the HMAC in the standard manner. */
		for (i = 0; i < md_block_size; i++)
			hmac_pad[i] ^= 0x6a;

		EVP_DigestUpdate(&md_ctx, hmac_pad, md_block_size);
		EVP_DigestUpdate(&md_ctx, mac_out, md_size);
		}
	EVP_DigestFinal(&md_ctx, md_out, &md_out_size_u);
	if (md_out_size)
		*md_out_size = md_out_size_u;
	EVP_MD_CTX_cleanup(&md_ctx);
	}