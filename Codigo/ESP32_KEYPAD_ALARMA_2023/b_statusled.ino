void BlendAnimUpdate(const AnimationParam& param) {
  // this gets called for each animation on every time step
  // progress will start at 0.0 and end at 1.0
  // we use the blend function on the RgbColor to mix
  // color based on the progress given to us in the animation
  float progress = NeoEase::SinusoidalInOut(param.progress);

  RgbColor updatedColor = RgbColor::LinearBlend(
    animationState[param.index].StartingColor,
    animationState[param.index].EndingColor,
    progress);

  // apply the color to the strip
    pixels.SetPixelColor(param.index, updatedColor);
}


void led_update() {
  if (animations.IsAnimating()) {
    // the normal loop just needs these two to run the active animations
    animations.UpdateAnimations();
    pixels.Show();
  }
}
void set_led_color(byte q, int t, byte r, byte g, byte b) {

  RgbColor desiredcolor = RgbColor(r, g, b);

  desiredcolor = desiredcolor.Dim(MAX_BRIGHTNESS);

  animationState[q].StartingColor = pixels.GetPixelColor(q);;
  animationState[q].EndingColor = desiredcolor;
  animations.StartAnimation(q, t, BlendAnimUpdate);
}
