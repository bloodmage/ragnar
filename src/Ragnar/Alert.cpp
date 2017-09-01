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
		this->_raw = Utils::GetManagedBinaryFromStandardString(msg);
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