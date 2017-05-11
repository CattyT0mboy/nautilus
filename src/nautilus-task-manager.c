#include "nautilus-task-manager.h"

#define TASK_LIMIT 1

typedef struct
{
    NautilusTask *task;

    NautilusTaskCallback callback;
    gpointer callback_data;
} NautilusTaskDetails;

struct _NautilusTaskManager
{
    GObject parent_instance;

    GThreadPool *thread_pool;
};

G_DEFINE_TYPE (NautilusTaskManager, nautilus_task_manager, G_TYPE_OBJECT)

static NautilusTaskManager *instance = NULL;

static GObject *
constructor (GType                  type,
             guint                  n_construct_properties,
             GObjectConstructParam *construct_properties)
{
    static GMutex mutex;
    GObjectClass *parent_class;

    g_mutex_lock (&mutex);

    if (instance != NULL)
    {
        g_mutex_unlock (&mutex);
        return g_object_ref (instance);
    }

    parent_class = G_OBJECT_CLASS (nautilus_task_manager_parent_class);
    instance = NAUTILUS_TASK_MANAGER (parent_class->constructor (type,
                                                                 n_construct_properties,
                                                                 construct_properties));

    g_object_add_weak_pointer (G_OBJECT (instance), (gpointer *) &instance);

    g_mutex_unlock (&mutex);

    return G_OBJECT (instance);
}

static void
finalize (GObject *object)
{
    NautilusTaskManager *self;

    self = NAUTILUS_TASK_MANAGER (object);

    g_thread_pool_free (self->thread_pool, TRUE, TRUE);
}

static void
nautilus_task_manager_class_init (NautilusTaskManagerClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructor = constructor;
    object_class->finalize = finalize;
}

static gboolean
call_callback (gpointer user_data)
{
    NautilusTaskDetails *details;

    details = user_data;

    if (details->callback != NULL)
    {
        details->callback (details->task, details->callback_data);
    }

    g_object_unref (details->task);
    g_free (details);

    return G_SOURCE_REMOVE;
}

static void
execute_task (gpointer data,
              gpointer user_data)
{
    NautilusTaskDetails *details;

    details = data;

    nautilus_task_execute (details->task, NULL, NULL);

    g_idle_add (call_callback, details);
}

static void
nautilus_task_manager_init (NautilusTaskManager *self)
{
    self->thread_pool = g_thread_pool_new (execute_task, self,
                                           TASK_LIMIT, FALSE,
                                           NULL);
}

void
nautilus_task_manager_queue_task (NautilusTaskManager  *self,
                                  NautilusTask         *task,
                                  NautilusTaskCallback  callback,
                                  gpointer              user_data)
{
    NautilusTaskDetails *details;

    g_return_if_fail (NAUTILUS_IS_TASK_MANAGER (self));
    g_return_if_fail (NAUTILUS_IS_TASK (task));

    details = g_new0 (NautilusTaskDetails, 1);

    details->task = g_object_ref (task);
    details->callback = callback;
    details->callback_data = user_data;

    g_thread_pool_push (self->thread_pool, details, NULL);
}

NautilusTaskManager *
nautilus_task_manager_dup_singleton (void)
{
    return g_object_new (NAUTILUS_TYPE_TASK_MANAGER, NULL);
}

