#pragma once

#include "TrackerAlert.h"

namespace libtorrent
{
	struct tracker_reply_alert;
}

namespace Ragnar
{
	public ref class TrackerReplyAlert : TrackerAlert
	{
	private:
		libtorrent::tracker_reply_alert* alert;
		bool disposed;

	internal:
		TrackerReplyAlert(libtorrent::tracker_reply_alert* alert);
	public:
		property int PeerCount { int get(); }
		~TrackerReplyAlert();
	};
}