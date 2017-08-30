using MiscUtil.IO;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Ragnar.Client.Plugin
{
    public class MessagePassingChannel
    {
        public byte[] ChannelPublic = Utils.ParseHex("df41ea3dc72f0e7f610d6ea16ca624f7e579487e77f52eb361a471467346a23e");
        public byte[] ChannelPrivate = Utils.ParseHex("4053082ba868f423976cc3d8229b914b2db1edbec53bf34d57f281a4137d444f977573d44ecaa644e56e4f94a29589a54ccaba4aca9e26d27bd62327797eab93");

        public string ChannelSHA1;
        public byte[] ChannelValue = null;
        public byte[] ValueSign = null; //64bit
        public UInt64 Version = 0;

        public event Action ChannelUpdated;

        internal void OnChannelUpdated()
        {
            if (ChannelUpdated != null)
            {
                //Invokes the delegates.
                ChannelUpdated();
            }
        }
        public void NewVersionMessage(UInt64 version, byte[] msg)
        {
            if (ChannelPrivate == null) throw new InvalidOperationException("Private key missing");
            if (version <= Version) throw new InvalidOperationException("version less than or equal to newest");
            byte[] sign = new byte[64];
            var cmsg = CompositeMsg(version, msg);
            ED25519Helper.Sign(sign, cmsg, ChannelPublic, ChannelPrivate);

            lock (this)
            {
                Version = version;
                ValueSign = sign;
                ChannelValue = msg;
            }

            OnChannelUpdated();
        }

        private byte[] CompositeMsg(ulong v, byte[] msg)
        {
            var ms = new MemoryStream();
            var bw = new EndianBinaryWriter(MiscUtil.Conversion.EndianBitConverter.Big, ms);
            bw.Write(v); //8
            bw.Write((int)msg.Length); //4
            bw.Write(msg);
            bw.Flush();
            return ms.ToArray();
        }
    }
    public static class MessagePassingChannels
    {
        static Dictionary<string, MessagePassingChannel> chns;
        static MessagePassingChannels()
        {
            chns = new Dictionary<string, MessagePassingChannel>();
            var newchn = new MessagePassingChannel();
            newchn.ChannelSHA1 = Utils.ToHex(new Unsafe.SHA1(newchn.ChannelPublic, 0, 20).Final());
            chns.Add(newchn.ChannelSHA1, newchn);
        }
        public static Dictionary<string, MessagePassingChannel>.KeyCollection Channels() { return chns.Keys; }
        public static MessagePassingChannel GetChannel(string hash)
        {
            if (!chns.ContainsKey(hash)) return null;
            return chns[hash];
        }
    }

    class MessagePlugin : Ragnar.Unsafe.Plugin
    {
        unsafe public override Unsafe.TorrentPlugin NewTorrent(TorrentHandle torrent, void* userdata)
        {
            var hex = torrent.InfoHash.ToHex();
            var chn = MessagePassingChannels.GetChannel(hex);
            if (chn != null)
            {
                Console.WriteLine("Channel found, login in");
                return new MessageTorrentPlugin(chn);
            }
            return null;
        }
    }
    
    class MessageTorrentPlugin : Ragnar.Unsafe.TorrentPlugin
    {
        public MessageTorrentPlugin(MessagePassingChannel chn)
        {
            this.chn = chn;
        }
        public override Unsafe.PeerPlugin NewConnection(Unsafe.PeerConnectionHandle peerconn)
        {
            if (peerconn.Type != Unsafe.ConnectionType.BITTORRENT_CONNECTION) return null;
            Console.WriteLine("New connection {0}", peerconn.Pid.ToHex());
            return new MessagePeerConnection(peerconn.GetPeerConnection(), chn);
        }

        public MessagePassingChannel chn;
    }

    class MessagePeerConnection : Ragnar.Unsafe.PeerPlugin
    {
        const int MessageIdx = 20;
        const string Message = "rg_msg";
        const string MessageVer = "rg_msgver";
        UInt64 RemoteVersion;
        UInt64 SentVersion = 0;
        UInt64 ReceivedVersion = 0;
        private Unsafe.PeerConnection peerconn;
        MessagePassingChannel chn;

        public MessagePeerConnection(Unsafe.PeerConnection peerconn, MessagePassingChannel chn)
        {
            this.peerconn = peerconn;
            this.chn = chn;
        }

        public override void AddHandshake(Unsafe.Entry entry)
        {
            base.AddHandshake(entry);
            var ed = entry as Unsafe.EntryDictionary;
            var m = ed["m"] as Unsafe.EntryDictionary;
            var ei = new Unsafe.EntryInteger(MessageIdx);
            var ev = new Unsafe.EntryString(chn.Version.ToString());
            m.Add(Message, ei);
            m.Add(MessageVer, ev);
            Console.WriteLine("HANDSHAKE VALUE: {0}", Encoding.UTF8.GetString(m.Encode()));
        }

        public override bool OnExtensionHandshake(Unsafe.LazyEntry h)
        {
            if (h.Type!= Unsafe.DataType.DICTIONARY)
                return false;
            var msg = h.Dict_Find("m", Unsafe.DataType.DICTIONARY);
            if (msg == null)
                return false;
            var msgret = msg.Dict_Find(Message, Unsafe.DataType.INT);
            if (msgret == null)
                return false;
            if (msgret.Int_Value() != MessageIdx)
                return false;
            msgret = msg.Dict_Find(MessageVer, Unsafe.DataType.STRING);
            if (msgret == null)
                return false;
            RemoteVersion = UInt64.Parse(msgret.String_Val());

            MaybeSendPacket();
            return true;
        }

        private void MaybeSendPacket()
        {
            lock (chn)
            {

                if (SentVersion >= chn.Version) return; //No new version found
                if (RemoteVersion >= chn.Version) return; //Remote version not updated (or even ahead of us)

                WritePacket();
                SentVersion = chn.Version;
            }
        }

        private void WritePacket()
        {
            MemoryStream ms = new MemoryStream(200);
            EndianBinaryWriter bw = new EndianBinaryWriter(MiscUtil.Conversion.EndianBitConverter.Big, ms);
            int psize = (Int32)(2 + chn.ChannelValue.Length + 8 + 4 + chn.ValueSign.Length);
            Console.WriteLine("Packet to send: {0}", psize);
            bw.Write(psize);//TotalSize don't include size itself
            bw.Write((byte)Unsafe.MessageType.MSG_EXTENDED); //1
            bw.Write((byte)MessageIdx); //1
            bw.Write(chn.ValueSign);
            bw.Write(chn.Version); //8
            bw.Write((int)chn.ChannelValue.Length); //4
            bw.Write(chn.ChannelValue);
            bw.Flush();
            var data = ms.ToArray();
            if (data.Length != psize + 4) throw new Exception("Packet size incorrect");
            unsafe
            {
                fixed (byte* b = data)
                {
                    peerconn.SendBuffer((sbyte*)b, (int)ms.Length);
                }
            }
        }

        unsafe public override bool OnExtended(int length, int msg, sbyte* buf, int buflen)
        {
            if (msg != MessageIdx) return false;
            if (length > 1024)
            {
                peerconn.Disconnect((int)Unsafe.ErrorCodeEnum.metadata_too_large, Unsafe.operation_t.op_sock_read, 2);
                return true;
            }

            if (!peerconn.btpeer_PacketFinished()) return true;
            //Copy buffer
            byte[] mbuf = new byte[buflen];
            Marshal.Copy((IntPtr)buf, mbuf, 0, buflen);

            MemoryStream ms = new MemoryStream(mbuf);
            EndianBinaryReader br = new EndianBinaryReader(MiscUtil.Conversion.EndianBitConverter.Big, ms);

            byte[] sign = br.ReadBytes(64);
            if (sign.Length != 64)
            {
                peerconn.Disconnect((int)Unsafe.ErrorCodeEnum.invalid_metadata_message, Unsafe.operation_t.op_sock_read, 2);
                return true;
            } //Sign too short

            var ver = br.ReadUInt64();
            if (ver <= ReceivedVersion && ReceivedVersion < RemoteVersion)
            {
                peerconn.Disconnect((int)Unsafe.ErrorCodeEnum.invalid_metadata_message, Unsafe.operation_t.op_sock_read, 2);
                return true;
            } //Duplicate transmission or outdated version
            ReceivedVersion = ver;
            if (ReceivedVersion > RemoteVersion) RemoteVersion = ReceivedVersion; //Indicates updated remote state

            var cvlen = br.ReadInt32();
            byte[] cvdata = br.ReadBytes(cvlen);
            if (cvdata.Length != cvlen)
            {
                peerconn.Disconnect((int)Unsafe.ErrorCodeEnum.invalid_metadata_message, Unsafe.operation_t.op_sock_read, 2);
                return true;
            } //Data too short

            fixed (byte* publickey = &chn.ChannelPublic[0])
                if (ED25519Helper.Verify((byte*)buf, (byte*)buf + 64, (uint)(8 + 4 + cvlen), publickey) == 0)
                {
                    peerconn.Disconnect((int)Unsafe.ErrorCodeEnum.invalid_metadata_message, Unsafe.operation_t.op_sock_read, 2);
                    return true;
                } //Verify failed

            lock (chn)
            {
                if (ver > chn.Version)
                {
                    chn.Version = ver;
                    chn.ChannelValue = cvdata;
                    chn.ValueSign = sign;
                    try
                    {
                        chn.OnChannelUpdated();
                    }
                    catch { }
                } //Out state is updated

            }
            return true;
        }

        unsafe public override void OnDisconnect(void* errcode)
        {
            Console.WriteLine("Peer disconnect");
        }
        public override unsafe void Tick()
        {
            base.Tick();
            MaybeSendPacket();
        }
    }
}
