#include "blog_manager.h"

blog_manager::blog_manager(const string &path) {
  storage__ = make_unique<StorageType>(init_blog_storage(path));
  storage__->sync_schema();
}