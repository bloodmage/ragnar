#include "stdafx.h"
#include "TrackerErrorAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	TrackerErrorAlert::TrackerErrorAlert(libtorrent::tracker_error_alert* alert)
		: TrackerAlert((libtorrent::tracker_alert*) alert)
	{
		this->alert = new libtorrent::tracker_error_alert(*alert);
	}

	TrackerErrorAlert::~TrackerErrorAlert()
	{
		if (this->disposed)
		{
			return;
		}
		this->disposed = true;
		delete this->alert;
	}

	int TrackerErrorAlert::FailureCount::get()
	{
		return this->alert->times_in_row;
	}

	int TrackerErrorAlert::StatusCode::get()
	{
		return this->alert->status_code;
	}
}