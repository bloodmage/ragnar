#include "stdafx.h"
#include "Alert.h"
#include "AlertFactory.h"
#include "FileCompletedAlert.h"
#include "FileRenamedAlert.h"
#include "MetadataReceivedAlert.h"
#include "PeerBanAlert.h"
#include "PeerConnectAlert.h"
#include "PeerUnsnubbedAlert.h"
#include "PieceFinishedAlert.h"
#include "SaveResumeDataAlert.h"
#include "ScrapeReplyAlert.h"
#include "StateChangedAlert.h"
#include "StateUpdateAlert.h"
#include "StatsAlert.h"
#include "StorageMovedAlert.h"
#include "TorrentAddedAlert.h"
#include "TorrentErrorAlert.h"
#include "TorrentFinishedAlert.h"
#include "TorrentPausedAlert.h"
#include "TorrentRemovedAlert.h"
#include "TorrentResumedAlert.h"
#include "TorrentCheckedAlert.h"
#include "UnwantedBlockAlert.h"
#include "PerformanceAlert.h"
#include "ListenFailedAlert.h"
#include "ExternalIPAlert.h"
#include "ListenSucceededAlert.h"
#include "PeerBlockedAlert.h"
#include "PeerDisconnectedAlert.h"
#include "TrackerWarningAlert.h"
#include "TrackerErrorAlert.h"
#include "TrackerReplyAlert.h"
#include "TrackerAnnounceAlert.h"
#include "ScrapeFailedAlert.h"
#include "DhtMutableItemAlert.h"
#include "DhtImmutableItemAlert.h"
#include "DhtPutAlert.h"

#include <libtorrent\alert.hpp>
#include <libtorrent\alert_types.hpp>
#include <libtorrent\session.hpp>

namespace Ragnar
{
    AlertFactory::AlertFactory(libtorrent::session& session)
        : _session(session)
    {
    }

    Alert^ AlertFactory::Pop()
    {
        std::auto_ptr<libtorrent::alert> alert = this->_session.pop_alert();

        if (!alert.get())
        {
            return nullptr;
        }

        return this->GetAlert(alert);
    }

    System::Collections::Generic::IEnumerable<Alert^>^ AlertFactory::PopAll()
    {
        std::deque<libtorrent::alert*> alerts;
        this->_session.pop_alerts(&alerts);

        auto result = gcnew System::Collections::Generic::List<Alert^>();

        for (std::deque<libtorrent::alert*>::iterator i = alerts.begin(), end(alerts.end()); i != end; i++)
        {
            std::auto_ptr<libtorrent::alert> a(*i);

            if (!a.get())
            {
                continue;
            }

            auto alert = this->GetAlert(a);

            if (alert != nullptr)
            {
                result->Add(alert);
            }
        }

        return result;
    }

    bool AlertFactory::PeekWait(System::TimeSpan timeout)
    {
        libtorrent::alert const* a = this->_session.wait_for_alert(libtorrent::milliseconds((unsigned long)timeout.TotalMilliseconds));
        return a != 0;
    }

    Alert^ AlertFactory::GetAlert(std::auto_ptr<libtorrent::alert> alert)
    {
        switch (alert->type())
        {
        case libtorrent::file_completed_alert::alert_type:
            return gcnew FileCompletedAlert(static_cast<libtorrent::file_completed_alert*>(alert.get()));

        case libtorrent::file_renamed_alert::alert_type:
            return gcnew FileRenamedAlert(static_cast<libtorrent::file_renamed_alert*>(alert.get()));

        case libtorrent::metadata_received_alert::alert_type:
            return gcnew MetadataReceivedAlert(static_cast<libtorrent::metadata_received_alert*>(alert.get()));

        case libtorrent::peer_ban_alert::alert_type:
            return gcnew PeerBanAlert(static_cast<libtorrent::peer_ban_alert*>(alert.get()));

        case libtorrent::peer_connect_alert::alert_type:
            return gcnew PeerConnectAlert(static_cast<libtorrent::peer_connect_alert*>(alert.get()));

        case libtorrent::peer_unsnubbed_alert::alert_type:
            return gcnew PeerUnsnubbedAlert(static_cast<libtorrent::peer_unsnubbed_alert*>(alert.get()));

        case libtorrent::piece_finished_alert::alert_type:
            return gcnew PieceFinishedAlert(static_cast<libtorrent::piece_finished_alert*>(alert.get()));

        case libtorrent::save_resume_data_alert::alert_type:
            return gcnew SaveResumeDataAlert(static_cast<libtorrent::save_resume_data_alert*>(alert.get()));

        case libtorrent::scrape_reply_alert::alert_type:
            return gcnew ScrapeReplyAlert(static_cast<libtorrent::scrape_reply_alert*>(alert.get()));

        case libtorrent::state_changed_alert::alert_type:
            return gcnew StateChangedAlert(static_cast<libtorrent::state_changed_alert*>(alert.get()));

        case libtorrent::state_update_alert::alert_type:
            return gcnew StateUpdateAlert(static_cast<libtorrent::state_update_alert*>(alert.get()));

        case libtorrent::stats_alert::alert_type:
            return gcnew StatsAlert(static_cast<libtorrent::stats_alert*>(alert.get()));

        case libtorrent::storage_moved_alert::alert_type:
            return gcnew StorageMovedAlert(static_cast<libtorrent::storage_moved_alert*>(alert.get()));

        case libtorrent::torrent_added_alert::alert_type:
            return gcnew TorrentAddedAlert(static_cast<libtorrent::torrent_added_alert*>(alert.get()));

        case libtorrent::torrent_error_alert::alert_type:
            return gcnew TorrentErrorAlert(static_cast<libtorrent::torrent_error_alert*>(alert.get()));

        case libtorrent::torrent_finished_alert::alert_type:
            return gcnew TorrentFinishedAlert(static_cast<libtorrent::torrent_finished_alert*>(alert.get()));

        case libtorrent::torrent_paused_alert::alert_type:
            return gcnew TorrentPausedAlert(static_cast<libtorrent::torrent_paused_alert*>(alert.get()));

        case libtorrent::torrent_removed_alert::alert_type:
            return gcnew TorrentRemovedAlert(static_cast<libtorrent::torrent_removed_alert*>(alert.get()));

        case libtorrent::torrent_resumed_alert::alert_type:
            return gcnew TorrentResumedAlert(static_cast<libtorrent::torrent_resumed_alert*>(alert.get()));

        case libtorrent::torrent_checked_alert::alert_type:
            return gcnew TorrentCheckedAlert(static_cast<libtorrent::torrent_checked_alert*>(alert.get()));

        case libtorrent::unwanted_block_alert::alert_type:
            return gcnew UnwantedBlockAlert(static_cast<libtorrent::unwanted_block_alert*>(alert.get()));

		case libtorrent::performance_alert::alert_type:
			return gcnew PerformanceAlert(static_cast<libtorrent::performance_alert*>(alert.get()));

		case libtorrent::listen_failed_alert::alert_type:
			return gcnew ListenFailedAlert(static_cast<libtorrent::listen_failed_alert*>(alert.get()));

		case libtorrent::external_ip_alert::alert_type:
			return gcnew ExternalIPAlert(static_cast<libtorrent::external_ip_alert*>(alert.get()));

		case libtorrent::listen_succeeded_alert::alert_type:
			return gcnew ListenSucceededAlert(static_cast<libtorrent::listen_succeeded_alert*>(alert.get()));

		case libtorrent::peer_blocked_alert::alert_type:
			return gcnew PeerBlockedAlert(static_cast<libtorrent::peer_blocked_alert*>(alert.get()));

		case libtorrent::peer_disconnected_alert::alert_type:
			return gcnew PeerDisconnectedAlert(static_cast<libtorrent::peer_disconnected_alert*>(alert.get()));

		case libtorrent::tracker_warning_alert::alert_type:
			return gcnew TrackerWarningAlert(static_cast<libtorrent::tracker_warning_alert*>(alert.get()));

		case libtorrent::tracker_error_alert::alert_type:
			return gcnew TrackerErrorAlert(static_cast<libtorrent::tracker_error_alert*>(alert.get()));

		case libtorrent::tracker_reply_alert::alert_type:
			return gcnew TrackerReplyAlert(static_cast<libtorrent::tracker_reply_alert*>(alert.get()));

		case libtorrent::tracker_announce_alert::alert_type:
			return gcnew TrackerAnnounceAlert(static_cast<libtorrent::tracker_announce_alert*>(alert.get()));

		case libtorrent::scrape_failed_alert::alert_type:
			return gcnew ScrapeFailedAlert(static_cast<libtorrent::scrape_failed_alert*>(alert.get()));
			
		case libtorrent::dht_mutable_item_alert::alert_type:
			return gcnew DhtMutableItemAlert(static_cast<libtorrent::dht_mutable_item_alert*>(alert.get()));

		case libtorrent::dht_immutable_item_alert::alert_type:
			return gcnew DhtImmutableItemAlert(static_cast<libtorrent::dht_immutable_item_alert*>(alert.get()));

		case libtorrent::dht_put_alert::alert_type:
			return gcnew DhtPutAlert(static_cast<libtorrent::dht_put_alert*>(alert.get()));

		default:
			return gcnew Alert(alert.get());
		}

        return nullptr;
    }
}
