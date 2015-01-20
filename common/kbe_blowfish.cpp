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

#include "ace/Log_Msg.h"
#include "openssl/rand.h"
#include "kbe_blowfish.hpp"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
KBEBlowfish::KBEBlowfish(const Key & key) :
key_(key),
keySize_(key.size()),
isGood_(false),
pBlowFishKey_(NULL)
{
	if( init() )
	{
		ACE_DEBUG(( LM_DEBUG, "KBEBlowfish::KBEBlowfish(const Key & key): Using Blowfish key: %s",
			this->strBlowFishKey() ));
	}
}

//-------------------------------------------------------------------------------------
KBEBlowfish::KBEBlowfish(int keySize) :
key_(keySize, 0),
keySize_(keySize),
isGood_(false),
pBlowFishKey_(NULL)
{
	RAND_bytes(( ACE_Byte* )const_cast<char *>( key_.c_str() ), key_.size());
	if( this->init() )
	{
		ACE_DEBUG(( LM_DEBUG,
			"KBEBlowfish::KBEBlowfish(int keySize): Using Blowfish key: %s",
			this->strBlowFishKey() ));
	}
}

//-------------------------------------------------------------------------------------
KBEBlowfish::~KBEBlowfish()
{
	delete pBlowFishKey();
	pBlowFishKey_ = NULL;
}

//-------------------------------------------------------------------------------------
bool KBEBlowfish::init()
{
	pBlowFishKey_ = new BF_KEY;

	if( ( MIN_KEY_SIZE <= keySize_ ) && ( keySize_ <= MAX_KEY_SIZE ) )
	{
		BF_set_key(this->pBlowFishKey(), key_.size(), (ACE_Byte*) key_.c_str());
		isGood_ = true;
	} else
	{
		ACE_DEBUG(( LM_ERROR, "KBEBlowfish::init: invalid length %d", keySize_ ));
		isGood_ = false;
	}

	return isGood_;
}

//-------------------------------------------------------------------------------------
const char * KBEBlowfish::strBlowFishKey() const
{
	static char buf[1024];
	char *c = buf;

	for( int i = 0; i < keySize_; i++ )
	{
		kbe_snprintf(c, 1024, "%02hhX ", (unsigned char) key_[i]);
	}

	c[-1] = '\0';
	return buf;
}

int KBEBlowfish::encrypt(const ACE_Byte * src, ACE_Byte * dest, int length)
{
	// BLOCK_SIZEµÄÕûÊý±¶
	if( length % BLOCK_SIZE != 0 )
	{
		ACE_DEBUG(( LM_CRITICAL,
			"Blowfish::encrypt: Input length (%d) is not a multiple of block size (%d)\n",
			length, (int) ( BLOCK_SIZE ) ));
	}

	ACE_UINT64 * pPrevBlock = NULL;
	for( int i = 0; i < length; i += BLOCK_SIZE )
	{
		if( pPrevBlock )
		{
			*(ACE_UINT64*) ( dest + i ) = *(ACE_UINT64*) ( src + i ) ^ ( *pPrevBlock );
		} else
		{
			*(ACE_UINT64*) ( dest + i ) = *(ACE_UINT64*) ( src + i );
		}

		BF_ecb_encrypt(dest + i, dest + i, this->pBlowFishKey(), BF_ENCRYPT);
		pPrevBlock = (ACE_UINT64*) ( src + i );
	}

	return length;
}

//-------------------------------------------------------------------------------------
int KBEBlowfish::decrypt(const ACE_Byte * src, ACE_Byte * dest, int length)
{
	if( length % BLOCK_SIZE != 0 )
	{
		ACE_DEBUG((
			LM_WARNING,
			"Blowfish::decrypt:"
			"Input stream size (%d) is not a multiple of the block size (%d)\n",
			length,
			(int) ( BLOCK_SIZE ) ));
		return -1;
	}

	ACE_UINT64 * pPrevBlock = NULL;
	for( int i = 0; i < length; i += BLOCK_SIZE )
	{
		BF_ecb_encrypt(src + i, dest + i, this->pBlowFishKey(), BF_DECRYPT);

		if( pPrevBlock )
		{
			*(ACE_UINT64*) ( dest + i ) ^= *pPrevBlock;
		}

		pPrevBlock = (ACE_UINT64*) ( dest + i );
	}

	return length;
}


ACE_KBE_END_VERSIONED_NAMESPACE_DECL
