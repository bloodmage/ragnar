#include "Stdafx.h"
#include "DhtPutAlert.h"
#include "Utils.h"
#include "SHA1Hash.h"

#include "libtorrent\alert_types.hpp"
using namespace System::Runtime::InteropServices;

namespace Ragnar
{
	DhtPutAlert::DhtPutAlert(libtorrent::dht_put_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = new libtorrent::dht_put_alert(*alert);
	}


	DhtPutAlert::~DhtPutAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}

	array<unsigned char, 1>^ DhtPutAlert::PublicKey::get()
	{
		auto ret = gcnew array<unsigned char, 1>(alert->public_key.size());
		Marshal::Copy((IntPtr)alert->public_key.begin(), ret, 0, alert->public_key.size());
		return ret;
	}
	array<unsigned char, 1>^ DhtPutAlert::Signature::get()
	{
		auto ret = gcnew array<unsigned char, 1>(alert->signature.size());
		Marshal::Copy((IntPtr)alert->signature.begin(), ret, 0, alert->signature.size());
		return ret;
	}
	System::UInt64 DhtPutAlert::Seq::get()
	{
		return this->alert->seq;
	}
	array<unsigned char, 1>^ DhtPutAlert::Salt::get()
	{
		return Utils::GetManagedBinaryFromStandardString(alert->salt);
	}
	SHA1Hash^ DhtPutAlert::Target::get()
	{
		return gcnew SHA1Hash(alert->target);
	}
}