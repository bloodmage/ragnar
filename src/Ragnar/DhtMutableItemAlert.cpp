#include "Stdafx.h"
#include "DhtMutableItemAlert.h"
#include "Utils.h"

#include "libtorrent\alert_types.hpp"
#include "libtorrent\bencode.hpp"
using namespace System::Runtime::InteropServices;

namespace Ragnar
{
	DhtMutableItemAlert::DhtMutableItemAlert(libtorrent::dht_mutable_item_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = new libtorrent::dht_mutable_item_alert(*alert);
	}


	DhtMutableItemAlert::~DhtMutableItemAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}

	array<unsigned char, 1>^ DhtMutableItemAlert::Key::get()
	{
		auto ret = gcnew array<unsigned char, 1>(alert->key.size());
		Marshal::Copy((IntPtr)alert->key.begin(), ret, 0, alert->key.size());
		return ret;
	}
	array<unsigned char, 1>^ DhtMutableItemAlert::Signature::get()
	{
		auto ret = gcnew array<unsigned char, 1>(alert->signature.size());
		Marshal::Copy((IntPtr)alert->signature.begin(), ret, 0, alert->signature.size());
		return ret;
	}
	System::UInt64 DhtMutableItemAlert::Seq::get()
	{
		return this->alert->seq;
	}
	array<unsigned char, 1>^ DhtMutableItemAlert::Salt::get()
	{
		return Utils::GetManagedBinaryFromStandardString(alert->salt);
	}
	array<unsigned char, 1>^ DhtMutableItemAlert::Entry::get()
	{
		return Utils::GetByteArrayFromLibtorrentEntry(alert->item);
	}
}