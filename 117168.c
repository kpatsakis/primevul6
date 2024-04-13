struct fuse *fuse_new(struct fuse_chan *ch, struct fuse_args *args,
		      const struct fuse_operations *op, size_t op_size,
		      void *user_data)
{
    struct fuse *f;
    struct node *root;
    struct fuse_fs *fs;
    struct fuse_lowlevel_ops llop = fuse_path_ops;

    if (fuse_create_context_key() == -1)
        goto out;

    f = (struct fuse *) calloc(1, sizeof(struct fuse));
    if (f == NULL) {
        fprintf(stderr, "fuse: failed to allocate fuse object\n");
        goto out_delete_context_key;
    }

    fs = fuse_fs_new(op, op_size, user_data);
    if (!fs)
        goto out_free;

    f->fs = fs;

    /* Oh f**k, this is ugly! */
    if (!fs->op.lock) {
        llop.getlk = NULL;
        llop.setlk = NULL;
    }

    f->conf.entry_timeout = 1.0;
    f->conf.attr_timeout = 1.0;
    f->conf.negative_timeout = 0.0;
    f->conf.intr_signal = FUSE_DEFAULT_INTR_SIGNAL;

    if (fuse_opt_parse(args, &f->conf, fuse_lib_opts, fuse_lib_opt_proc) == -1)
        goto out_free_fs;

#ifdef __SOLARIS__
    if (f->conf.modules) {
        char *module;
        char *next;

        for (module = f->conf.modules; module; module = next) {
            char *p;
            for (p = module; *p && *p != ':'; p++);
            next = *p ? p + 1 : NULL;
            *p = '\0';
            if (module[0] && fuse_push_module(f, module, args) == -1)
                goto out_free_fs;
        }
    }
#endif /* __SOLARIS__ */

    if (!f->conf.ac_attr_timeout_set)
        f->conf.ac_attr_timeout = f->conf.attr_timeout;

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    /*
     * In FreeBSD, we always use these settings as inode numbers are needed to
     * make getcwd(3) work.
     */
    f->conf.readdir_ino = 1;
#endif

    f->se = fuse_lowlevel_new(args, &llop, sizeof(llop), f);

    if (f->se == NULL) {
#ifdef __SOLARIS__
        if (f->conf.help)
            fuse_lib_help_modules();
#endif /* __SOLARIS__ */
        goto out_free_fs;
    }

    fuse_session_add_chan(f->se, ch);

    f->ctr = 0;
    f->generation = 0;
    /* FIXME: Dynamic hash table */
    f->name_table_size = 14057;
    f->name_table = (struct node **)
        calloc(1, sizeof(struct node *) * f->name_table_size);
    if (f->name_table == NULL) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        goto out_free_session;
    }

    f->id_table_size = 14057;
    f->id_table = (struct node **)
        calloc(1, sizeof(struct node *) * f->id_table_size);
    if (f->id_table == NULL) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        goto out_free_name_table;
    }

    fuse_mutex_init(&f->lock);
    pthread_rwlock_init(&f->tree_lock, NULL);

    root = (struct node *) calloc(1, sizeof(struct node));
    if (root == NULL) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        goto out_free_id_table;
    }

    root->name = strdup("/");
    if (root->name == NULL) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        goto out_free_root;
    }

    if (f->conf.intr &&
        fuse_init_intr_signal(f->conf.intr_signal, &f->intr_installed) == -1)
        goto out_free_root_name;

    root->parent = NULL;
    root->nodeid = FUSE_ROOT_ID;
    root->generation = 0;
    root->refctr = 1;
    root->nlookup = 1;
    hash_id(f, root);

    return f;

 out_free_root_name:
    free(root->name);
 out_free_root:
    free(root);
 out_free_id_table:
    free(f->id_table);
 out_free_name_table:
    free(f->name_table);
 out_free_session:
    fuse_session_destroy(f->se);
 out_free_fs:
    /* Horrible compatibility hack to stop the destructor from being
       called on the filesystem without init being called first */
    fs->op.destroy = NULL;
    fuse_fs_destroy(f->fs);
#ifdef __SOLARIS__
    free(f->conf.modules);
#endif /* __SOLARIS__ */
 out_free:
    free(f);
 out_delete_context_key:
    fuse_delete_context_key();
 out:
    return NULL;
}