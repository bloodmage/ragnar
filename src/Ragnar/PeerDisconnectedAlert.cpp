#include "stdafx.h"
#include "PeerDisconnectedAlert.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	PeerDisconnectedAlert::PeerDisconnectedAlert(libtorrent::peer_disconnected_alert* alert)
		: PeerAlert((libtorrent::peer_alert*) alert)
	{
		this->alert = alert;
	}

	PeerDisconnectedAlert::~PeerDisconnectedAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}
}