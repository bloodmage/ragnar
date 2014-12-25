#pragma once

#include "Alert.h"

namespace libtorrent
{
	struct listen_failed_alert;
}

namespace Ragnar
{
	public ref class ListenFailedAlert : Alert
	{
	private:
		libtorrent::listen_failed_alert* alert;
		bool disposed;

	internal:
		ListenFailedAlert(libtorrent::listen_failed_alert* alert);

	public:
		property System::Net::IPEndPoint^ EndPoint  { System::Net::IPEndPoint^ get(); }
		// error_code

		~ListenFailedAlert();
	};
}
