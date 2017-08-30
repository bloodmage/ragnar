#include "stdafx.h"
#include "Alert.h"
#include "ListenFailedAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	ListenFailedAlert::ListenFailedAlert(libtorrent::listen_failed_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = alert;
	}

	System::Net::IPEndPoint^ ListenFailedAlert::EndPoint::get()
	{
		auto endp = this->alert->endpoint;
		std::string ip_str = endp.address().to_string();
		System::Net::IPAddress^ ip_a = System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(ip_str));
		return gcnew System::Net::IPEndPoint(ip_a, (int)endp.port());
	}

	ListenFailedAlert::~ListenFailedAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}
}