#pragma once

class Crossfade : public Animation {
 private:
  MasterState * masterState;
  MilliTimer timer{};
  uint8_t hue = 0;

 public:
  Crossfade(MasterState * masterState_)
      : masterState(masterState_) {}

 protected:
  void activate() override { masterState->knob->activate(35, 5, 200, false); }

  bool updateAnimation(const bool justActivated) override {
    masterState->knob->update();

    if (timer.hasElapsedWithReset(masterState->knob->get()) || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
      return true;
    }

    return false;
  }
};
