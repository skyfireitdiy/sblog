#include "blog_manage.h"

void blog_manage::blog_manage(const string &path)
{
    storage__ = make_unique<StorageType >(init_user_storage(path));
    storage__->sync_schema();
}