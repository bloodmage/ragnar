#pragma once

#include "Alert.h"

namespace libtorrent
{
	struct listen_succeeded_alert;
}

namespace Ragnar
{
	public ref class ListenSucceededAlert : Alert
	{
	private:
		libtorrent::listen_succeeded_alert* alert;
		bool disposed;

	internal:
		ListenSucceededAlert(libtorrent::listen_succeeded_alert* alert);

	public:
		property System::Net::IPEndPoint^ EndPoint  { System::Net::IPEndPoint^ get(); }
		~ListenSucceededAlert();
	};
}