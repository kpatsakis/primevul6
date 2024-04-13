static int rebasing_and_stripping_io_read(RIO *io, RIODesc *fd, ut8 *buf, int count) {
	r_return_val_if_fail (io, -1);
	RCore *core = (RCore*) io->corebind.core;
	if (!core || !core->bin || !core->bin->binfiles) {
		return -1;
	}
	struct MACH0_(obj_t) *obj = NULL;
	RListIter *iter;
	RBinFile *bf;
	r_list_foreach (core->bin->binfiles, iter, bf) {
		if (bf->fd == fd->fd ) {
			/* The first field of MACH0_(obj_t) is
			 * the mach_header, whose first field is
			 * the MH magic.
			 * This code assumes that bin objects are
			 * at least 4 bytes long.
			 */
			ut32 *magic = bf->o->bin_obj;
			if (magic && (*magic == MH_MAGIC ||
					*magic == MH_CIGAM ||
					*magic == MH_MAGIC_64 ||
					*magic == MH_CIGAM_64)) {
				obj = bf->o->bin_obj;
			}
			break;
		}
	}
	if (!obj || !obj->original_io_read) {
		if (fd->plugin->read == &rebasing_and_stripping_io_read) {
			return -1;
		}
		return fd->plugin->read (io, fd, buf, count);
	}
	if (obj->rebasing_buffer) {
		return obj->original_io_read (io, fd, buf, count);
	}
	static ut8 *internal_buffer = NULL;
	static int internal_buf_size = 0;
	if (count > internal_buf_size) {
		if (internal_buffer) {
			R_FREE (internal_buffer);
			internal_buffer = NULL;
		}
		internal_buf_size = R_MAX (count, 8);
		internal_buffer = (ut8 *) malloc (internal_buf_size);
	}
	ut64 io_off = io->off;
	int result = obj->original_io_read (io, fd, internal_buffer, count);
	if (result == count) {
		rebase_buffer (obj, io_off - bf->o->boffset, fd, internal_buffer, count);
		memcpy (buf, internal_buffer, result);
	}
	return result;
}