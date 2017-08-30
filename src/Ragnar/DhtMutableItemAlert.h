#pragma once
#include "Alert.h"

namespace libtorrent
{
	struct dht_mutable_item_alert;
}

namespace Ragnar
{
	public ref class DhtMutableItemAlert : Alert
	{
	private:
		libtorrent::dht_mutable_item_alert* alert;
		bool disposed;

	internal:
		DhtMutableItemAlert(libtorrent::dht_mutable_item_alert* alert);

	public:
		property array<unsigned char, 1>^ Key  { array<unsigned char, 1>^ get(); }
		property array<unsigned char, 1>^ Signature { array<unsigned char, 1>^ get(); }
		property System::UInt64 Seq { System::UInt64 get(); }
		property array<unsigned char, 1>^ Salt { array<unsigned char, 1>^ get(); }
		property array<unsigned char, 1>^ Entry { array<unsigned char, 1>^ get(); }
		~DhtMutableItemAlert();
	};
}