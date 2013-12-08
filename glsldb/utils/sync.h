/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef _SYNC_H
#define _SYNC_H
#if (_MSC_VER > 1000)
#pragma once
#endif /* (_MSC_VER > 1000) */

#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
#include <pthread.h>
#include <semaphore.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A mutex lock for synchronising threads. */
#ifdef _WIN32
typedef CRITICAL_SECTION ThdLock;
#else /* _WIN32 */
typedef pthread_mutex_t ThdLock;
#endif /* _WIN32 */

/**
 * Create a new mutex lock.
 *
 * @param lock Pointer to the synchronisation object to initialise.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
#ifdef _WIN32
#define createThdLock(lock) (InitializeCriticalSection(lock), 0)
#else /* _WIN32 */
#define createThdLock(lock) pthread_mutex_init(lock, NULL)
#endif /* _WIN32 */

/**
 * Acquire 'lock'.
 *
 * @param lock Pointer to the synchronsiation object.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
#ifdef _WIN32
#define acquireThdLock(lock) (EnterCriticalSection(lock), 0)
#else /* _WIN32 */
#define acquireThdLock(lock) pthread_mutex_lock(lock)
#endif /* _WIN32 */

/**
 * Release 'lock'.
 *
 * @param lock Pointer to the synchronisation object.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
#ifdef _WIN32
#define releaseThdLock(lock) (LeaveCriticalSection(lock), 0)
#else /* _WIN32 */
#define releaseThdLock(lock) pthread_mutex_unlock(lock)
#endif /* _WIN32 */

/**
 * Delete 'lock'. 'lock' can only be used again after recreating it.
 *
 * @param lock Pointer to the synchronisation object.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
#ifdef _WIN32
#define deleteThdLock(lock) (DeleteCriticalSection(lock), 0)
#else /* _WIN32 */
#define deleteThdLock(lock) pthread_mutex_destroy(lock)
#endif /* _WIN32 */

/** An event object for interprocess synchronisation. */
#ifdef _WIN32
typedef HANDLE IpcEvent;
#else /* _WIN32 */
typedef sem_t *IpcEvent;
#endif /* _WIN32 */

/*
 * Use this constant for waiting infinitely for an event to enter signaled
 * state.
 */
#ifdef _WIN32
#define TIMEOUT_INFINITE (INFINITE)
#else /* _WIN32 */
#define TIMEOUT_INFINITE (-1)
#endif /* _WIN32 */

/**
 * Create an event object for synchronising processes.
 *
 * @param evt            Receives the event object if created successfully.
 * @param reserved       Reserved. Must be 0.
 * @param isInitiallySet If TRUE, the event is initially set.
 * @param isCreateOnly   If TRUE, the operation failed if an event object with
 *                       the same name already exists. Otherwise, this object is
 *                       opened.
 * @param name           The name of the event. This name must not include
 *                       Windows kernel namespaces or a POSIX leading '/'.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
int createIpcEvent(IpcEvent *evt, const int reserved, const int isInitiallySet,
		const int isCreateOnly, const char *name);

/**
 * Signal the event.
 *
 * Only a single waiting thread is released if the event enters signaled state.
 *
 * @param evt The event to be signaled.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
int setIpcEvent(IpcEvent evt);

/**
 * Wait for the event to enter signaled state.
 *
 * @param evt The event to wait for.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
int waitIpcEvent(IpcEvent evt, int timeout);

/**
 * Delete an event.
 *
 * @param evt The event to be deleted.
 *
 * @return 0 in case of success, a system error code otherwise.
 */
int deleteIpcEvent(IpcEvent evt);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SYNC_H */
