#ifndef Sync_Lock_H
#define Sync_Lock_H

#include <pthread.h>

typedef pthread_spinlock_t Lock;

static void Lock_init(Lock* l){
	pthread_spin_init(l,PTHREAD_PROCESS_PRIVATE);
}
static void Lock_destroy(Lock* l){
	pthread_spin_destroy(l);
}
static void Lock_lock(Lock* l){
	pthread_spin_lock(l);
}
static void Lock_unlock(Lock* l){
	pthread_spin_unlock(l);
}

#endif

