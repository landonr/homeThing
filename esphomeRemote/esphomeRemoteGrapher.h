#include <memory>
#include <string>
#include <vector>
#include "esphome.h"

#pragma once

class Graph {
 public:
     explicit Graph(DisplayBuffer *db, const std::vector<float> &data){
         _raw_data = data;
         _db = db;
         processData();
     }

    void processData(){
    int samples = _raw_data.size();
    int items_per_bin = samples > _bin_size ? std::ceil(samples / _bin_size) : 1;
    std::vector<float> binned_data(_bin_size);

    // are there less samples than bins?
    if (samples < _bin_size){
        binned_data = _raw_data;
    } else{
        int current_bin = 0;
        for (int sample = 0; sample < samples; ++sample) {
            int sum = 0;
            int tempSample = sample;
            for (int item = 0; item < items_per_bin; ++item) {
                if(tempSample < _raw_data.size()){
                    sum += _raw_data[tempSample];
                }else{
                    break;
                }
            }
            sample = sample + (tempSample - sample);
            if (current_bin < _bin_size) {
                binned_data.push_back(float(sum / items_per_bin)); // TODO replace c-cast
            } else {
            }
            ++current_bin;
        }
    }
    _binned_data = binned_data;
}
    void draw(){
        ESP_LOGD("Graph", "enter draw()");
        int x_offset = 4;
        int y_offset = 38;
        int width = 230;
        int height = 90;
        id(_db).horizontal_line(x_offset, y_offset, width, _axisColor);
        id(_db).horizontal_line(x_offset, y_offset + height - 1, width, _axisColor);
        id(_db).vertical_line(x_offset, y_offset, height, _axisColor);
        id(_db).vertical_line(x_offset + width - 1, y_offset, height, _axisColor);
        float ymin = 0;
        float ymax = 6;
        float yrange = ymax - ymin;
        Color c = _graphColor;  

        y_offset = 110;
        int oldx = 0;
        int oldy = 0;
        ESP_LOGD("Graph", "start line drawing");
        for (int i = 0; i < _binned_data.size(); ++i) {
            int x2 = x_offset + i;
            int y2 = y_offset - _binned_data[i];
            if (oldx == 0) {
                oldx = x2;
                oldy = y2;
            }
            id(_db).line(oldx, oldy, x2, y2, c);
            oldx = x2;
            oldy = y2;
    }
    };


 private:
  // tdisplay res 240x135
  int _bin_size = 240 - 10; // TODO make display resolution global var per device
  Color _graphColor = id(my_green);
  Color _axisColor = id(my_blue);
  std::vector<float> _raw_data;
  std::vector<float> _binned_data;
  DisplayBuffer *_db = nullptr;

};
