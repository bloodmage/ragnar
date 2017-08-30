using Ragnar.Client.Plugin;
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
    /// Interaction logic for MessagePassing.xaml
    /// </summary>
    public partial class MessagePassing : Window
    {
        public MessagePassing()
        {
            InitializeComponent();
        }

        string ChannelSHA1;
        MessagePassingChannel chn;
        public void Install (string SHA1)
        {
            ChannelSHA1 = SHA1;
            chn = MessagePassingChannels.GetChannel(ChannelSHA1);
            pubkey.Text = Utils.ToHex(chn.ChannelPublic);
            privkey.Text = Utils.ToHex(chn.ChannelPrivate);

            chn.ChannelUpdated += chn_ChannelUpdated;
            this.Title += "_" + SHA1;
        }

        void chn_ChannelUpdated()
        {
            Dispatcher.Invoke(new Action(() =>
            {
                gen1.Text = chn.Version.ToString();
                data1.Text = enc.GetString(chn.ChannelValue);
                sign1.Text = Utils.ToHex(chn.ValueSign);

                gen2.Text = (chn.Version + 1).ToString();
            }));
        }

        public Encoding enc = Encoding.UTF8;
        private void commit_Click(object sender, RoutedEventArgs e)
        {
            chn.NewVersionMessage(UInt64.Parse(gen2.Text), enc.GetBytes(data2.Text));
        }


    }
}
