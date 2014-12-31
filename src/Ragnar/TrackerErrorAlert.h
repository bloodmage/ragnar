#pragma once

#include "TrackerAlert.h"

namespace libtorrent
{
	struct tracker_error_alert;
}

namespace Ragnar
{
	public ref class TrackerErrorAlert : TrackerAlert
	{
	private:
		libtorrent::tracker_error_alert* alert;
		bool disposed;

	internal:
		TrackerErrorAlert(libtorrent::tracker_error_alert* alert);
	public:
		property int FailureCount { int get(); }
		property int StatusCode {int get(); }
		~TrackerErrorAlert();
	};
}