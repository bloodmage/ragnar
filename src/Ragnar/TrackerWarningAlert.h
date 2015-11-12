#pragma once

#include "TrackerAlert.h"

namespace libtorrent
{
	struct tracker_warning_alert;
}

namespace Ragnar
{
	public ref class TrackerWarningAlert : TrackerAlert
	{
	private:
		libtorrent::tracker_warning_alert* alert;
		bool disposed;

	internal:
		TrackerWarningAlert(libtorrent::tracker_warning_alert* alert);
	public:
		property System::String^ Message { System::String^ get(); }
		~TrackerWarningAlert();
	};
}