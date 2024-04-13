xfs_attr3_leaf_lookup_int(
	struct xfs_buf		*bp,
	struct xfs_da_args	*args)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_attr_leaf_entry *entries;
	struct xfs_attr_leaf_name_local *name_loc;
	struct xfs_attr_leaf_name_remote *name_rmt;
	xfs_dahash_t		hashval;
	int			probe;
	int			span;

	trace_xfs_attr_leaf_lookup(args);

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&ichdr, leaf);
	entries = xfs_attr3_leaf_entryp(leaf);
	ASSERT(ichdr.count < XFS_LBSIZE(args->dp->i_mount) / 8);

	/*
	 * Binary search.  (note: small blocks will skip this loop)
	 */
	hashval = args->hashval;
	probe = span = ichdr.count / 2;
	for (entry = &entries[probe]; span > 4; entry = &entries[probe]) {
		span /= 2;
		if (be32_to_cpu(entry->hashval) < hashval)
			probe += span;
		else if (be32_to_cpu(entry->hashval) > hashval)
			probe -= span;
		else
			break;
	}
	ASSERT(probe >= 0 && (!ichdr.count || probe < ichdr.count));
	ASSERT(span <= 4 || be32_to_cpu(entry->hashval) == hashval);

	/*
	 * Since we may have duplicate hashval's, find the first matching
	 * hashval in the leaf.
	 */
	while (probe > 0 && be32_to_cpu(entry->hashval) >= hashval) {
		entry--;
		probe--;
	}
	while (probe < ichdr.count &&
	       be32_to_cpu(entry->hashval) < hashval) {
		entry++;
		probe++;
	}
	if (probe == ichdr.count || be32_to_cpu(entry->hashval) != hashval) {
		args->index = probe;
		return XFS_ERROR(ENOATTR);
	}

	/*
	 * Duplicate keys may be present, so search all of them for a match.
	 */
	for (; probe < ichdr.count && (be32_to_cpu(entry->hashval) == hashval);
			entry++, probe++) {
/*
 * GROT: Add code to remove incomplete entries.
 */
		/*
		 * If we are looking for INCOMPLETE entries, show only those.
		 * If we are looking for complete entries, show only those.
		 */
		if ((args->flags & XFS_ATTR_INCOMPLETE) !=
		    (entry->flags & XFS_ATTR_INCOMPLETE)) {
			continue;
		}
		if (entry->flags & XFS_ATTR_LOCAL) {
			name_loc = xfs_attr3_leaf_name_local(leaf, probe);
			if (name_loc->namelen != args->namelen)
				continue;
			if (memcmp(args->name, name_loc->nameval,
							args->namelen) != 0)
				continue;
			if (!xfs_attr_namesp_match(args->flags, entry->flags))
				continue;
			args->index = probe;
			return XFS_ERROR(EEXIST);
		} else {
			name_rmt = xfs_attr3_leaf_name_remote(leaf, probe);
			if (name_rmt->namelen != args->namelen)
				continue;
			if (memcmp(args->name, name_rmt->name,
							args->namelen) != 0)
				continue;
			if (!xfs_attr_namesp_match(args->flags, entry->flags))
				continue;
			args->index = probe;
			args->rmtblkno = be32_to_cpu(name_rmt->valueblk);
			args->rmtblkcnt = XFS_B_TO_FSB(args->dp->i_mount,
						   be32_to_cpu(name_rmt->valuelen));
			return XFS_ERROR(EEXIST);
		}
	}
	args->index = probe;
	return XFS_ERROR(ENOATTR);
}