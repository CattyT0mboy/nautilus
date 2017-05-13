#ifndef NAUTILUS_FILE_TASK_H
#define NAUTILUS_FILE_TASK_H

#include "nautilus-task.h"

#define NAUTILUS_TYPE_FILE_TASK (nautilus_file_task_get_type ())

G_DECLARE_DERIVABLE_TYPE (NautilusFileTask, nautilus_file_task,
                          NAUTILUS, FILE_TASK,
                          GObject)

struct _NautilusFileTaskClass
{
    GObjectClass parent_class;

    void (*execute) (NautilusTask *task);
};

#endif
