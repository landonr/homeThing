
#pragma once
#include "esphome.h"
#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include <memory>
#include "graph.h"
#include <random>

class GraphHT {
 public:
     explicit GraphHT(DisplayBuffer *db/*, const std::vector<float> &data*/){
         // _raw_data = data;
         _db = db;

     }

     void setInitialData(const std::vector<float> &data){
         _data = data;
          _currentSize = _data.size();
          _data.resize(id(my_display).get_width(), NAN);
        ESP_LOGD("Graph", "value 0: %f", _data[0]);
     }

    void prepareGraph(){
        ESP_LOGD("Graph", "prepare graph");
        ESP_LOGD("Graph", "initializing _single_temperature_graph object");
        _single_temperature_graph = new esphome::graph::Graph();
        _single_temperature_graph->set_duration(3600);
        _single_temperature_graph->set_width(id(my_display).get_width());
        _single_temperature_graph->set_height(100);
        _single_temperature_graph->set_component_source("graph");
        // App.register_component(_single_temperature_graph);
        // _single_temperature_graph->set_grid_x(20);
        // _single_temperature_graph->set_grid_y(1.0f);
        // _single_temperature_graph->set_max_range(30);
        ESP_LOGD("Graph", "done prepare graph");
    }
    void prepareGraphTrace(){
        ESP_LOGD("Graph", "Prepare GraphTrace");
        ESP_LOGD("Graph", "initializing gt object");
        _graph_graphtrace = new esphome::graph::GraphTrace(esphome::graph::GraphTrace());
        _graph_graphtrace->set_sensor(at_down_bathroom_1_temperature);
        _graph_graphtrace->set_name("at_down_bathroom_1_temperature");
        // _graph_graphtrace->set_line_thickness(2);
        // _graph_graphtrace->set_line_type(esphome::graph::LINE_TYPE_SOLID);
        // _graph_graphtrace->set_line_color(id(my_red));
        ESP_LOGD("Graph", "done prepare GraphTrace");
    }

    void drawGraph(){
      ESP_LOGD("Graph", "enter draw()");
      // first time drawing intialize graph
      if (!_graphInitialized){
          _graphInitialized = true;
          prepareGraph();
          prepareGraphTrace();
          
          _nhd.init(id(my_display).get_width());
          _nhd.setData(_data, _currentSize);
          _nhd.set_update_time_ms(_single_temperature_graph->get_duration() * 1000 / _single_temperature_graph->get_width());

          _nhd.take_sample(21.0);
          ESP_LOGD("Graph", "add tracedata");
          _graph_graphtrace->set_tracedata(_nhd);
          ESP_LOGD("Graph", "added tracedata");
          _single_temperature_graph->add_trace(_graph_graphtrace);
          _single_temperature_graph->draw(_db, _xOffset, _yOffset, id(color_accent_primary));
      }else{
          _single_temperature_graph->draw(_db, _xOffset, _yOffset, id(color_accent_primary));

      }
    };

    void addValue(float val){
          _single_temperature_graph->addValue(val);
    }


 private:
  // tdisplay res 240x135
  DisplayBuffer *_db = nullptr;
  esphome::graph::Graph *_single_temperature_graph;
  esphome::graph::GraphTrace *_graph_graphtrace;
  esphome::graph::HistoryData _nhd ;
  bool _graphInitialized = false;
  float value_{0};
  std::vector<float> _data  {};
  int _currentSize = 0;
  uint16_t _xOffset = 5;
  uint16_t _yOffset = 30;
    std::random_device _rd;
};
