echo "formatting..."
clang-format esphomeRemote/*.h --style=file:.clang-format --i
echo "formatted"
esphome run m5stickc.yaml --no-logs
esphome run tdisplayt4.yaml --no-logs
esphome run tdisplayremote.yaml --no-logs
esphome run fireremote.yaml --no-logs
