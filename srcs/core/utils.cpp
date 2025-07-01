#include <string>
#include <vector>

namespace Zappy {

std::string join(const std::vector<std::string>& vec, const std::string& separator) {
    if (vec.empty()) return "";

    std::string result = vec.front();
    for (size_t i = 1; i < vec.size(); ++i) {
        result += separator + vec[i];
    }
    return result;
}

}