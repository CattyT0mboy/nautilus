#include "nautilus-task.h"

G_DEFINE_INTERFACE (NautilusTask, nautilus_task, G_TYPE_OBJECT)

static void
nautilus_task_default_init (NautilusTaskInterface *klass)
{
}

GCancellable *
nautilus_task_get_cancellable (NautilusTask *task)
{
    NautilusTaskInterface *iface;

    g_return_val_if_fail (NAUTILUS_IS_TASK (task), NULL);

    iface = NAUTILUS_TASK_GET_IFACE (task);

    g_return_val_if_fail (iface->get_cancellable != NULL, NULL);

    return iface->get_cancellable (task);
}

gboolean
nautilus_task_is_canceled (NautilusTask *task)
{
    GCancellable *cancellable;

    g_return_val_if_fail (NAUTILUS_IS_TASK (task), FALSE);

    cancellable = nautilus_task_get_cancellable (task);

    return g_cancellable_is_cancelled (cancellable);
}

void
nautilus_task_cancel (NautilusTask *task)
{
    GCancellable *cancellable;

    g_return_if_fail (NAUTILUS_IS_TASK (task));

    cancellable = nautilus_task_get_cancellable (task);

    g_cancellable_cancel (cancellable);
}

void
nautilus_task_execute (NautilusTask         *task,
                       NautilusTaskCallback  callback,
                       gpointer              user_data)
{
    NautilusTaskInterface *iface;

    g_return_if_fail (NAUTILUS_IS_TASK (task));

    iface = NAUTILUS_TASK_GET_IFACE (task);

    g_return_if_fail (iface->execute != NULL);

    iface->execute (task);
    if (callback != NULL)
    {
        callback (task, user_data);
    }
}
