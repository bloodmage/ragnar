#include "stdafx.h"
#include "PerformanceAlert.h"
#include "TorrentHandle.h"
#include "PerformanceWarningType.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	PerformanceAlert::PerformanceAlert(libtorrent::performance_alert* alert)
		: TorrentAlert((libtorrent::torrent_alert*) alert)
	{
		this->_alert = new libtorrent::performance_alert(*alert);
	}

	PerformanceWarningType^ PerformanceAlert::WarningType::get()
	{
		return static_cast<PerformanceWarningType>(this->_alert->warning_code);
	}
}