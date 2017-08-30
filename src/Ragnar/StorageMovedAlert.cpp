#include "stdafx.h"
#include "StorageMovedAlert.h"
#include "Utils.h"

#include <libtorrent\alert_types.hpp>

namespace Ragnar
{
    StorageMovedAlert::StorageMovedAlert(libtorrent::storage_moved_alert* alert)
        : TorrentAlert((libtorrent::torrent_alert*) alert)
    {
        this->_path = Utils::GetManagedStringFromStandardString(alert->path);
    }

    System::String^ StorageMovedAlert::Path::get()
    {
        return this->_path;
    }
}
