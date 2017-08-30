#include "stdafx.h"
#include "SHA1Hash.h"
#include "Utils.h"
#include <libtorrent\sha1_hash.hpp>

namespace Ragnar
{
    SHA1Hash::SHA1Hash(const libtorrent::sha1_hash &hash)
    {
        this->_hash = new libtorrent::sha1_hash(hash);
    }

	SHA1Hash::SHA1Hash(String^ lowcase_hex)
	{
		this->_hash = new libtorrent::sha1_hash(Utils::GetSha1HashFromString(lowcase_hex));
	}

	SHA1Hash::SHA1Hash(array<byte>^ data)
	{
		if (data->Length != 20) throw gcnew ArgumentException("Data length must be 20");
		pin_ptr<byte> d_ = &data[0];
		auto o(std::string((char*)d_, 20));
		this->_hash = new libtorrent::sha1_hash(o);
	}

	SHA1Hash::~SHA1Hash()
	{
		if (this->_hash != NULL)
		{
			delete this->_hash;
			this->_hash = NULL;
		}
	}
	SHA1Hash::!SHA1Hash()
	{
		this->~SHA1Hash();
	}
    bool SHA1Hash::IsZero::get()
    {
        return this->_hash->is_all_zeros();
    }

    void SHA1Hash::Clear()
    {
        this->_hash->clear();
    }

	System::String^ SHA1Hash::ToHex()
	{
		if (hex == nullptr)
		{
			auto o(libtorrent::to_hex(this->_hash->to_string()));
			hex = gcnew System::String(o.c_str());
		}
		return hex;
	}

	const byte* SHA1Hash::Ptr()
	{
		return &((*this->_hash)[0]);
	}

	System::String^ SHA1Hash::ToString()
	{
		if (str == nullptr)
		{
			auto o(this->_hash->to_string());
			str = gcnew System::String(o.c_str());
		}
		return str;
	}
}