#ifndef DISPLAYUPDATEINTERFACE
#define DISPLAYUPDATEINTERFACE

class DisplayUpdateInterface
{
  public:
  virtual void updateDisplay(bool force) = 0;
};

#endif