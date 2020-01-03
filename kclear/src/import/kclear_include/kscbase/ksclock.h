// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/********************************************************************
	created:	2010/03/08
	created:	8:3:2010   9:43
	filename: 	ksclock.h
	author:		Jiang Fengbing
	
	purpose:	锁的封装
*********************************************************************/

#ifndef KSCLOCK_INC_
#define KSCLOCK_INC_

//////////////////////////////////////////////////////////////////////////

#include "kscbase.h"

//////////////////////////////////////////////////////////////////////////

class KLock 
{
public:
	KLock()
	{
		m_hMutex = CreateMutex(NULL, FALSE, NULL);
	}

	~KLock()
	{
		CloseHandle(m_hMutex);
	}

	void Acquire()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	void Release()
	{
		ReleaseMutex(m_hMutex);
	}

private:
	HANDLE m_hMutex;
};

class KAutoLock 
{
public:
	explicit KAutoLock(KLock& lockImpl) : m_lockImpl(lockImpl)
	{
		m_lockImpl.Acquire();
	}

	~KAutoLock()
	{
		m_lockImpl.Release();
	}

private:
	KLock& m_lockImpl;
};

//////////////////////////////////////////////////////////////////////////
//
// 使用方法: 把KLock作为类的成员, 在需要使用锁的时候在作用域内放置一个
// KAutoLock lock;即可
//
//////////////////////////////////////////////////////////////////////////

#endif	// KSCLOCK_INC_
