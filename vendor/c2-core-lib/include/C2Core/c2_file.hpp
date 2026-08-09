#pragma once

#include <string>
#include <string_view>


namespace C2Core::File {
    std::string ReadText(std::string_view filepath);
}
