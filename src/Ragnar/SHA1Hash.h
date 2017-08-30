#pragma once

namespace libtorrent
{
    class sha1_hash;
}

namespace Ragnar
{
    public ref class SHA1Hash
    {
    internal:
		libtorrent::sha1_hash* _hash;
		System::String^ hex;
		System::String^ str;
		SHA1Hash(const libtorrent::sha1_hash &hash);
		~SHA1Hash();
		!SHA1Hash();

    public:
		SHA1Hash(System::String^ lowcase_hex);
		SHA1Hash(array<byte>^ data);

        property bool IsZero { bool get(); }

        void Clear();

        System::String^ ToHex();
		const byte* Ptr();

        virtual System::String^ ToString() override;
    };
}
