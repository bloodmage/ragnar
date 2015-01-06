#pragma once

namespace libtorrent
{
	struct dht_lookup;
}

namespace Ragnar
{
	public ref class DhtLookup
	{
	private:
		libtorrent::dht_lookup* _dht_lookup;
	internal:
		DhtLookup(const libtorrent::dht_lookup &dht_lookup);

	public:
		~DhtLookup();
		property System::String^ Type { System::String^ get(); }
		property int OutstandingRequests { int get(); }
		property int Timeouts { int get(); }
		property int Responses { int get(); }
		property int BranchFactor { int get(); }
		property int NodesLeft { int get(); }
		property int LastSent { int get(); }
		property int FirstTimeout { int get(); }
	};
}