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

#include "kbekey.hpp"
#include "common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

KBEKey::KBEKey(const std::string& pubkeyname, const std::string& prikeyname) : KBE_RSA()
{
	if( pubkeyname.size() > 0 || prikeyname.size() > 0 )
	{
		ACE_ASSERT(pubkeyname.size() > 0);

		if( g_componentType != CLIENT )
		{
			ACE_ASSERT(prikeyname.size() > 0);

			bool key1 = KBE_RSA::loadPrivate(prikeyname);
			bool key2 = KBE_RSA::loadPublic(pubkeyname);
			ACE_ASSERT(key1 == key2);

			if( !key1 && !key2 )
			{
				bool ret = generateKey(pubkeyname, prikeyname);
				ACE_ASSERT(ret);
				key1 = loadPrivate(prikeyname);
				key2 = loadPublic(pubkeyname);
				ACE_ASSERT(key1 && key2);
			}
		} else
		{
			bool key = loadPublic(pubkeyname);
			ACE_ASSERT(key);
		}
	}
}

//-------------------------------------------------------------------------------------
bool KBEKey::isGood()const
{
	if( g_componentType == CLIENT )
	{
		return rsa_public != NULL;
	}

	return rsa_public != NULL && rsa_private != NULL;
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
