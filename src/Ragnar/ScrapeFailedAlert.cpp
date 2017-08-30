#include "stdafx.h"
#include "ScrapeFailedAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	ScrapeFailedAlert::ScrapeFailedAlert(libtorrent::scrape_failed_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->ms = Utils::GetManagedStringFromStandardString(alert->msg);
	}

	System::String^ ScrapeFailedAlert::Message::get()
	{
		return this->ms;
	}
}