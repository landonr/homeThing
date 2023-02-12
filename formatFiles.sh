echo "formatting..."
clang-format --style=Google -i homeThing/*.h homeThing/*.cpp components/*/*.h components/*/*.cpp
echo "formatted"
