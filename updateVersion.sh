#!/bin/bash

# Check if the number of arguments is correct
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <directory>"
  exit 1
fi

# Get the directory path from the first argument
directory="$1"

# Check if the directory exists
if [ ! -d "$directory" ]; then
  echo "Error: Directory '$directory' does not exist."
  exit 1
fi

# Get the current Git branch name
branch_name=$(git rev-parse --abbrev-ref HEAD)

# Get the current date in the format YYYY-MM-DD
current_date=$(date +%Y-%m-%d)

# Content to be written to the file with the current date and branch name
content="#ifndef COMPONENTS_HOMETHING_VERSION_H_
#define COMPONENTS_HOMETHING_VERSION_H_
#include <string>

namespace esphome {
namespace homething_menu_base {
static const std::string COMPONENTS_HOMETHING_VERSION = \"${branch_name}/${current_date}\";
}
}  // namespace esphome
#endif  // COMPONENTS_HOMETHING_VERSION_H_
"

# Generate a filename with a timestamp
timestamp=$(date +%Y%m%d%H%M%S)
filename="output_${timestamp}.txt"

# Combine the directory path and filename to get the full path
output_file="${directory}/${filename}"

# Write the content to the file
echo -e "$content" > "$output_file"

echo "Content has been written to $output_file."