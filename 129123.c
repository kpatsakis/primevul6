static void audit_log_task_info(struct audit_buffer *ab, struct task_struct *tsk)
{
	char name[sizeof(tsk->comm)];
	struct mm_struct *mm = tsk->mm;
	struct vm_area_struct *vma;

	/* tsk == current */

	get_task_comm(name, tsk);
	audit_log_format(ab, " comm=");
	audit_log_untrustedstring(ab, name);

	if (mm) {
		down_read(&mm->mmap_sem);
		vma = mm->mmap;
		while (vma) {
			if ((vma->vm_flags & VM_EXECUTABLE) &&
			    vma->vm_file) {
				audit_log_d_path(ab, "exe=",
						 vma->vm_file->f_path.dentry,
						 vma->vm_file->f_path.mnt);
				break;
			}
			vma = vma->vm_next;
		}
		up_read(&mm->mmap_sem);
	}
	audit_log_task_context(ab);
}