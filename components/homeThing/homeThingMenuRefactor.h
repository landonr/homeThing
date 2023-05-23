
#pragma once
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#ifdef USE_LIGHT_GROUP
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#endif

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#ifdef USE_SENSOR_GROUP
#include "esphome/components/homeassistant_sensor_group/HomeAssistantSensorGroup.h"
#endif

#ifdef USE_SERVICE_GROUP
#include "esphome/components/homeassistant_service_group/HomeAssistantServiceGroup.h"
#endif

#ifdef USE_SWITCH_GROUP
#include "esphome/components/homeassistant_switch_group/HomeAssistantSwitchGroup.h"
#endif

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuRefactor {
 public:
  HomeThingMenuRefactor(display::DisplayBuffer* new_display_buffer,
                        HomeThingMenuDisplayState* new_display_state,
                        HomeThingMenuTextHelpers* new_text_helpers)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        text_helpers_(new_text_helpers) {}
  // move to menu items
  void drawGroupedBar(int yPos, bool extend);
  void drawLightSliderBar(int xPos, int yPos, int sliderHeight,
                          SliderSelectionState sliderState,
                          std::shared_ptr<MenuTitleSlider> slider,
                          bool drawRGB);
  void drawLightSliderRGBBar(int xPos, int yPos, int sliderHeight,
                             int slider_width);
  void drawLightSlider(int xPos, int yPos, SliderSelectionState sliderState,
                       std::shared_ptr<MenuTitleSlider> slider, bool drawRGB);
  void drawLightSwitch(bool switchState, int yPos, Color lightColor,
                       bool rowSelected);
  void drawLightSliderCircle(int xPos, int yPos, int sliderHeight,
                             SliderSelectionState sliderState,
                             std::shared_ptr<MenuTitleSlider> slider,
                             bool drawRGB);
  void drawLightSliderTitle(int xPos, int yPos, int sliderHeight,
                            SliderSelectionState sliderState,
                            std::shared_ptr<MenuTitleSlider> slider,
                            bool drawRGB);
  void drawSwitch(bool switchState, int yPos);
  void drawArrow(int yPos, int menuTitlesCount, int maxItems);

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuTextHelpers* text_helpers_{nullptr};
  const char* const TAG = "homething.menu.refactor";
};
}  // namespace homething_menu_base
}  // namespace esphome
