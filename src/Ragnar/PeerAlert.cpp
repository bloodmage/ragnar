#include "stdafx.h"
#include "PeerAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
    PeerAlert::PeerAlert(libtorrent::peer_alert* alert)
        : Alert((libtorrent::alert*) alert)
    {
		auto address = System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(alert->ip.address().to_string()));
        this->_endPoint = gcnew System::Net::IPEndPoint(address, alert->ip.port());
    }

    System::Net::EndPoint^ PeerAlert::EndPoint::get()
    {
        return this->_endPoint;
    }
}
