
#pragma once
#include <string>
#include "esphome/components/display/display.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingMenuTitleSlider.h"
#include "esphome/components/homeThing/homeThingMenuTitleToggle.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuRefactor {
 public:
  HomeThingMenuRefactor(
      display::Display* new_display,
      homething_display_state::HomeThingDisplayState* new_display_state)
      : display_(new_display),
        display_state_(new_display_state) {}
  // move to menu items
  void drawGroupedBar(int yPos, bool extend);
  void drawLightSliderBar(int xPos, int yPos, int sliderHeight,
                          SliderSelectionState sliderState,
                          const MenuTitleSlider* slider, bool drawRGB);
  void drawLightSliderRGBBar(int xPos, int yPos, int sliderHeight,
                             int slider_width);
  void drawLightSlider(int xPos, int yPos, SliderSelectionState sliderState,
                       const MenuTitleSlider* slider, bool drawRGB);
  void drawLightSwitch(bool switchState, int yPos, Color lightColor,
                       bool rowSelected);
  void drawLightSliderCircle(int xPos, int yPos, int sliderHeight,
                             SliderSelectionState sliderState,
                             const MenuTitleSlider* slider, bool drawRGB);
  void drawLightSliderTitle(int xPos, int yPos, int sliderHeight,
                            SliderSelectionState sliderState,
                            const MenuTitleSlider* slider, bool drawRGB);
  void drawSwitch(bool switchState, int yPos);
  void drawArrow(int yPos, int menuTitlesCount, int maxItems);

 private:
  display::Display* display_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  const char* const TAG = "homething.menu.refactor";
};
}  // namespace homething_menu_base
}  // namespace esphome
