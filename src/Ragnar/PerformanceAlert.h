#pragma once

#include "TorrentAlert.h"
#include "PerformanceWarningType.h"

namespace libtorrent
{
	struct performance_alert;
}

namespace Ragnar
{
	ref class TorrentHandle;

	public ref class PerformanceAlert : TorrentAlert
	{
	private:
		libtorrent::performance_alert* _alert;

	internal:
		PerformanceAlert(libtorrent::performance_alert* alert);

		public:
			property PerformanceWarningType^ WarningType { PerformanceWarningType^ get(); }
	};
}