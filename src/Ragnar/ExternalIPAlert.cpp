#include "stdafx.h"
#include "Alert.h"
#include "ExternalIPAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	ExternalIPAlert::ExternalIPAlert(libtorrent::external_ip_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = alert;
	}

	System::Net::IPAddress^ ExternalIPAlert::IP::get()
	{
		auto add = this->alert->external_address;
		System::Net::IPAddress^ ip_a = System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(add.to_string()));
		return ip_a;
	}

	ExternalIPAlert::~ExternalIPAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}
}