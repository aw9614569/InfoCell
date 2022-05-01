#pragma once

#include <nlohmann/json.hpp>

#include "ArcTask.h"

namespace synth {

ArcTask ConvertJsonToCell(const nlohmann::json& jsonArcFile);

} // namespace synth