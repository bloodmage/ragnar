#pragma once
#include "libtorrent/export.hpp"
#include <boost/cstdint.hpp>
typedef boost::uint32_t u32;
typedef boost::uint8_t u8;

namespace libtorrent
{
	struct TORRENT_EXPORT sha_ctx
	{
		u32 state[5];
		u32 count[2];
		u8 buffer[64];
	};

	// we don't want these to clash with openssl's libcrypto
	TORRENT_EXPORT void SHA1_init(sha_ctx* context);
	TORRENT_EXPORT void SHA1_update(sha_ctx* context, u8 const* data, u32 len);
	TORRENT_EXPORT void SHA1_final(u8* digest, sha_ctx* context);

}

namespace Ragnar
{
	namespace Unsafe
	{
		public ref class SHA1
		{
		private:
			libtorrent::sha_ctx* ptr;
			array<u8>^ final;
		public:
			SHA1() {
				ptr = new libtorrent::sha_ctx();
				libtorrent::SHA1_init(ptr);
			}
			SHA1(array<u8>^ data, int ptr, int len) : SHA1()
			{
				if (ptr < 0 || ptr > data->Length) throw gcnew System::ArgumentException("ptr out of range");
				if (len<0 || ptr + len> data->Length) throw gcnew System::ArgumentOutOfRangeException("len out of range");
				pin_ptr<u8> d_ = &data[0];
				Update((u8*)d_ + ptr, len);
			}
			SHA1(u8 const* data, u32 len) : SHA1() { Update(data, len); }
			~SHA1() {
				if (ptr != NULL) { delete ptr; ptr = NULL; }
			}
			!SHA1() { this->~SHA1(); }
			void Update(array<u8>^ data, int ptr, int len)
			{
				if (ptr < 0 || ptr > data->Length) throw gcnew System::ArgumentException("ptr out of range");
				if (len<0 || ptr + len> data->Length) throw gcnew System::ArgumentOutOfRangeException("len out of range");
				pin_ptr<u8> d_ = &data[0];
				Update((u8*)d_ + ptr, len);
			}
			void Update(u8 const* data, u32 len) {
				if (ptr == NULL) throw gcnew System::InvalidOperationException("Already finalized");
				libtorrent::SHA1_update(ptr, data, len);
			}

			array<u8>^ Final() {
				if (ptr != NULL){
					final = gcnew array<u8>(20);
					pin_ptr<u8> f_ = &final[0];
					libtorrent::SHA1_final(f_, ptr);
					this->~SHA1();
				}
				return final;
			}
		};
	}
}