#include "stdafx.h"
#include "DhtRoutingBucket.h"

#include <libtorrent\session_status.hpp>

namespace Ragnar
{
	DhtRoutingBucket::DhtRoutingBucket(const libtorrent::dht_routing_bucket &dht_routing_bucket)
	{
		this->_dht_routing_bucket = new libtorrent::dht_routing_bucket(dht_routing_bucket);
	}

	DhtRoutingBucket::~DhtRoutingBucket()
	{
		delete this->_dht_routing_bucket;
	}

	int DhtRoutingBucket::NumNodes::get()
	{
		return this->_dht_routing_bucket->num_nodes;
	}

	int DhtRoutingBucket::NumReplacements::get()
	{
		return this->_dht_routing_bucket->num_replacements;
	}

	int DhtRoutingBucket::LastActive::get()
	{
		return this->_dht_routing_bucket->last_active;
	}
}