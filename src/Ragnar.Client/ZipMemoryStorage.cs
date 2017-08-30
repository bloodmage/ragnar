using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;

namespace Ragnar.Client
{
    [Serializable]
    [DataContract(Name = "file")]
    public class SingleFileInfo
    {
        [DataMember(Name = "name")]
        public string Filename { get; internal set; }
        [DataMember(Name = "start")]
        public long Start { get; internal set; }
        [DataMember(Name = "length")]
        public long Length { get; internal set; }
    }

    [Serializable]
    [DataContract(Name = "files")]
    public class StoreFiles : List<SingleFileInfo> { }

    [Serializable]
    [DataContract(Name = "storinfo")]
    public class StorageInfo
    {
        [DataMember(Name = "files")]
        public StoreFiles Files { get; internal set; }
        [DataMember(Name = "piecelen")]
        public int PieceLen { get; internal set; }
    }
    class ZipMemoryStorage : Unsafe.StorageInterfaceBase
    {
        public override bool Initialize(Unsafe.StorageError err) { return false; }
        public override void DeleteFiles(int options, Unsafe.StorageError err) { }
        public override bool HasAnyFile(Unsafe.StorageError err) { return filedata.Count > 0; }
        public override void ReleaseFiles(Unsafe.StorageError err) { }
        public override void FinalizeFile(int index, Unsafe.StorageError ret) { }
        public override int MoveStorage(string savepath, int flags, Unsafe.StorageError err) { return 0; }

        HashSet<int> dirtylist = new HashSet<int>();
        public unsafe override int Read(sbyte* buf, int slot, int offset, int size)
        {
            if (!filedata.ContainsKey(slot)) return 0;
            var ms = filedata[slot];

            int available = (int)ms.Length - offset;
            if (available <= 0) return 0;
            if (available > size) available = size;
            ms.Seek(offset, SeekOrigin.Begin);
            for (int i = 0; i < available; i++)
                buf[i] = (sbyte)ms.ReadByte();
            return available;
        }

        public unsafe override int Write(sbyte* buf, int slot, int offset, int size)
        {
            if (!filedata.ContainsKey(slot))
                filedata[slot] = new MemoryStream();

            var ms = filedata[slot];
            ms.Seek(offset, SeekOrigin.Begin);
            for (int i = 0; i < size; i++)
                ms.WriteByte((byte)buf[i]);
            lock (dirtylist)
                dirtylist.Add(slot);

            return size;
        }


        public override void RenameFile(int index, string new_filename, Unsafe.StorageError err) { err.MakeErrorCode(Unsafe.ErrorCodeEnum.torrent_invalid_name); err.File = 0; err.Operation = Unsafe.file_operation_t.rename; }

        public unsafe override void SetFilePriority(void* prio, Unsafe.StorageError ret) { }

        public unsafe override bool VerifyResumeData(void* rd, void* err, Unsafe.StorageError ret) { return false; }
        public unsafe override void WriteResumeData(void* rd, Unsafe.StorageError ret) { }

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            //Save zip stream
            Save();
            Storages.Instance.Remove(hash.ToHex());
        }


        /// <summary>
        /// TODO: call save on finished
        /// </summary>
        public void Save()
        {
            if (dirtylist.Count != 0)
            {

                storpath = Path.Combine(basedir, hash.ToHex() + ".zip");
                using (var fs = File.Open(storpath, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.Read))
                using (var zip = new ZipArchive(fs, ZipArchiveMode.Update))
                {
                    foreach (var i in dirtylist)
                    {
                        var ent = zip.GetEntry(i.ToString());
                        if (ent == null) ent = zip.CreateEntry(i.ToString());
                        using (var s = ent.Open())
                        {
                            var buf = filedata[i];

                            s.SetLength(buf.Length);
                            buf.Seek(0, SeekOrigin.Begin);
                            buf.CopyTo(s);
                        }
                    }

                    StorageInfo info = new StorageInfo();
                    info.Files = Files;
                    Files.TrimExcess();
                    info.PieceLen = piecelen;
                    var serialize = new DataContractJsonSerializer(typeof(StorageInfo));
                    using (var ms = new MemoryStream())
                    {
                        serialize.WriteObject(ms, info);
                        ms.Seek(0, SeekOrigin.Begin);
                        var ent = zip.GetEntry("info");
                        if (ent == null) ent = zip.CreateEntry("info");
                        using (var s = ent.Open())
                        {
                            s.SetLength(ms.Length);
                            ms.CopyTo(s);
                        }
                    }
                }

                dirtylist.Clear();
           }
        }

        Dictionary<int, MemoryStream> filedata = new Dictionary<int, MemoryStream>();
        FileStorage stor;
        string basedir;
        private SHA1Hash hash;

        String storpath;
        private void Load()
        {
            storpath = Path.Combine(basedir, hash.ToHex() + ".zip");
            try
            {
                if (File.Exists(storpath))
                    using (var fs = File.Open(storpath, FileMode.Open, FileAccess.Read, FileShare.Read))
                    using (var zip = new ZipArchive(fs, ZipArchiveMode.Read))
                    {
                        foreach (var i in zip.Entries)
                        {
                            if (i.Name == "info") continue;
                            var idx = int.Parse(i.Name);
                            var ms = new MemoryStream();
                            using (var si = i.Open())
                                si.CopyTo(ms);
                            ms.Seek(0, SeekOrigin.Begin);
                            filedata[idx] = ms;
                        }

                        var serialize = new DataContractJsonSerializer(typeof(StorageInfo));
                        {
                            var ent = zip.GetEntry("info");
                            StorageInfo info = null;
                            using (var s = ent.Open())
                                info = (StorageInfo)serialize.ReadObject(s);
                            Files = info.Files;
                            piecelen = info.PieceLen;
                        }
                    }
            }
            catch { }
        }

        public StoreFiles Files { get; internal set; }
        int piecelen;
        void FillStorage(FileStorage real)
        {
            piecelen = real.PieceLength;
            Files = new StoreFiles();
            for (var i = 0;i<real.NumFiles;i++)
            {
                var fe = real.At(i);
                Files.Add(new SingleFileInfo() { Filename = fe.Path, Length = fe.Size, Start = fe.Offset });
            }
        }

        public byte[] RetrieveFile(SingleFileInfo info)
        {
            byte[] ret = new byte[info.Length];
            int startpiece = (int)(info.Start / piecelen);
            int startoff = (int)(info.Start % piecelen);
            long remaining = info.Length;
            if (remaining == 0) return ret;
            unsafe
            {
                fixed(byte* ret_ = &ret[0])
                {
                    sbyte* sret = (sbyte*)ret_;
                    while (remaining>0)
                    {
                        int cread = 0;
                        if (remaining >= piecelen - startoff)
                            cread = piecelen - startoff;
                        else
                            cread = (int)remaining;
                        if (cread != Read(sret, startpiece, startoff, cread))
                            throw new InvalidOperationException("File not finished");

                        startpiece++;
                        remaining -= cread;
                        sret += cread;
                    }
                }
            }
            return ret;
        }

        internal static ZipMemoryStorage GetInstance(FileStorage real, string basedir, SHA1Hash hash)
        {
            var zip = new ZipMemoryStorage();
            Storages.Instance.Add(hash.ToHex(), zip);
            zip.stor = real;
            zip.FillStorage(real);

            zip.basedir = basedir;
            zip.hash = hash;
            zip.Load();
            return zip;
        }
    }
}
