using Caliburn.Micro;
using Ragnar.Client.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Ragnar.Client.Views
{
    /// <summary>
    /// Interaction logic for DHTInteroperate.xaml
    /// </summary>
    public partial class DHTInteroperate : Window
    {
        public static DHTInteroperate instance;
        public DHTInteroperate()
        {
            InitializeComponent();
            instance = this;
            mycb = new PutterDelegate(putgood);
        }

        PutterDelegate mycb;

        Encoding g = Encoding.UTF8;

        internal void Handle(DhtPutAlert alert)
        {
            Dispatcher.Invoke(() =>
            {
                alertshow.Content = "DhtPutAlert";
                pubkeyrecv.Text = ED25519Helper.ToHexString(alert.PublicKey);
                salt.Text = g.GetString(alert.Salt);
                transid.Text = alert.Seq.ToString();
                sign.Text = ED25519Helper.ToHexString(alert.Signature);
                sha1.Text = alert.Target.ToHex();
                Console.WriteLine(alert.Message);
            });
        }
        internal void Handle(DhtMutableItemAlert alert)
        {
            Dispatcher.Invoke(() =>
            {
                alertshow.Content = "DhtMutableItemAlert";
                pubkeyrecv.Text = ED25519Helper.ToHexString(alert.Key);
                salt.Text = g.GetString(alert.Salt);
                transid.Text = alert.Seq.ToString();
                sign.Text = ED25519Helper.ToHexString(alert.Signature);
                value.Text = g.GetString(alert.Entry);
                Console.WriteLine(alert.Message);
            });
        }
        internal void Handle(DhtImmutableItemAlert alert)
        {
            Dispatcher.Invoke(()=>
            {
                alertshow.Content = "DhtImmutableItemAlert";
                sha1.Text = alert.Target.ToHex();
                value.Text = g.GetString(alert.Entry);
                Console.WriteLine(alert.Message);
            });
        }

        private void add1_Click(object sender, RoutedEventArgs e)
        {
            Session s = SessionService.Instance._session as Session;
            var bytes = g.GetBytes(value.Text);
            unsafe
            {
                fixed (byte* buf = bytes)
                    sha1.Text = s.DhtPutItem(buf, bytes.Length).ToHex();
            }
        }

        private void add2_Click(object sender, RoutedEventArgs e)
        {
            Session s = SessionService.Instance._session as Session;
            var key_ = ED25519Helper.FromHexString(pubkey.Text);
            var salt_ = g.GetBytes(salt.Text);
            s.DhtDynamicItemPutter.RegisterPutCallback(key_, mycb, salt_);
        }

        bool putgood(byte[] key, ref byte[] entry, byte[] signature, ref UInt64 seq, byte[] salt)
        {
            Console.WriteLine("===PUTGOOD===");
            Console.WriteLine("key: {0}", ED25519Helper.ToHexString(key));
            Console.WriteLine("entry: {0}", g.GetString(entry));
            Console.WriteLine("sign: {0}", ED25519Helper.ToHexString(signature));
            Console.WriteLine("seq: {0}", seq);
            Console.WriteLine("salt: {0}", g.GetString(salt));

            byte[] priv_ = null;
            byte[] outentry = null;
            UInt64 outseq = 0;
            Dispatcher.Invoke(new System.Action(() =>
            {
                outentry = g.GetBytes(value.Text);
                priv_ = ED25519Helper.FromHexString(privkey.Text);
                outseq = UInt64.Parse(transid.Text);
            }));
            entry = outentry;
            seq = outseq;

            DhtDynamicPutter.SignMutableItem(signature, entry, salt, seq, key, priv_);
            return true;
        }

        private void retr1_Click(object sender, RoutedEventArgs e)
        {
            Session s = SessionService.Instance._session as Session;
            s.DhtGetItem(new SHA1Hash(sha1.Text));
        }

        private void retr2_Click(object sender, RoutedEventArgs e)
        {
            Session s = SessionService.Instance._session as Session;
            var key = ED25519Helper.FromHexString(pubkey.Text);
            var salt_ = g.GetBytes(salt.Text);
            s.DhtGetItem(key, salt_);
        }

        private void gen_Click(object sender, RoutedEventArgs e)
        {
            var seed = ED25519Helper.CreateSeed();
            var pair = ED25519Helper.CreateKeypair(seed);
            privkey.Text = ED25519Helper.ToHexString(pair.Item2);
            pubkey.Text = ED25519Helper.ToHexString(pair.Item1);
        }

        private void refresh_Click(object sender, RoutedEventArgs e)
        {
            Session s = SessionService.Instance._session as Session;
            
            var sw = new System.IO.StringWriter();
            var stat = s.QueryStatus();
            sw.WriteLine("DhtNodes: {0}", stat.DhtNodes);
            sw.WriteLine("DhtGlobalNodes: {0}", stat.DhtGlobalNodes);
            sw.WriteLine("DhtNodeCache: {0}", stat.DhtNodeCache);
            sw.WriteLine("DhtDownloadRate: {0}", stat.DhtDownloadRate);
            sw.WriteLine("DhtTorrents: {0}", stat.DhtTorrents);
            sw.WriteLine("DhtTotalAllocations: {0}", stat.DhtTotalAllocations);
            sw.WriteLine("DhtUploadRate: {0}", stat.DhtUploadRate);
            sw.WriteLine("Routing table:");
            foreach (var i in stat.DhtRoutingTable)
                sw.WriteLine("  {0}: {1} {2}", i.LastActive, i.NumNodes, i.NumReplacements);
            dhtstat.Text = sw.ToString();

        }
    }
}
