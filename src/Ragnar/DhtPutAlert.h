#pragma once
#include "Alert.h"

namespace libtorrent
{
	struct dht_put_alert;
}

namespace Ragnar
{
	ref class SHA1Hash;

	public ref class DhtPutAlert : Alert
	{
	private:
		libtorrent::dht_put_alert* alert;
		bool disposed;

	internal:
		DhtPutAlert(libtorrent::dht_put_alert* alert);

	public:
		property SHA1Hash^ Target  { SHA1Hash^ get(); }
		property array<unsigned char, 1>^ PublicKey  { array<unsigned char, 1>^ get(); }
		property array<unsigned char, 1>^ Signature { array<unsigned char, 1>^ get(); }
		property System::UInt64 Seq { System::UInt64 get(); }
		property array<unsigned char, 1>^ Salt { array<unsigned char, 1>^ get(); }

		~DhtPutAlert();
	};
}