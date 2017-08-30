#include "stdafx.h"
#include "PeerBlockedAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	PeerBlockedAlert::PeerBlockedAlert(libtorrent::peer_blocked_alert* alert)
		: TorrentAlert((libtorrent::torrent_alert*) alert)
	{
		this->alert = new libtorrent::peer_blocked_alert(*alert);
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
		return System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(this->alert->ip.to_string()));
	}
}