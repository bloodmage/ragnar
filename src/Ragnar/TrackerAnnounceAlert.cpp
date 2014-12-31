#include "stdafx.h"
#include "TrackerAnnounceAlert.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	TrackerAnnounceAlert::TrackerAnnounceAlert(libtorrent::tracker_announce_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->alert = alert;
	}

	TrackerAnnounceAlert::~TrackerAnnounceAlert()
	{
		if (this->disposed)
		{
			return;
		}
		this->disposed = true;
		delete this->alert;
	}

	TrackerAnnounceEvent^ TrackerAnnounceAlert::Event::get()
	{
		return static_cast<TrackerAnnounceEvent>(this->alert->event);
	}
}
