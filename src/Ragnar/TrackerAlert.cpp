#include "stdafx.h"
#include "TrackerAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
    TrackerAlert::TrackerAlert(libtorrent::tracker_alert* alert)
        : TorrentAlert((libtorrent::torrent_alert*) alert)
    {
		this->_url = Utils::GetManagedStringFromStandardString(alert->url);
    }

    System::String^ TrackerAlert::Url::get()
    {
        return this->_url;
    }
}
