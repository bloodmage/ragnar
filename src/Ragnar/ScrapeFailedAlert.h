#pragma once
#include "stdafx.h"
#include "TrackerAlert.h"

namespace libtorrent
{
	struct scrape_failed_alert;
}

namespace Ragnar
{
	public ref class ScrapeFailedAlert : TrackerAlert
	{
	private:
		System::String^ ms;
		int _incomplete;

	internal:
		ScrapeFailedAlert(libtorrent::scrape_failed_alert* alert);

	public:
		property System::String^ Message { System::String^ get(); }
	};
}