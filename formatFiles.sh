echo "formatting..."
clang-format --style=file:.clang-format -i esphomeRemote/*.h esphomeRemote/*.cpp components/*/*.h components/*/*.cpp
echo "formatted"
