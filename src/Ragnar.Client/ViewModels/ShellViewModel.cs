﻿using System;
using System.Linq;
using Caliburn.Micro;
using Microsoft.Win32;
using Ragnar.Client.Messages;
using Ragnar.Client.Models;
using Ragnar.Client.Services;
using Ragnar.Client.Views;
using System.IO;

namespace Ragnar.Client.ViewModels
{
    public sealed class ShellViewModel : Screen
        , IHandle<TorrentAddedMessage>
        , IHandle<TorrentUpdatedMessage>
    {
        private readonly ISessionService _sessionService;
        private readonly IWindowManager _windowManager;
        private readonly IEventAggregator _eventAggregator;
        private readonly IObservableCollection<Torrent> _torrents; 

        public ShellViewModel(ISessionService sessionService,
            IWindowManager windowManager,
            IEventAggregator eventAggregator)
        {
            if (sessionService == null) throw new ArgumentNullException("sessionService");
            if (windowManager == null) throw new ArgumentNullException("windowManager");
            if (eventAggregator == null) throw new ArgumentNullException("eventAggregator");

            _sessionService = sessionService;
            _windowManager = windowManager;
            _eventAggregator = eventAggregator;
            _torrents = new BindableCollection<Torrent>();

            _eventAggregator.Subscribe(this);
        }

        public IObservableCollection<Torrent> Torrents
        {
            get { return _torrents; }
        }

        public void Handle(TorrentAddedMessage message)
        {
            _torrents.Add(message.Torrent);
        }

        public void Handle(TorrentUpdatedMessage message)
        {

            var torrent = Torrents.SingleOrDefault(t => t.InfoHash == message.Torrent.InfoHash);
            if (torrent == null) return;

            torrent.Progress = message.Torrent.Progress;
            torrent.DownloadRate = message.Torrent.DownloadRate;
            torrent.UploadRate = message.Torrent.UploadRate;
            torrent.State = message.Torrent.State;
        }

        protected override void OnActivate()
        {
            _sessionService.Start();
        }

        protected override void OnDeactivate(bool close)
        {
            _sessionService.Stop();
        }

        public void ShowAddTorrent()
        {
            var dialog = new OpenFileDialog
            {
                Filter = "Torrents (*.torrent)|*.torrent",
                Title = "Select a .torrent file to open"
            };

            if (dialog.ShowDialog() == true)
            {
                // Show Add dialog with torrent selected
                var torrentInfo = new TorrentInfo(dialog.FileName);
                _windowManager.ShowDialog(new AddTorrentViewModel(_eventAggregator, torrentInfo));
            }
        }

        public void DHTTest()
        {
            new DHTInteroperate().Show();
        }

        public void MSGTest()
        {
            (SessionService.Instance._session as Session).AddTorrentExtension(new Ragnar.Client.Plugin.MessagePlugin());
            var msg = new MessagePassing();
            msg.Show();
            var hash = Ragnar.Client.Plugin.MessagePassingChannels.Channels().First();
            msg.Install(hash);

            var _addParams = AddTorrentParams.FromInfoHash(new SHA1Hash(hash));
            _addParams.SavePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Downloads");
            _eventAggregator.PublishOnBackgroundThread(new AddTorrentMessage(_addParams));
        }
    }
}
