
class MasterState {
 private:
  int numberOfAnimations;
  int animationIndex;
  int knobSetting;
  bool manuallySet;

 public:
  MasterState(int numberOfAnimations)
      : numberOfAnimations(numberOfAnimations)
      , animationIndex(0)
      , knobSetting(0)
      , manuallySet(false) {}

  void manualUpdateIncrementAnimation() {
    animationIndex =
        (animationIndex == numberOfAnimations - 1) ? 1 : animationIndex + 1;
    manuallySet = true;
  }

  void manuallyChangeKnobSetting(int newKnobSetting) {
    knobSetting = newKnobSetting;
    manuallySet = true;
  }

  void markAsApplied() { manuallySet = false; }
};