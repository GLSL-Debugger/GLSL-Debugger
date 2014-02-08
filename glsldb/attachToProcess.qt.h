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

#ifndef ATTACHTOPROCESS_QT_H_INCLUDED
#define ATTACHTOPROCESS_QT_H_INCLUDED
#if (_MSC_VER > 1000)
#pragma once
#endif /* (_MSC_VER > 1000) */

#ifdef _WIN32
#include <windows.h>
#include <Tlhelp32.h>
#else /* _WIN32 */
#include <unistd.h>
#endif /* _WIN32 */

#include <QtCore/QAbstractListModel>
#include <QtCore/QString>

/**
 * This list model represents a snapshot of all running processes at the point
 * in time it was created. The process list can be updated using Update().
 */
class ProcessSnapshotModel: public QAbstractListModel {
Q_OBJECT
	;

public:

#ifdef _WIN32
	typedef DWORD PID;
#else /* _WIN32 */
	typedef pid_t PID;
#endif /* _WIN32 */

	/**
	 * The ProcessSnapshotModel::Items represent a single process currently
	 * running on the system.
	 */
	class Item {

	public:

		~Item(void);

		inline const QString& GetExe(void) const
		{
			return this->exe;
		}

		inline const QString& GetOwner(void) const
		{
			return this->owner;
		}

		inline PID GetPid(void) const
		{
			return this->pid;
		}

		inline bool IsAttachtable(void) const
		{
			return this->isAttachable;
		}

	private:

#ifdef _WIN32
		QString toQString(const wchar_t *str);
#endif /* _WIN32 */

		Item(const char *exe, const char *owner, PID pid, bool isAttachtable =
				true, Item *parent = NULL);

#ifdef _WIN32
		Item(PROCESSENTRY32& pe, Item *parent = NULL);
#endif /* _WIN32 */

		/** Next process in linked list. */
		Item *child;

		/** Previous process in linked list. */
		Item *parent;

		/** Name of executable image. */
		QString exe;

		/** Name of process owner. */
		QString owner;

		/** Determines whether the user can attach to the process. */
		bool isAttachable;

		/** Process ID. */
		PID pid;

		friend class ProcessSnapshotModel;
	}; /* end class Item */

	/** Create a new model with a new process snapshot. */
	ProcessSnapshotModel(QObject *parent = NULL);

	/** Dtor. */
	virtual ~ProcessSnapshotModel(void);

	/** Update the process model with a new snapshot. */
	bool Update(void);

	/*
	 * Implementation of Qt pure virtual methods:
	 */

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex& index,
			int role = Qt::DisplayRole) const;

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;

	virtual QModelIndex index(int row, int column, const QModelIndex& parent =
			QModelIndex()) const;

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:

	/** Superclass typedef. */
	typedef QAbstractListModel Super;

	/** Root of the process list. */
	Item *root;

};
/* end class ProcessSnapshotModel */

#ifdef _WIN32

typedef struct ATTACHMENT_INFORMATION_t {
	HANDLE hProcess;    // Process handle of the process we attached to.
	HMODULE hLibrary;// Handle to injected debug library.
}ATTACHMENT_INFORMATION;

/**
 * Attach to the remote process with process ID 'pid' and inject the debug
 * library 'libPath'. In the target process, the environment variable
 * "GLSL_DEBUGGER_SHMID" is set to 'smName' and the variable
 * "GLSL_DEBUGGER_DBGFCTNS_PATH" is set to 'dbgFuncPath'.
 *
 * Before attaching 'libPath', the detours marker DLL as returned
 * by ::DetourGetDetouredMarker() is attached. It is guaranteed that the
 * environment variables have been set before 'libPath' is attached.
 *
 * The handles returned to 'outAi' are required to detach from the process.
 * The caller should not modify or close them.
 *
 * 'desiredAccess' is the access that the returned handle should have to the
 * remote process. The function ensures that the handle has at least the
 * access rights required for doing the work of this function, which might
 * not be enough for the things the caller wants to do.
 */
bool AttachToProcess(ATTACHMENT_INFORMATION& outAi, DWORD pid,
		DWORD desiredAccess, const char *libPath, const char *smName,
		const char *dbgFuncPath);

/**
 * Detach from the process designated by 'attachInfo'. If 'attachInfo' does not
 * hold the required handles, the function fails.
 */
bool DetachFromProcess(ATTACHMENT_INFORMATION& inOutAi);
#else /* _WIN32 */
#endif /* _WIN32 */

#endif /* ATTACHTOPROCESS_QT_H_INCLUDED */
