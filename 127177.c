static int vhost_vdpa_map(struct vhost_vdpa *v, u64 iova,
			  u64 size, u64 pa, u32 perm, void *opaque)
{
	struct vhost_dev *dev = &v->vdev;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	int r = 0;

	r = vhost_iotlb_add_range_ctx(dev->iotlb, iova, iova + size - 1,
				      pa, perm, opaque);
	if (r)
		return r;

	if (ops->dma_map) {
		r = ops->dma_map(vdpa, iova, size, pa, perm, opaque);
	} else if (ops->set_map) {
		if (!v->in_batch)
			r = ops->set_map(vdpa, dev->iotlb);
	} else {
		r = iommu_map(v->domain, iova, pa, size,
			      perm_to_iommu_flags(perm));
	}
	if (r) {
		vhost_iotlb_del_range(dev->iotlb, iova, iova + size - 1);
		return r;
	}

	if (!vdpa->use_va)
		atomic64_add(PFN_DOWN(size), &dev->mm->pinned_vm);

	return 0;
}