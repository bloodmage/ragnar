#include "stdafx.h"
#include "ScrapeFailedAlert.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	ScrapeFailedAlert::ScrapeFailedAlert(libtorrent::scrape_failed_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->ms = gcnew System::String(alert->msg.c_str());
		delete alert;
	}

	System::String^ ScrapeFailedAlert::Message::get()
	{
		return this->ms;
	}
}