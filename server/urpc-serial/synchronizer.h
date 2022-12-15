#ifndef URPC_SYNCHRONIZER_H
#define URPC_SYNCHRONIZER_H


struct urpc_synchronizer_t;

struct urpc_synchronizer_t *
urpc_syncronizer_create(
);

int
urpc_synchronizer_acquire(
    struct urpc_synchronizer_t *s
);

int
urpc_synchronizer_release(
    struct urpc_synchronizer_t *s
);

int
urpc_synchronizer_destroy(
    struct urpc_synchronizer_t *s
);

#endif //URPC_SYNCHRONIZER_H
