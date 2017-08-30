#pragma once
#include "Alert.h"

namespace libtorrent
{
	struct dht_immutable_item_alert;
}

namespace Ragnar
{
	ref class SHA1Hash;

	public ref class DhtImmutableItemAlert : Alert
	{
	private:
		libtorrent::dht_immutable_item_alert* alert;
		bool disposed;

	internal:
		DhtImmutableItemAlert(libtorrent::dht_immutable_item_alert* alert);

	public:
		property SHA1Hash^ Target  { SHA1Hash^ get(); }
		property array<unsigned char, 1>^ Entry { array<unsigned char, 1>^ get(); }
		~DhtImmutableItemAlert();
	};
}