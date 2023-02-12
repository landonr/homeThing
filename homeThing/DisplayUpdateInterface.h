#pragma once

class DisplayUpdateInterface {
 public:
  virtual void updateDisplay(bool force) = 0;
};
