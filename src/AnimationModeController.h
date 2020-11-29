#include "BaseControllers.h"
#include "MilliTimer.h"
#include "animations/animations.h"
#include <Button.h>
#include <MasterState.h>

class AnimationModeController : public BaseController {
 private:
  Button & button;
  MasterState masterState;
  uint8_t animationIndex = 0;
  MilliTimer radioTimer{};

 public:
  AnimationModeController(Button & button, MasterState & masterState)
      : button(button)
      , masterState(masterState) {}

 protected:
  virtual void activate() override {}

  virtual void loop(const bool justActivated) override {
    bool animationIndexChanged = false;

    if (button.hasClickEventOccurred(ClickEvent::CLICK)) {
      masterState.manualUpdateIncrementAnimation();
    }

    Animation * currentAnimation = animations[animationIndex];

    if (animationIndexChanged || justActivated) {
      currentAnimation->setAsActive();
    }

    currentAnimation->run();
  }
};