#pragma once

class BaseController {
 private:
  bool justActivated = false;

 public:
  void setAsActive() { justActivated = true; }

  virtual void run() {
    if (justActivated) {
      activate();
    }
    loop(justActivated);
    justActivated = false;
  }

 protected:
  virtual void activate() = 0;
  virtual void loop(const bool justActivated) = 0;
};
