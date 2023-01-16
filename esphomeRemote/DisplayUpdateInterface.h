#pragma once

class DisplayUpdateInterface {
 public:
  virtual void updateDisplay(bool force) = 0;
  virtual void setUpdateInterval(int updateInterval) = 0;
};
