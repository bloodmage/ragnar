#include "stdafx.h"
#include "TrackerWarningAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	TrackerWarningAlert::TrackerWarningAlert(libtorrent::tracker_warning_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->alert = new libtorrent::tracker_warning_alert(*alert);
	}

	TrackerWarningAlert::~TrackerWarningAlert()
	{
		if (this->disposed)
		{
			return;
		}
		this->disposed = true;
		delete this->alert;
	}

	System::String^ TrackerWarningAlert::Message::get()
	{
		return Utils::GetManagedStringFromStandardString(this->alert->msg);
	}
}