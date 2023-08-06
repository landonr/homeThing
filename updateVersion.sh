#!/bin/bash

# Called by "git commit" with no arguments.  The hook should
# exit with non-zero status after issuing an appropriate message if
# it wants to stop the commit.

versionfilename="components/homeThing/version.h"

# Version number
#echo \# Generated using git pre-commit hook > $versionfilename
echo -e "#pragma #once \n#include <string> \n\nnamespace esphome { \nnamespace homething_menu_base { \n    static std::string COMPONENTS_HOMETHING_VERSION_H = \"" > $versionfilename

echo -n $(git rev-parse --abbrev-ref HEAD) > $versionfilename
echo -n "-" >> $versionfilename
echo $(git log --pretty=oneline | wc -l) >> $versionfilename

echo -e "\"; \n} \n} \n"

exec git add $versionfilename
