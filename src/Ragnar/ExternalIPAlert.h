#pragma once

#include "Alert.h"

namespace libtorrent
{
	struct external_ip_alert;
}

namespace Ragnar
{
	public ref class ExternalIPAlert : Alert
	{
	private:
		libtorrent::external_ip_alert* alert;
		bool disposed;

	internal:
		ExternalIPAlert(libtorrent::external_ip_alert* alert);

	public:
		property System::Net::IPAddress^ IP  { System::Net::IPAddress^ get(); }
		~ExternalIPAlert();
	};
}