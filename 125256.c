static int _php_image_type (char data[8])
{
	/* Based on ext/standard/image.c */

	if (data == NULL) {
		return -1;
	}

	if (!memcmp(data, php_sig_gd2, 3)) {
		return PHP_GDIMG_TYPE_GD2;
	} else if (!memcmp(data, php_sig_jpg, 3)) {
		return PHP_GDIMG_TYPE_JPG;
	} else if (!memcmp(data, php_sig_png, 3)) {
		if (!memcmp(data, php_sig_png, 8)) {
			return PHP_GDIMG_TYPE_PNG;
		}
	} else if (!memcmp(data, php_sig_gif, 3)) {
		return PHP_GDIMG_TYPE_GIF;
	}
	else {
		gdIOCtx *io_ctx;
		io_ctx = gdNewDynamicCtxEx(8, data, 0);
		if (io_ctx) {
			if (_php_ctx_getmbi(io_ctx) == 0 && _php_ctx_getmbi(io_ctx) >= 0) {
				io_ctx->gd_free(io_ctx);
				return PHP_GDIMG_TYPE_WBM;
			} else {
				io_ctx->gd_free(io_ctx);
			}
		}
	}
	return -1;
}