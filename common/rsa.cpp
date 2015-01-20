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

#include "rsa.hpp"
#include "common.h"
#include "format.h"

#include <iostream>
#include <fstream>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
//-------------------------------------------------------------------------------------
KBE_RSA::KBE_RSA(const std::string& pubkeyname, const std::string& prikeyname) :
rsa_public(0),
rsa_private(0)
{
	if( pubkeyname.size() > 0 || prikeyname.size() > 0 )
	{
		ACE_ASSERT(pubkeyname.size() > 0);
		ACE_ASSERT(prikeyname.size() > 0);

		bool key = loadPrivate(prikeyname) && loadPublic(pubkeyname);
		ACE_ASSERT(key);
	}
}

//-------------------------------------------------------------------------------------
KBE_RSA::KBE_RSA() :
rsa_public(0),
rsa_private(0)
{
}

//-------------------------------------------------------------------------------------
KBE_RSA::~KBE_RSA()
{
	if( rsa_public != NULL )
	{
		RSA_free(static_cast<RSA*>( rsa_public ));
		rsa_public = NULL;
	}

	if( rsa_private != NULL )
	{
		RSA_free(static_cast<RSA*>( rsa_private ));
		rsa_private = NULL;
	}
}

//-------------------------------------------------------------------------------------
bool KBE_RSA::loadPublic(const std::string& keyname)
{
	FILE *fp = NULL;

	if( rsa_public == NULL )
	{
		fp = fopen(keyname.c_str(), "rb");
		if( !fp )
		{
			return false;
		}

		rsa_public = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
		if( NULL == rsa_public )
		{
			ERR_load_crypto_strings();
			char err[1024];
			char* errret = ERR_error_string(ERR_get_error(), err);
			fclose(fp);;
			ACE_ERROR_RETURN((
				LM_ERROR,
				"KBE_RSA::loadPublic: PEM_read_RSAPublicKey is error.(%s : %s)\n",
				errret, err ),
				false);
		}
	}

	if( fp )
		fclose(fp);
	return rsa_public != NULL;
}

//-------------------------------------------------------------------------------------
bool KBE_RSA::loadPrivate(const std::string& keyname)
{
	FILE *fp = NULL;

	if( rsa_private == NULL )
	{
		fp = fopen(keyname.c_str(), "rb");
		if( !fp )
		{
			return false;
		}

		rsa_private = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
		if( NULL == rsa_private )
		{
			ERR_load_crypto_strings();
			char err[1024];
			char* errret = ERR_error_string(ERR_get_error(), err);
			fclose(fp);
			ACE_ERROR_RETURN((
				LM_ERROR,
				"KBE_RSA::loadPrivate: PEM_read_RSAPrivateKey is error.(%s : %s)\n",
				errret, err ),
				false);
		}
	}

	if( fp )
		fclose(fp);
	return rsa_private != NULL;
}

//-------------------------------------------------------------------------------------
bool KBE_RSA::generateKey(const std::string& pubkeyname,
	const std::string& prikeyname, int keySize, int e)
{
	ACE_ASSERT(rsa_public == NULL && rsa_private == NULL);

	RSA* rsa = NULL;
	FILE *fp = NULL;

	if( ( rsa = RSA_generate_key(keySize, e, NULL, NULL) ) == NULL )
	{
		ERR_load_crypto_strings();
		char err[1024];
		char* errret = ERR_error_string(ERR_get_error(), err);
		ACE_ERROR_RETURN((
			LM_ERROR,
			"KBE_RSA::generateKey: RSA_generate_key is error.(%s : %s)\n",
			errret, err ),
			false);
	}

	if( !RSA_check_key(rsa) )
	{
		ACE_ERROR(( LM_ERROR, "KBE_RSA::generateKey: invalid RSA Key.\n" ));
		RSA_free(rsa);
		return false;
	}

	fp = fopen(prikeyname.c_str(), "w");
	if( !fp )
	{
		RSA_free(rsa);
		return false;
	}

	if( !PEM_write_RSAPrivateKey(fp, static_cast<RSA*>( rsa ), NULL, NULL, 0, 0, NULL) )
	{
		ERR_load_crypto_strings();
		char err[1024];
		char* errret = ERR_error_string(ERR_get_error(), err);
		fclose(fp);
		RSA_free(rsa);
		ACE_ERROR_RETURN((
			LM_ERROR,
			"KBE_RSA::generateKey: PEM_write_RSAPrivateKey is error.(%s : %s)\n",
			errret, err ),
			false);
	}

	fclose(fp);
	fp = fopen(pubkeyname.c_str(), "w");
	if( !fp )
	{
		RSA_free(rsa);
		return false;
	}

	if( !PEM_write_RSAPublicKey(fp, static_cast<RSA*>( rsa )) )
	{
		ERR_load_crypto_strings();
		char err[1024];
		char* errret = ERR_error_string(ERR_get_error(), err);
		fclose(fp);
		RSA_free(rsa);
		ACE_ERROR_RETURN((
			LM_ERROR,
			"KBE_RSA::generateKey: PEM_write_RSAPublicKey is error.(%s : %s)\n",
			errret, err ),
			false);
	}

	RSA_free(rsa);
	fclose(fp);
	ACE_ERROR_RETURN((
		LM_ERROR,
		"KBE_RSA::generateKey: RSA key generated. keysize(%d) bits.\n", keySize ),
		loadPrivate(prikeyname) && loadPublic(pubkeyname));
}

//-------------------------------------------------------------------------------------
std::string KBE_RSA::encrypt(const std::string& instr)
{
	std::string encrypted;
	if( encrypt(instr, encrypted) < 0 )
		return "";

	char strencrypted[1024];
	memset(strencrypted, 0, 1024);
	bytes2string((unsigned char *) encrypted.data(), encrypted.size(), (unsigned char *) strencrypted, 1024);
	return strencrypted;
}

//-------------------------------------------------------------------------------------
int KBE_RSA::encrypt(const std::string& instr, std::string& outCertifdata)
{
	ACE_ASSERT(rsa_public != NULL);

	unsigned char* certifdata = (unsigned char*) calloc(RSA_size(static_cast<RSA*>( rsa_public )) + 1, sizeof(unsigned char));

	int certifsize = RSA_public_encrypt(instr.size(),
		(unsigned char*) instr.c_str(), certifdata, static_cast<RSA*>( rsa_public ), RSA_PKCS1_OAEP_PADDING);

	if( certifsize < 0 )
	{
		ERR_load_crypto_strings();
		char err[1024];
		char* errret = ERR_error_string(ERR_get_error(), err);
		free(certifdata);
		ACE_ERROR_RETURN((
			LM_ERROR,
			"KBE_RSA::encrypt: RSA_public_encrypt is error.(%s : %s)\n",
			errret, err ),
			certifsize);
	}

	outCertifdata.assign((const char*) certifdata, certifsize);
	free(certifdata);
	return certifsize;
}

//-------------------------------------------------------------------------------------
void KBE_RSA::hexCertifData(const std::string& inCertifdata)
{
	ACE_HEX_DUMP((
		LM_DEBUG,
		const_cast<char*>( inCertifdata.data() ),
		inCertifdata.size(),
		"KBE_RSA::encrypt: encrypted string:" ));
}

//-------------------------------------------------------------------------------------
int KBE_RSA::decrypt(const std::string& inCertifdata, std::string& outstr)
{
	ACE_ASSERT(rsa_private != NULL);

	int rsa_len = RSA_size(static_cast<RSA*>( rsa_private ));
	unsigned char* keydata = (unsigned char*) calloc(rsa_len + 1, sizeof(unsigned char));

	int keysize = RSA_private_decrypt(rsa_len,
		(const unsigned char *) inCertifdata.data(), keydata, static_cast<RSA*>( rsa_private ), RSA_PKCS1_OAEP_PADDING);

	if( keysize < 0 )
	{
		ERR_load_crypto_strings();
		char err[1024];
		char* errret = ERR_error_string(ERR_get_error(), err);
		free(keydata);
		ACE_ERROR_RETURN((
			LM_ERROR,
			"KBE_RSA::decrypt: RSA_private_decrypt is error.(%s : %s)\n",
			errret, err ),
			keysize);
	}

	outstr.assign((const char*) keydata, keysize);
	free(keydata);
	return keysize;
}

//-------------------------------------------------------------------------------------
std::string KBE_RSA::decrypt(const std::string& instr)
{
	unsigned char strencrypted[1024];
	memset(strencrypted, 0, 1024);
	string2bytes((unsigned char *) instr.data(), (unsigned char *) &strencrypted[0], 1024);
	std::string encrypted;
	encrypted.assign((char*) strencrypted, 1024);

	std::string out;
	if( decrypt(encrypted, out) < 0 )
		return "";

	return out;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
