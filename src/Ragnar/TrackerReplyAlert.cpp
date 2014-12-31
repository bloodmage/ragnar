#include "stdafx.h"
#include "TrackerReplyAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	TrackerReplyAlert::TrackerReplyAlert(libtorrent::tracker_reply_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->alert = alert;
	}

	TrackerReplyAlert::~TrackerReplyAlert()
	{
		if (this->disposed)
		{
			return;
		}
		this->disposed = true;
		delete this->alert;
	}

	int TrackerReplyAlert::PeerCount::get()
	{
		return this->alert->num_peers;
	}
}
