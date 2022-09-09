/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Florian Schmidt <florian.schmidt@neclab.eu>
 *
 * Copyright (c) 2017, NEC Europe Ltd., NEC Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <uk/alloc.h>
#include <sys/mman.h>
#include <uk/plat/paging.h>
#include <uk/plat/io.h>

/* Forward to libucallocator calls */
void *malloc(size_t size)
{
	return uk_malloc(uk_alloc_get_default(), size);
}

void *calloc(size_t nmemb, size_t size)
{
	return uk_calloc(uk_alloc_get_default(), nmemb, size);
}

void *realloc(void *ptr, size_t size)
{
	return uk_realloc(uk_alloc_get_default(), ptr, size);
}

int posix_memalign(void **memptr, size_t align, size_t size)
{
	return uk_posix_memalign(uk_alloc_get_default(),
				 memptr, align, size);
}

void *memalign(size_t align, size_t size)
{
	return uk_memalign(uk_alloc_get_default(), align, size);
}

void free(void *ptr)
{
	return uk_free(uk_alloc_get_default(), ptr);
}

int mprotect(void *addr __unused, size_t len __unused, int prot __unused)
{
	return 0;
}

void uk_mprotect_pthread_guard(void *addr, size_t len, int protect)
{
	int rc = 0;
	uintptr_t paddr;

	if (len != PAGE_SIZE)
		UK_CRASH("%s: unexpected size of the guard: 0x%lx", __func__, len);

	if ((uintptr_t)addr&(PAGE_SIZE-1))
		UK_CRASH("%s: guard is not page aligned: %p", __func__, addr);

	if (protect) {
		uk_pr_info("%s: unmap %p\n", __func__, addr);
		rc = ukplat_page_unmap(ukplat_pt_get_active(), (uintptr_t)addr, 1, PAGE_FLAG_KEEP_FRAMES);
		if (rc)
			UK_CRASH("%s: failed to unmap guard page at %p", __func__, addr);
	} else {
		uk_pr_info("%s: map %p\n", __func__, addr);
		/* TODO: boldly assume this is not the first page of the heap */
		paddr = ukplat_virt_to_phys(addr - 0x1000) + 0x1000;
		rc = ukplat_page_map(ukplat_pt_get_active(), (uintptr_t)addr,
				     paddr, 1, PAGE_ATTR_PROT_RW, 0);
		if (rc)
			UK_CRASH("%s: failed to (re)map guard page at %p", __func__, addr);
	}
}
