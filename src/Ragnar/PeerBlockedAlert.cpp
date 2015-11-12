#include "stdafx.h"
#include "PeerBlockedAlert.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	PeerBlockedAlert::PeerBlockedAlert(libtorrent::peer_blocked_alert* alert)
		: TorrentAlert((libtorrent::torrent_alert*) alert)
	{
		this->alert = alert;
	}
	PeerBlockedAlert::~PeerBlockedAlert()
	{
		if (this->disposed)
		{
			return;
		}
		this->disposed = true;
		delete this->alert;
	}

	System::Net::IPAddress^ PeerBlockedAlert::Address::get()
	{
		return System::Net::IPAddress::Parse(gcnew System::String(this->alert->ip.to_string().c_str()));
	}
}
