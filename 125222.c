bool AES_GCM_EncryptContext::Encrypt(
	const void *pPlaintextData, size_t cbPlaintextData,
	const void *pIV,
	void *pEncryptedDataAndTag, uint32 *pcbEncryptedDataAndTag,
	const void *pAdditionalAuthenticationData, size_t cbAuthenticationData
) {

	// Make sure caller's buffer is big enough to hold the result.
	if ( cbPlaintextData + crypto_aead_aes256gcm_ABYTES > *pcbEncryptedDataAndTag )
	{
		*pcbEncryptedDataAndTag = 0;
		return false;
	}

    unsigned long long cbEncryptedDataAndTag_longlong;
    crypto_aead_aes256gcm_encrypt_afternm(
		static_cast<unsigned char*>( pEncryptedDataAndTag ), &cbEncryptedDataAndTag_longlong,
		static_cast<const unsigned char*>( pPlaintextData ), cbPlaintextData,
		static_cast<const unsigned char*>(pAdditionalAuthenticationData), cbAuthenticationData,
		nullptr,
		static_cast<const unsigned char*>( pIV ),
		static_cast<const crypto_aead_aes256gcm_state*>( m_ctx )
	);

    *pcbEncryptedDataAndTag = cbEncryptedDataAndTag_longlong;

    return true;
}