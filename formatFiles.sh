echo "formatting..."
clang-format --style=Google -i homeThing/*.h homeThing/*.cpp components/*/*.h components/*/*.cpp components/*/*/*.h components/*/*/*.cpp
cpplint --filter=-legal/copyright,-build/include_subdir --recursive homeThing/ components/ 
echo "formatted"
