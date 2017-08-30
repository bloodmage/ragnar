using System;
using System.Diagnostics;
using System.Threading;
using Caliburn.Micro;
using Ragnar.Client.Messages;
using Ragnar.Client.Models;
using Ragnar.Client.Views;

namespace Ragnar.Client.Services
{
    public class SessionService : ISessionService
        , IHandle<AddTorrentMessage>
    {
        public readonly ISession _session;
        private readonly IEventAggregator _eventAggregator;
        private readonly Thread _alertsThread;
        private bool _isStopping;
        public static SessionService Instance;

        public SessionService(ISession session, IEventAggregator eventAggregator)
        {
            if (session == null) throw new ArgumentNullException("session");
            if (eventAggregator == null) throw new ArgumentNullException("eventAggregator");

            _session = session;
            _eventAggregator = eventAggregator;
            _alertsThread = new Thread(ReadAlerts);

            _eventAggregator.Subscribe(this);

            Instance = this;
        }

        public void Start()
        {
            _session.SetAlertMask(SessionAlertCategory.All);
            _session.ListenOn(6881, 6889);
            (_session as Session).StartDht();

            // Start reading the alerts
            _alertsThread.Start();
        }

        public void Stop()
        {
            _isStopping = true;
            _alertsThread.Join();
        }

        public void Handle(AddTorrentMessage message)
        {
            _session.AddTorrent(message.Params);
        }

        private void ReadAlerts()
        {
            var timeout = TimeSpan.FromSeconds(0.5);
            var lastPost = DateTime.Now;

            while (!_isStopping)
            {
                if ((DateTime.Now - lastPost).TotalSeconds > 1)
                {
                    _session.PostTorrentUpdates();

                    lastPost = DateTime.Now;
                }

                var foundAlerts = _session.Alerts.PeekWait(timeout);
                if (!foundAlerts) continue;

                var alerts = _session.Alerts.PopAll();

                foreach (var alert in alerts)
                {
                    Debug.WriteLine(alert.Message);

                    if (alert is TorrentAddedAlert)
                    {
                        Handle((TorrentAddedAlert) alert);
                    }
                    else if (alert is StateUpdateAlert)
                    {
                        Handle((StateUpdateAlert) alert);
                    }
                    else if (alert is DhtImmutableItemAlert)
                    {
                        if (DHTInteroperate.instance != null)
                            DHTInteroperate.instance.Handle(alert as DhtImmutableItemAlert);
                    }
                    else if (alert is DhtMutableItemAlert)
                    {
                        if (DHTInteroperate.instance != null)
                            DHTInteroperate.instance.Handle(alert as DhtMutableItemAlert);
                    }
                    else if (alert is DhtPutAlert)
                    {
                        if (DHTInteroperate.instance != null)
                            DHTInteroperate.instance.Handle(alert as DhtPutAlert);
                    }
                    else if (alert is StatsAlert) { }
                    else
                    {
                        if (alert.Message.Contains("ncoming")) continue;
                        if (alert.Message.Contains("NAT")) continue;
                        if (alert.Message.Contains("UPnP")) continue;
                        if (alert.Message.Contains("DHT")) continue;
                        if (alert.Message.Contains(">>>")) continue;
                        if (alert.Message.Contains("<<<")) continue;
                        if (alert.Message.Contains("HAVE")) continue;
                        if (alert.Message.Contains("***")) continue;
                        Console.WriteLine("Unhandled alert {0}", alert.Message);
                    }
                }
            }
        }

        private void Handle(TorrentAddedAlert alert)
        {
            var t = new Torrent();
            t.InfoHash = alert.Handle.InfoHash.ToHex();
            if (alert.Handle.TorrentFile!=null)
            {
                t.Name = alert.Handle.TorrentFile.Name;
                t.Size = alert.Handle.TorrentFile.TotalSize;
            }
            t.State = alert.Handle.GetStatus().State;
            var msg = new TorrentAddedMessage(t);

            _eventAggregator.PublishOnBackgroundThread(msg);
        }

        private void Handle(StateUpdateAlert alert)
        {
            for (var i = 0; i < alert.Statuses.Count; i++)
            {
                var status = alert.Statuses[i];
                var ih = status.InfoHash.ToHex();
                if (status.State== TorrentState.Finished || status.State == TorrentState.Seeding)
                {
                    (Storages.Instance[ih] as ZipMemoryStorage).Save();
                }
                var torrent = new Torrent
                {
                    InfoHash = ih,
                    Name = status.Name,
                    Progress = status.Progress,
                    DownloadRate = status.DownloadRate,
                    UploadRate = status.UploadRate,
                    State = status.State
                };

                _eventAggregator.PublishOnBackgroundThread(new TorrentUpdatedMessage(torrent));
            }
        }
    }
}
