#include "Stdafx.h"
#include "DhtDynamicItemPutter.h"
#include "Session.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <libtorrent\kademlia\item.hpp>
#include <libtorrent\session.hpp>
#include <libtorrent\bencode.hpp>
#include <libtorrent\alloca.hpp>
#include <msclr\lock.h>
#include <vcclr.h>
using namespace System::Runtime::InteropServices;
namespace Ragnar
{
	bool arrayequal(array<byte, 1>^ a, array<byte, 1>^ b)
	{
		if (a == nullptr && b == nullptr) return true;
		if (a == nullptr || b == nullptr) return false;
		if (a->Length != b->Length) return false;
		for (int i = 0; i < a->Length; i++)
			if (a[i] != b[i]) return false;
		return true;
	}

	class _dhtputterhelper
	{
	public:
		gcroot<DhtDynamicPutter^> handle;
		_dhtputterhelper(DhtDynamicPutter^ putter) :handle(putter) { }

		void putterfunc(boost::array<char,32> key, libtorrent::entry& entry, boost::array<char, 64>& sign, boost::uint64_t& seq, std::string const& salt)
		{
			array<byte, 1>^ key_ = gcnew array<byte, 1>(32);
			Marshal::Copy((System::IntPtr)key.begin(), key_, 0, 32);

			array<byte, 1>^ salt_ = Utils::GetManagedBinaryFromStandardString(salt);

			PutterDelegate^ delegate_ = delegateLookup(key_, salt_);
			if (delegate_ == nullptr) return;
			
			std::string flatentry;
			libtorrent::bencode(std::back_inserter(flatentry), entry);
			array<byte, 1>^ entry_ = Utils::GetManagedBinaryFromStandardString(flatentry);

			array<byte, 1>^ sign_ = gcnew array<byte, 1>(64);
			Marshal::Copy((System::IntPtr)sign.begin(), sign_, 0, 64);

			System::UInt64 localseq = seq;
			if (delegate_(key_, entry_, sign_, localseq, salt_))
			{
				//Copy back
				libtorrent::entry newentry;
				if (entry_ != nullptr)
				{
					pin_ptr<byte> buff = &entry_[0];
					newentry = libtorrent::bdecode((byte*)buff, (byte*)buff + entry_->Length);
				    entry = newentry;
				}

				auto entrystr = entry.to_string();
				System::Console::WriteLine("newentry decoded: {0}", Utils::GetManagedStringFromStandardString(entrystr));

				seq = localseq;
				Marshal::Copy(sign_, 0, (IntPtr)sign.begin(), 64);
			}

			handle->UnregisterPutCallback(delegate_);

		}

		void putcallback(array<byte, 1>^ key, array<byte, 1>^ salt)
		{
			boost::array<char, 32U> key_;
			Marshal::Copy(key, 0, (System::IntPtr)key_.begin(), 32);
			std::string salt_ = "";
			if (salt != nullptr && salt->Length > 0)
			{
				pin_ptr<byte> abuf = &salt[0];
				salt_ = std::string((char*)abuf, (char*)abuf + salt->Length);
			}
			boost::function<void(libtorrent::entry&, boost::array<char, 64>&, boost::uint64_t&, std::string const&)> func;
			func = boost::bind(&_dhtputterhelper::putterfunc, this, key_, _1, _2, _3, _4);
			handle->host->_session->dht_put_item(key_, func, salt_);
		}

		PutterDelegate^ delegateLookup(array<byte, 1>^ key, array<byte, 1>^ salt)
		{
			DhtDynamicPutter^ putter = handle;
			if (putter == nullptr) return nullptr;

			auto delegates = putter->delegates;
			if (delegates == nullptr) return nullptr;
			msclr::lock l(delegates);

			for (int i = 0; i < delegates->Count; i++)
				if (arrayequal((*delegates)[i]->Item1, key) && arrayequal((*delegates)[i]->Item2, salt))
					return (*delegates)[i]->Item3;
			return nullptr;
		}
	};


	DhtDynamicPutter::DhtDynamicPutter(Session^ sess)
	{
		host = sess;
		delegates = gcnew System::Collections::Generic::List<System::Tuple<array<byte, 1>^, array<byte, 1>^, PutterDelegate^>^>();
		helper = new _dhtputterhelper(this);
	}

	void DhtDynamicPutter::Unload()
	{
		host = nullptr;
		if (delegates != nullptr)
			delegates->Clear();
		if (helper!=NULL)
			delete helper->handle;
		delegates = nullptr;
	}
	
	DhtDynamicPutter::!DhtDynamicPutter()
	{
		delete helper;
		helper = NULL;
	}

	void DhtDynamicPutter::RegisterPutCallback(array<byte, 1>^ key, PutterDelegate^ callback, array<byte, 1>^ salt)
	{
		if (key == nullptr || key->Length != 32) throw gcnew System::InvalidOperationException("Key must be array of length 32");
		msclr::lock l(delegates);

		auto datatuple = gcnew System::Tuple<array<byte, 1>^, array<byte, 1>^, PutterDelegate^>(key, salt, callback);

		UnregisterPutCallback(callback);
		UnregisterPutCallback(key, salt);
		delegates->Add(datatuple);
		
		helper->putcallback(key, salt);
	}
	void DhtDynamicPutter::UnregisterPutCallback(array<byte, 1>^ key, array<byte, 1>^ salt)
	{
		if (key == nullptr || key->Length != 32) throw gcnew System::InvalidOperationException("Key must be array of length 32");

		msclr::lock l(delegates);
		for (int i = 0; i < delegates->Count;i++)
			if (arrayequal((*delegates)[i]->Item1, key) && arrayequal((*delegates)[i]->Item2, salt))
			{
				delegates->RemoveAt(i);
				return;
			}
	}

	void DhtDynamicPutter::UnregisterPutCallback(PutterDelegate^ callback)
	{
		msclr::lock l(delegates);
		for (int i = 0; i < delegates->Count; i++)
			if (callback == (*delegates)[i]->Item3)
			{
				delegates->RemoveAt(i);
				return;
			}
	}

	void DhtDynamicPutter::SignMutableItem(array<byte, 1>^ sig, array<byte, 1>^ v, array<byte, 1>^ salt, System::UInt64 seq, array<byte, 1>^ pk, array<byte, 1>^ sk)
	{
		if (sig->Length != 64) throw gcnew InvalidOperationException("sig must be a 64 byte buffer");
		if (v == nullptr) throw gcnew NullReferenceException("value must not be null");
		if (salt == nullptr) salt = gcnew array<byte>(0);
		if (pk->Length != 32) throw gcnew InvalidOperationException("pk must be of length 32");
		if (sk->Length != 64) throw gcnew InvalidOperationException("sk must be of length 64");
		pin_ptr<byte> v_ = &v[0];
		byte* saltref = NULL;
		pin_ptr<byte> salt_;
		if (salt->Length != 0)
		{
			salt_ = &salt[0];
			saltref = salt_;
		}
		__int64 seq_ = seq;
		pin_ptr<byte> pk_ = &pk[0];
		pin_ptr<byte> sk_ = &sk[0];
		pin_ptr<byte> sig_ = &sig[0];

		libtorrent::dht::sign_mutable_item(std::make_pair((char*)v_, v->Length), std::make_pair((char*)saltref, salt->Length), seq_, (char*)pk_, (char*)sk_, (char*)sig_);
	}

	bool DhtDynamicPutter::VerifyMutableItem(array<byte, 1>^ sig, array<byte, 1>^ v, array<byte, 1>^ salt, System::UInt64 seq, array<byte, 1>^ pk)
	{
		if (sig->Length != 64) throw gcnew InvalidOperationException("sig must be a 64 byte buffer");
		if (v == nullptr) throw gcnew NullReferenceException("value must not be null");
		if (salt == nullptr) salt = gcnew array<byte>(0);
		if (pk->Length != 32) throw gcnew InvalidOperationException("pk must be of length 32");
		pin_ptr<byte> v_ = &v[0];
		byte* saltref = NULL;
		pin_ptr<byte> salt_;
		if (salt->Length != 0)
		{
			salt_ = &salt[0];
			saltref = salt_;
		}
		__int64 seq_ = seq;
		pin_ptr<byte> pk_ = &pk[0];
		pin_ptr<byte> sig_ = &sig[0];

		return libtorrent::dht::verify_mutable_item(std::make_pair((char*)v_, v->Length), std::make_pair((char*)saltref, salt->Length), seq_, (char*)pk_, (char*)sig_);
	}
}