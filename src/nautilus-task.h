#ifndef NAUTILUS_TASK_H
#define NAUTILUS_TASK_H

#include <gio/gio.h>
#include <glib-object.h>

#define NAUTILUS_TYPE_TASK (nautilus_task_get_type ())

G_DECLARE_INTERFACE (NautilusTask, nautilus_task, NAUTILUS, TASK, GObject)

typedef void (*NautilusTaskCallback) (NautilusTask *task,
                                      gpointer      user_data);

struct _NautilusTaskInterface
{
    GTypeInterface g_iface;

    /* Public methods */
    GCancellable * (*get_cancellable) (NautilusTask *task);

    void           (*execute)         (NautilusTask *task);
};

GCancellable *nautilus_task_get_cancellable (NautilusTask *task);

gboolean nautilus_task_is_canceled (NautilusTask *task);

void nautilus_task_cancel (NautilusTask *task);
void nautilus_task_execute (NautilusTask         *task,
                            NautilusTaskCallback  callback,
                            gpointer              user_data);

#endif
