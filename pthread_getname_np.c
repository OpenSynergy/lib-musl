#define _GNU_SOURCE
#include <string.h>

#include "pthread_impl.h"

#include <uk/thread.h>

int pthread_getname_np(pthread_t thread, char *name, size_t len)
{
	if (thread != pthread_self())
		return ENOTSUP;

	strncpy(name, uk_thread_current()->name, len);

	return 0;
}
