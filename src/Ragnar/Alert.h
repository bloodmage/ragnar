#pragma once

namespace libtorrent
{
    class alert;
}

namespace Ragnar
{
    using namespace System;

    public ref class Alert
    {
    private:
        String^ _message;
		array<byte, 1>^ _raw;
    internal:
        Alert(libtorrent::alert* alert);

    public:
        property String^ Message { String^ get(); }
		property array<byte>^ Raw{array<byte>^ get(); }
    };
}