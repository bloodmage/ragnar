#include "stdafx.h"
#include "DhtLookup.h"

#include <libtorrent\session_status.hpp>

namespace Ragnar
{
	DhtLookup::DhtLookup(const libtorrent::dht_lookup &status)
	{
		this->_dht_lookup = new libtorrent::dht_lookup(status);
	}

	DhtLookup::~DhtLookup()
	{
		delete this->_dht_lookup;
	}

	System::String^ DhtLookup::Type::get()
	{
		return gcnew System::String(this->_dht_lookup->type);
	}

	int DhtLookup::OutstandingRequests::get()
	{
		return this->_dht_lookup->outstanding_requests;
	}

	int DhtLookup::Timeouts::get()
	{
		return this->_dht_lookup->timeouts;
	}

	int DhtLookup::Responses::get()
	{
		return this->_dht_lookup->responses;
	}

	int DhtLookup::BranchFactor::get()
	{
		return this->_dht_lookup->branch_factor;
	}

	int DhtLookup::NodesLeft::get()
	{
		return this->_dht_lookup->nodes_left;
	}

	int DhtLookup::LastSent::get()
	{
		return this->_dht_lookup->last_sent;
	}

	int DhtLookup::FirstTimeout::get()
	{
		return this->_dht_lookup->first_timeout;
	}
}