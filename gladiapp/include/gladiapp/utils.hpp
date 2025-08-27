#pragma once
#include <sstream>
#include <iomanip>

namespace gladiapp
{
    namespace utils
    {
        /**
         * Formats the given byte size into a human-readable string.
         */
        std::string formatBytes(long long bytes)
        {
            const char *units[] = {"bytes", "Kb", "Mb", "Gb", "Tb", "Pb", "Eb"};
            int unitIndex = 0;
            double value = static_cast<double>(bytes);

            while (value >= 1024 && unitIndex < (sizeof(units) / sizeof(units[0])) - 1)
            {
                value /= 1024;
                unitIndex++;
            }

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value << " " << units[unitIndex];
            return ss.str();
        }
    }
}