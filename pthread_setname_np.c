#define _GNU_SOURCE
#include <string.h>

#include "pthread_impl.h"

#include <uk/thread.h>

struct uk_thread *tid2ukthread(pid_t tid);

int pthread_setname_np(pthread_t thread, const char *name)
{
	size_t len;
	struct uk_thread *ukth;

	if ((len = strnlen(name, 16)) > 15) return ERANGE;

	if (!(ukth = tid2ukthread(thread->tid)))
		return -1;

	ukth->name = name;

	return 0;
}
