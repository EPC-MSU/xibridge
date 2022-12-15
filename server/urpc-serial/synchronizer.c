#include "synchronizer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "tinycthread.h"
#include <zf_log.h>


struct urpc_synchronizer_t
{
    mtx_t mutex;
    cnd_t cond;
    // handle "spurious wakeup" problem
    bool really_notified;
    unsigned int acquire_counter;
};


struct urpc_synchronizer_t *
urpc_syncronizer_create(
)
{
    struct urpc_synchronizer_t *s = malloc(sizeof(struct urpc_synchronizer_t));

    if (s == NULL)
    {
        goto malloc_failed;
    }

    if (mtx_init(&s->mutex, mtx_plain) != thrd_success)
    {
        goto mutex_init_failed;
    }

    if (cnd_init(&s->cond) != thrd_success)
    {
        goto cond_init_failed;
    }

    s->really_notified = false;
    s->acquire_counter = 1;

    return s;

cond_init_failed:
    mtx_destroy(&s->mutex);

mutex_init_failed:
    free(s);

malloc_failed:
    ZF_LOGE("Can't create synchronizer");
    return NULL;
}


int urpc_synchronizer_acquire(
    struct urpc_synchronizer_t *s
)
{
    assert(s != NULL);

    if (mtx_lock(&s->mutex) != thrd_success)
    {
        ZF_LOGE("can't acquire synchronizer");
        return 1;
    }

    s->acquire_counter++;
    return 0;
}


// Must be called from the same thread which previously called urpc_synchronizer_acquire!
int urpc_synchronizer_release(
    struct urpc_synchronizer_t *s
)
{
    assert(s != NULL && s->acquire_counter >= 1);

    s->acquire_counter--;
    if (s->acquire_counter == 0)
    {
        s->really_notified = true;
        if (cnd_signal(&s->cond) != thrd_success)
        {
            ZF_LOGE("can't release synchronizer");
            return 1;
        }
    }

    if (mtx_unlock(&s->mutex) != thrd_success)
    {
        ZF_LOGE("can't release synchronizer");
        return 1;
    }

    return 0;
}


int urpc_synchronizer_destroy(
    struct urpc_synchronizer_t *s
)
{
    assert(s != NULL);

    if (mtx_lock(&s->mutex) != thrd_success)
    {
        ZF_LOGE("can't destroy synchronizer");
        return 1;
    }

    s->acquire_counter--;
    if (s->acquire_counter > 0)
    {
        // wait for pending in-flight requests to complete (and fight the "spurious wakeup" problem)
        while (!s->really_notified)
        {
            if (cnd_wait(&s->cond, &s->mutex) != thrd_success)
            {
                ZF_LOGE("can't destroy synchronizer");
                return 1;
            }
        }

    }

    assert(s->acquire_counter == 0);
    // at this point there SHOULD be no more users of the device except this stackframe so just unlock the mutex and cleanup resource
    if (mtx_unlock(&s->mutex) != thrd_success)
    {
        ZF_LOGE("can't destroy synchronizer");
        return 1;
    }

    mtx_destroy(&s->mutex);
    cnd_destroy(&s->cond);
    free(s);

    return 0;
}
