#pragma once
#include "BaseControllers.h"
#include "MilliTimer.h"
#include "animations/animations.h"
#include <Button.h>
#include <MasterState.h>
#include "animations/Crossfade.h"

class AnimationModeController : public BaseController {
 private:
  LSButton & button;
  MasterState * masterState;
  uint8_t animationIndex = 0;
  MilliTimer radioTimer{};
  Animation * animations[NUMBER_OF_ANMIATIONS];

 public:
  AnimationModeController(LSButton & button, MasterState * masterState)
      : button(button)
      , masterState(masterState) {

    Crossfade crossfade(masterState);
    *animations = {&crossfade};
      }

  void setup(){
    Crossfade crossfade(masterState);
    *animations = {&crossfade};
  }
 protected:
  virtual void activate() override {}

  virtual void loop(const bool justActivated) override {
    bool animationIndexChanged = false;

    if (button.hasClickEventOccurred(ClickEvent::CLICK)) {
      masterState->manualUpdateIncrementAnimation();
    }

    Animation * currentAnimation = animations[animationIndex];

    if (animationIndexChanged || justActivated) {
      currentAnimation->setAsActive();
    }

    currentAnimation->run();
  }


};