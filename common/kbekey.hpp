/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KBENGINE_KEY_HPP
#define KBENGINE_KEY_HPP

#include "ace/pre.h"
#include <string>
#include "rsa.hpp"
#include "ace/Null_Mutex.h"
#include "ace/Singleton.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

//引擎的key管理
class KBEKey : public KBE_RSA
{
	private:
	friend class ACE_Singleton < KBEKey, ACE_Null_Mutex > ;

	public:
	typedef ACE_Singleton < KBEKey, ACE_Null_Mutex > Singlton;

	public:
	KBEKey(const std::string& pubkeyname, const std::string& prikeyname);
	KBEKey::KBEKey() :KBE_RSA() { }
	virtual ~KBEKey() { }
	virtual bool isGood()const;
};
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBENGINE_KEY_HPP
