#include "Stdafx.h"
#include "DhtImmutableItemAlert.h"
#include "Utils.h"
#include "SHA1Hash.h"

#include "libtorrent\alert_types.hpp"
#include "libtorrent\bencode.hpp"
using namespace System::Runtime::InteropServices;

namespace Ragnar
{
	DhtImmutableItemAlert::DhtImmutableItemAlert(libtorrent::dht_immutable_item_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = new libtorrent::dht_immutable_item_alert(*alert);
	}


	DhtImmutableItemAlert::~DhtImmutableItemAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}

	SHA1Hash^ DhtImmutableItemAlert::Target::get()
	{
		return gcnew SHA1Hash(alert->target);
	}

	array<unsigned char, 1>^ DhtImmutableItemAlert::Entry::get()
	{
		return Utils::GetByteArrayFromLibtorrentEntry(alert->item);
	}
}