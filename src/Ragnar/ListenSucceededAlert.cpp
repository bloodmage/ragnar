#include "stdafx.h"
#include "Alert.h"
#include "ListenSucceededAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
	ListenSucceededAlert::ListenSucceededAlert(libtorrent::listen_succeeded_alert* alert)
		: Alert((libtorrent::alert*) alert)
	{
		this->alert = alert;
	}

	System::Net::IPEndPoint^ ListenSucceededAlert::EndPoint::get()
	{
		auto endp = this->alert->endpoint;
		std::string ip_str = endp.address().to_string();
		System::Net::IPAddress^ ip_a = System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(ip_str));
		return gcnew System::Net::IPEndPoint(ip_a, (int)endp.port());
	}

	ListenSucceededAlert::~ListenSucceededAlert()
	{
		if (this->disposed)
			return;

		delete this->alert;
		this->disposed = true;
	}
}