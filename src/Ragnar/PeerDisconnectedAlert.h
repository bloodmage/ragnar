#pragma once

#include "PeerAlert.h"

namespace libtorrent
{
	struct peer_disconnected_alert;
}

namespace Ragnar
{
	public ref class PeerDisconnectedAlert : PeerAlert
	{
	private:
		libtorrent::peer_disconnected_alert* alert;
		bool disposed;

	internal:
		PeerDisconnectedAlert(libtorrent::peer_disconnected_alert* alert);
	public:
		//error code
		~PeerDisconnectedAlert();
	};
}