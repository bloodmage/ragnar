#include "Stdafx.h"
#include "ED25519Helper.h"
#include "Utils.h"

#include "libtorrent\ed25519.hpp"
#include "libtorrent\alloca.hpp"
using namespace System::Runtime::InteropServices;
namespace Ragnar
{
	array<unsigned char, 1>^ ED25519Helper::CreateSeed()
	{
		unsigned char seed[ed25519_seed_size];
		ed25519_create_seed(seed);
		auto ret = gcnew array<unsigned char, 1>(ed25519_seed_size);
		Marshal::Copy((System::IntPtr)seed, ret, 0, ed25519_seed_size);
		return ret;
	}
	System::Tuple<array<unsigned char, 1>^, array<unsigned char, 1>^>^ ED25519Helper::CreateKeypair(array<unsigned char, 1>^ seed)
	{
		if (seed->Length != ed25519_seed_size) throw gcnew System::InvalidOperationException("Seed must be of length 32");
		unsigned char pubkey[ed25519_public_key_size];
		unsigned char privkey[ed25519_private_key_size];

		pin_ptr<unsigned char> seed_ = &seed[0];
		ed25519_create_keypair(pubkey, privkey, seed_);

		auto pubkey_ = gcnew array<unsigned char, 1>(ed25519_public_key_size);
		Marshal::Copy((System::IntPtr)pubkey, pubkey_, 0, ed25519_public_key_size);
		auto privkey_ = gcnew array<unsigned char, 1>(ed25519_private_key_size);
		Marshal::Copy((System::IntPtr)privkey, privkey_, 0, ed25519_private_key_size);
		

		return gcnew System::Tuple<array<unsigned char, 1>^, array<unsigned char, 1>^>(pubkey_, privkey_);
	}

	void ED25519Helper::Sign(array<byte, 1>^ signature, array<byte, 1>^ message, array<byte, 1>^ pubkey, array<byte, 1>^ privkey)
	{
		if (signature->Length != ed25519_signature_size) throw gcnew System::InvalidOperationException("Signature buffer must have length 64");
		if (pubkey->Length != ed25519_public_key_size) throw gcnew System::InvalidOperationException("Public key must have length 32");
		if (privkey->Length != ed25519_private_key_size) throw gcnew System::IndexOutOfRangeException("Private key must have length 64");

		if (message == nullptr) throw gcnew System::ArgumentNullException("Message must not be null");

		pin_ptr<byte> sign_ = &signature[0];
		pin_ptr<byte> message_ = &message[0];
		pin_ptr<byte> pubkey_ = &pubkey[0];
		pin_ptr<byte> privkey_ = &privkey[0];

		ed25519_sign(sign_, message_, message->Length, pubkey_, privkey_);
	}

	int ED25519Helper::Verify(const unsigned char* signature, const unsigned char* message, size_t message_len, const unsigned char* public_key)
	{
		return ed25519_verify(signature, message, message_len, public_key);
	}

	System::String^ ED25519Helper::ToHexString(array<unsigned char, 1>^ bytes)
	{
		pin_ptr<byte> bytes_ = &bytes[0];
		byte*out = TORRENT_ALLOCA(byte, bytes->Length * 2 + 1);
		libtorrent::to_hex((const char*)bytes_, bytes->Length, (char*)out);
		return gcnew System::String((char*)out, 0, bytes->Length * 2);
	}

	array<unsigned char, 1>^ ED25519Helper::FromHexString(System::String^ hexdata)
	{
		if (hexdata->Length % 2 != 0) return nullptr;
		auto hexstr = Utils::GetStdStringFromManagedString(hexdata);
		char*binbuf = TORRENT_ALLOCA(char, (hexdata->Length+1) / 2);
		if (!libtorrent::from_hex(&hexstr[0], hexdata->Length, binbuf)) return nullptr;
		auto ret = gcnew array<byte, 1>(hexdata->Length / 2);
		Marshal::Copy((IntPtr)binbuf, ret, 0, hexdata->Length / 2);
		return ret;
	}
}