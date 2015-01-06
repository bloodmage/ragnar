#pragma once

namespace libtorrent
{
	struct dht_routing_bucket;
}

namespace Ragnar
{
	public ref class DhtRoutingBucket
	{
	private:
		libtorrent::dht_routing_bucket* _dht_routing_bucket;
	internal:
		DhtRoutingBucket(const libtorrent::dht_routing_bucket &dht_routing_bucket);

	public:
		~DhtRoutingBucket();
		property int NumNodes { int get(); }
		property int NumReplacements { int get(); }
		property int LastActive { int get(); }
	};
}