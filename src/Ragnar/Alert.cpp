#include "stdafx.h"
#include "Alert.h"
#include "Utils.h"

#include <libtorrent\alert.hpp>

namespace Ragnar
{
    using namespace System;

    Alert::Alert(libtorrent::alert* alert)
    {
		std::string msg(alert->message());
		this->_raw = gcnew array<byte>(msg.size());
		System::Runtime::InteropServices::Marshal::Copy((System::IntPtr)(void*)msg.c_str(), this->_raw, 0, msg.size());
		this->_message = System::Text::Encoding::UTF8->GetString(this->_raw);
	}
	array<byte>^ Alert::Raw::get()
	{
		return this->_raw;
	}
    String^ Alert::Message::get()
    {
        return this->_message;
    }
}