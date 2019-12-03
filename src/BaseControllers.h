#ifndef BaseControllers_H
#define BaseControllers_H

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
  virtual void loop(bool justActivated) = 0;
};


#endif // BaseControllers_H