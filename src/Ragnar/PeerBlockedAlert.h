#pragma once

#include "TorrentAlert.h"

namespace libtorrent
{
	struct peer_blocked_alert;
}

namespace Ragnar
{
	public ref class PeerBlockedAlert : TorrentAlert
	{
	private:
		libtorrent::peer_blocked_alert* alert;
		bool disposed;

	internal:
		PeerBlockedAlert(libtorrent::peer_blocked_alert* alert);
	public:
		property System::Net::IPAddress^ Address { System::Net::IPAddress^ get(); }
		~PeerBlockedAlert();
	};
}