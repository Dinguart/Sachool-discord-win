#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <stdio.h>
#include <optional>

using fileMap = std::unordered_map<std::string, std::string>;

std::optional<fileMap> readFile(const std::string file);