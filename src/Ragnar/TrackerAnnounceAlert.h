#pragma once

#include "TrackerAlert.h"
#include "TrackerAnnounceEventType.h"

namespace libtorrent
{
	struct tracker_announce_alert;
}

namespace Ragnar
{
	public ref class TrackerAnnounceAlert : TrackerAlert
	{
	private:
		libtorrent::tracker_announce_alert* alert;
		bool disposed;

	internal:
		TrackerAnnounceAlert(libtorrent::tracker_announce_alert* alert);
	public:
		property TrackerAnnounceEvent^ Event { TrackerAnnounceEvent^ get(); }
		~TrackerAnnounceAlert();
	};
}