#pragma once

#include <boost/asio/ip/address.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <libtorrent\alloca.hpp>
#include <libtorrent\bencode.hpp>
#include <libtorrent\peer_id.hpp>
#include <libtorrent\time.hpp>
#include <cliext\vector>
#include <vector>

using namespace System;
using namespace System::Net;
namespace libtorrent
{
	typedef __int64 size_type;
}
namespace Ragnar
{
    public ref class Utils
    {
	public:
		static System::Text::Encoding^ windows = System::Text::Encoding::GetEncoding("Windows-1252");
		static System::Text::Encoding^ local = System::Text::Encoding::Default;
        static cli::array<unsigned char>^ GetByteArrayFromLibtorrentEntry(const libtorrent::entry &entry)
        {
            std::vector<unsigned char> buffer;
            libtorrent::bencode(std::back_inserter(buffer), entry);

            cli::array<unsigned char>^ result = gcnew cli::array<unsigned char>(buffer.size());

            for (int i = 0; i < result->Length; i++)
            {
                result[i] = buffer[i];
            }

            return result;
        }

        static TimeSpan^ GetTimeSpanFromPosixTimeDuration(boost::posix_time::time_duration const &t)
        {
            std::tm x = to_tm(t);
            return gcnew TimeSpan(x.tm_hour, x.tm_min, x.tm_sec);
        }

        static libtorrent::sha1_hash GetSha1HashFromString(String^ str)
        {
            libtorrent::sha1_hash hash;
			auto stdstr = GetStdStringFromManagedString(str);
			const char* ptr = stdstr.c_str();

            libtorrent::from_hex(ptr, str->Length, (char*)&hash[0]);

            return hash;
        }

		static array<byte, 1>^ ParseHex(String^ str)
		{
			auto out = gcnew array<byte, 1>((str->Length + 1) / 2);
			pin_ptr<byte> out_ = &out[0];

			auto stdstr = GetStdStringFromManagedString(str);
			const char* ptr = stdstr.c_str();

			libtorrent::from_hex(ptr, str->Length, (char*)out_);
			return out;
		}

		static System::String^ ToHex(array<byte, 1>^ bin)
		{
			if (bin->Length == 0) return gcnew System::String("");

			char*buf = TORRENT_ALLOCA(char, bin->Length * 2 + 1);
			pin_ptr<byte> bin_ = &bin[0];
			std::string sbin((char*)bin_, bin->Length);
			std::string o(libtorrent::to_hex(sbin));

			return gcnew System::String(o.c_str());
		}

		static String^ GetManagedStringFromStandardString(const std::string str)
		{
			System::String^ bad = gcnew System::String(str.c_str(), 0, str.size());
			array<byte, 1>^ data = windows->GetBytes(bad);

			System::String^ fixed = local->GetString(data);

			delete bad;
			delete data;

			return fixed;
		}
		
        static std::string GetStdStringFromManagedString(String^ str)
        {
            if (str->Length == 0)
                return std::string();

            cli::array<System::Byte> ^arr = System::Text::Encoding::Convert(
                System::Text::Encoding::Unicode,
                System::Text::Encoding::UTF8,
                System::Text::Encoding::Unicode->GetBytes(str)
                );

            cli::pin_ptr<unsigned char> ptr = &arr[0];
            const char *data = (const char*)(const unsigned char*)ptr;

            return std::string(data);
        }

        static System::Net::IPAddress^ GetIPAddress(boost::asio::ip::address const &address)
        {
			auto o(address.to_string());
            return IPAddress::Parse(gcnew String(o.c_str()));
        }

        static DateTime GetDateTimeFromTimeT(time_t time)
        {
            DateTime dt = DateTime(1970, 1, 1, 0, 0, 0, 0, System::DateTimeKind::Utc);
            dt = dt.AddSeconds((double)time);
            dt = dt.ToLocalTime();

            return dt;
        }
    };
}