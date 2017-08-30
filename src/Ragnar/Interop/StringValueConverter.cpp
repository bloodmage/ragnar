#include "Stdafx.h"
#include "StringValueConverter.h"
#include "..\Utils.h"

#include <msclr\marshal_cppstd.h>
#include <string>

namespace Ragnar
{
    namespace Interop
    {
        std::string StringValueConverter::To(System::String^ value)
        {
            return msclr::interop::marshal_as<std::string>(value);
        }

        System::String^ StringValueConverter::From(const std::string &value)
        {
			return Utils::GetManagedStringFromStandardString(value);
        }
    }
}
