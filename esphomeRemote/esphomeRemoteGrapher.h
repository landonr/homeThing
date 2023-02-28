
#pragma once
#include "esphome.h"
#include <memory>
#include <string>
#include <vector>
// #include <esphome/components/graph/graph.h>
#include "esphome/core/color.h"
// #include <newgraph.h> 
// #include "esphome/core/color.h"
#include "esphome/core/component.h"
#include <cstdint>
#include <utility>
#include "esphome/components/display/display_buffer.h"
// #include "esphome/core/color.h"
#include "esphome/core/log.h"
// #include "esphome/core/hal.h"
#include <algorithm>
#include <sstream>
#include <iostream>  // std::cout, std::fixed
#include <iomanip>
#include <memory>
#include "graph.h"

// namespace esphome {
//
// // forward declare DisplayBuffer
// namespace display {
// class DisplayBuffer;
// class Font;
// }  // namespace display
//
// namespace graph {
//
// class Graph;
//
// const Color COLOR_ON(255, 255, 255, 255);
//
// /// Bit pattern defines the line-type
// enum LineType {
//   LINE_TYPE_SOLID = 0b1111,
//   LINE_TYPE_DOTTED = 0b0101,
//   LINE_TYPE_DASHED = 0b1110,
//   // Following defines number of bits used to define line pattern
//   PATTERN_LENGTH = 4
// };
//
// enum DirectionType {
//   DIRECTION_TYPE_AUTO,
//   DIRECTION_TYPE_HORIZONTAL,
//   DIRECTION_TYPE_VERTICAL,
// };
//
// enum ValuePositionType {
//   VALUE_POSITION_TYPE_NONE,
//   VALUE_POSITION_TYPE_AUTO,
//   VALUE_POSITION_TYPE_BESIDE,
//   VALUE_POSITION_TYPE_BELOW
// };
//
// class GraphLegend {
//  public:
//   void init(Graph *g);
//   void set_name_font(display::Font *font) { this->font_label_ = font; }
//   void set_value_font(display::Font *font) { this->font_value_ = font; }
//   void set_width(uint32_t width) { this->width_ = width; }
//   void set_height(uint32_t height) { this->height_ = height; }
//   void set_border(bool val) { this->border_ = val; }
//   void set_lines(bool val) { this->lines_ = val; }
//   void set_values(ValuePositionType val) { this->values_ = val; }
//   void set_units(bool val) { this->units_ = val; }
//   void set_direction(DirectionType val) { this->direction_ = val; }
//
//  protected:
//   uint32_t width_{0};
//   uint32_t height_{0};
//   bool border_{true};
//   bool lines_{true};
//   ValuePositionType values_{VALUE_POSITION_TYPE_AUTO};
//   bool units_{true};
//   DirectionType direction_{DIRECTION_TYPE_AUTO};
//   display::Font *font_label_{nullptr};
//   display::Font *font_value_{nullptr};
//   // Calculated values
//   Graph *parent_{nullptr};
//   //                      (x0)          (xs,ys)         (xs,ys)
//   // <x_offset,y_offset> ------> LABEL1 -------> LABEL2 -------> ...
//   //                                | \(xv,yv)        \ .
//   //                                |  \               \-> VALUE1+units
//   //                          (0,yl)|   \-> VALUE1+units
//   //                                v     (top_center)
//   //                            LINE_SAMPLE
//   int x0_{0};  // X-offset to centre of label text
//   int xs_{0};  // X spacing between labels
//   int ys_{0};  // Y spacing between labels
//   int yl_{0};  // Y spacing from label to line sample
//   int xv_{0};  // X distance between label to value text
//   int yv_{0};  // Y distance between label to value text
//   friend Graph;
// };
//
// class HistoryData {
//  public:
//   void init(int length);
//   // ~HistoryData();
//   void set_update_time_ms(uint32_t update_time_ms) { update_time_ = update_time_ms; }
//   void take_sample(float data);
//   int get_length() const { return length_; }
//   float get_value(int idx) const { return samples_[(count_ + length_ - 1 - idx) % length_]; }
//   float get_recent_max() const { return recent_max_; }
//   float get_recent_min() const { return recent_min_; }
//
//  protected:
//   uint32_t last_sample_;
//   uint32_t period_{0};       /// in ms
//   uint32_t update_time_{0};  /// in ms
//   int length_;
//   int count_{0};
//   float recent_min_{NAN};
//   float recent_max_{NAN};
//   std::vector<float> samples_;
// };
//
// class GraphTrace {
//  public:
//   void init(Graph *g);
//   void set_name(std::string name) { name_ = std::move(name); }
//   void set_sensor(sensor::Sensor *sensor) { sensor_ = sensor; }
//   uint8_t get_line_thickness() { return this->line_thickness_; }
//   void set_line_thickness(uint8_t val) { this->line_thickness_ = val; }
//   enum LineType get_line_type() { return this->line_type_; }
//   void set_line_type(enum LineType val) { this->line_type_ = val; }
//   Color get_line_color() { return this->line_color_; }
//   void set_line_color(Color val) { this->line_color_ = val; }
//   std::string get_name() { return name_; }
//   const HistoryData *get_tracedata() { return &data_; }
//   void addValue(float val){data_.take_sample(val);};
//
//  protected:
//   sensor::Sensor *sensor_{nullptr};
//   std::string name_{""};
//   uint8_t line_thickness_{3};
//   enum LineType line_type_ { LINE_TYPE_SOLID };
//   Color line_color_{COLOR_ON};
//   HistoryData data_;
//
//   friend Graph;
//   friend GraphLegend;
// };
//
// class Graph : public Component {
//  public:
//   void draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color);
//   void draw_legend(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color);
//
//   void setup() override;
//   float get_setup_priority() const override { return setup_priority::PROCESSOR; }
//   void dump_config() override;
//
//   void set_duration(uint32_t duration) { duration_ = duration; }
//   void set_width(uint32_t width) { width_ = width; }
//   void set_height(uint32_t height) { height_ = height; }
//   void set_min_value(float val) { this->min_value_ = val; }
//   void set_max_value(float val) { this->max_value_ = val; }
//   void set_min_range(float val) { this->min_range_ = val; }
//   void set_max_range(float val) { this->max_range_ = val; }
//   void set_grid_x(float val) { this->gridspacing_x_ = val; }
//   void set_grid_y(float val) { this->gridspacing_y_ = val; }
//   void set_border(bool val) { this->border_ = val; }
//   void add_trace(GraphTrace *trace) { traces_.push_back(trace); }
//   void add_legend(GraphLegend *legend) {
//     this->legend_ = legend;
//     legend->init(this);
//   }
//   uint32_t get_duration() { return duration_; }
//   uint32_t get_width() { return width_; }
//   uint32_t get_height() { return height_; }
//   void addValue(float val){traces_[0]->addValue(val);}
//
//  protected:
//   uint32_t duration_;  /// in seconds
//   uint32_t width_;     /// in pixels
//   uint32_t height_;    /// in pixels
//   float min_value_{NAN};
//   float max_value_{NAN};
//   float min_range_{1.0};
//   float max_range_{NAN};
//   float gridspacing_x_{NAN};
//   float gridspacing_y_{NAN};
//   bool border_{true};
//   std::vector<GraphTrace *> traces_;
//   GraphLegend *legend_{nullptr};
//
//   friend GraphLegend;
// };
//
// }  // namespace graph
// }  // namespace esphome
//
// namespace esphome {
// namespace graph {
//
// using namespace display;
//
// static const char *const TAG = "graph";
// static const char *const TAGL = "graphlegend";
//
// void HistoryData::init(int length) {
//   this->length_ = length;
//   this->samples_.resize(length, NAN);
//   this->last_sample_ = millis();
// }
//
// void HistoryData::take_sample(float data) {
//   uint32_t tm = millis();
//   uint32_t dt = tm - last_sample_;
//   last_sample_ = tm;
//
//   // Step data based on time
//   this->period_ += dt;
//     ESP_LOGD("TS", "hello ");
//     // ESP_LOGD("TS", "hello1");
//     // ESP_LOGD("TS", "hello %i ", this->period_);
//     // ESP_LOGD("TS", "hello %i ",  this->update_time_);
//   while (this->period_ >= this->update_time_) {
//     // ESP_LOGD("TS", "hello2");
//     // ESP_LOGD("TS", "hello %i ", this->count_);
//     // ESP_LOGD("TS", "hello  %f",  data);
//     this->samples_[this->count_] = data;
//     // ESP_LOGD("TS", "hello3");
//     // ESP_LOGD("TS", "hello %i ", this->count_);
//     ESP_LOGD("TS", "hello  %f",  data);
//     this->period_ -= this->update_time_;
//     this->count_ = (this->count_ + 1) % this->length_;
//     ESP_LOGD("TS", "hello  %f",  data);
//     ESP_LOGV(TAG, "Updating trace with value: %f", data);
//   }
//   if (!std::isnan(data)) {
//     // Recalc recent max/min
//     this->recent_min_ = data;
//     this->recent_max_ = data;
//     for (int i = 0; i < this->length_; i++) {
//       if (!std::isnan(this->samples_[i])) {
//         if (this->recent_max_ < this->samples_[i])
//           this->recent_max_ = this->samples_[i];
//         if (this->recent_min_ > this->samples_[i])
//           this->recent_min_ = this->samples_[i];
//       }
//     }
//   }
// }
//
// void GraphTrace::init(Graph *g) {
//   ESP_LOGI(TAG, "Init trace for sensor %s", this->get_name().c_str());
//   this->data_.init(g->get_width());
//   sensor_->add_on_state_callback([this](float state) { this->data_.take_sample(state); });
//   this->data_.set_update_time_ms(g->get_duration() * 1000 / g->get_width());
// }
//
// void Graph::draw(DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color) {
//
//   /// Plot border
//   if (this->border_) {
//     buff->horizontal_line(x_offset, y_offset, this->width_, color);
//     buff->horizontal_line(x_offset, y_offset + this->height_ - 1, this->width_, color);
//     buff->vertical_line(x_offset, y_offset, this->height_, color);
//     buff->vertical_line(x_offset + this->width_ - 1, y_offset, this->height_, color);
//   }
//   /// Determine best y-axis scale and range
//   float ymin = NAN;
//   float ymax = NAN;
//   for (auto *trace : traces_) {
//     float mx = trace->get_tracedata()->get_recent_max();
//     float mn = trace->get_tracedata()->get_recent_min();
//     if (std::isnan(ymax) || (ymax < mx))
//       ymax = mx;
//     if (std::isnan(ymin) || (ymin > mn))
//       ymin = mn;
//   }
//   // Adjust if manually overridden
//   if (!std::isnan(this->min_value_))
//     ymin = this->min_value_;
//   if (!std::isnan(this->max_value_))
//     ymax = this->max_value_;
//
//   float yrange = ymax - ymin;
//   if (yrange > this->max_range_) {
//     // Look back in trace data to best-fit into local range
//     float mx = NAN;
//     float mn = NAN;
//     for (uint32_t i = 0; i < this->width_; i++) {
//       for (auto *trace : traces_) {
//         float v = trace->get_tracedata()->get_value(i);
//         if (!std::isnan(v)) {
//           if ((v - mn) > this->max_range_)
//             break;
//           if ((mx - v) > this->max_range_)
//             break;
//           if (std::isnan(mx) || (v > mx))
//             mx = v;
//           if (std::isnan(mn) || (v < mn))
//             mn = v;
//         }
//       }
//     }
//     yrange = this->max_range_;
//     if (!std::isnan(mn)) {
//       ymin = mn;
//       ymax = ymin + this->max_range_;
//     }
//     ESP_LOGV(TAG, "Graphing at max_range. Using local min %f, max %f", mn, mx);
//   }
//
//   float y_per_div = this->min_range_;
//   if (!std::isnan(this->gridspacing_y_)) {
//     y_per_div = this->gridspacing_y_;
//   }
//   // Restrict drawing too many gridlines
//   if (yrange > 10 * y_per_div) {
//     while (yrange > 10 * y_per_div) {
//       y_per_div *= 2;
//     }
//     ESP_LOGW(TAG, "Graphing reducing y-scale to prevent too many gridlines");
//   }
//
//   // Adjust limits to nice y_per_div boundaries
//   int yn = int(ymin / y_per_div);
//   int ym = int(ymax / y_per_div) + int(1 * (fmodf(ymax, y_per_div) != 0));
//   ymin = yn * y_per_div;
//   ymax = ym * y_per_div;
//   yrange = ymax - ymin;
//
//   /// Draw grid
//   if (!std::isnan(this->gridspacing_y_)) {
//     for (int y = yn; y <= ym; y++) {
//       int16_t py = (int16_t) roundf((this->height_ - 1) * (1.0 - (float) (y - yn) / (ym - yn)));
//       for (uint32_t x = 0; x < this->width_; x += 2) {
//         buff->draw_pixel_at(x_offset + x, y_offset + py, color);
//       }
//     }
//   }
//   if (!std::isnan(this->gridspacing_x_) && (this->gridspacing_x_ > 0)) {
//     int n = this->duration_ / this->gridspacing_x_;
//     // Restrict drawing too many gridlines
//     if (n > 20) {
//       while (n > 20) {
//         n /= 2;
//       }
//       ESP_LOGW(TAG, "Graphing reducing x-scale to prevent too many gridlines");
//     }
//     for (int i = 0; i <= n; i++) {
//       for (uint32_t y = 0; y < this->height_; y += 2) {
//         buff->draw_pixel_at(x_offset + i * (this->width_ - 1) / n, y_offset + y, color);
//       }
//     }
//   }
//
//   /// Draw traces
//   ESP_LOGV(TAG, "Updating graph. ymin %f, ymax %f", ymin, ymax);
//   for (auto *trace : traces_) {
//     Color c = trace->get_line_color();
//     uint16_t thick = trace->get_line_thickness();
//     for (uint32_t i = 0; i < this->width_; i++) {
//       float v = (trace->get_tracedata()->get_value(i) - ymin) / yrange;
//       if (!std::isnan(v) && (thick > 0)) {
//         int16_t x = this->width_ - 1 - i;
//         uint8_t b = (i % (thick * LineType::PATTERN_LENGTH)) / thick;
//         if (((uint8_t) trace->get_line_type() & (1 << b)) == (1 << b)) {
//           int16_t y = (int16_t) roundf((this->height_ - 1) * (1.0 - v)) - thick / 2;
//           for (uint16_t t = 0; t < thick; t++) {
//             buff->draw_pixel_at(x_offset + x, y_offset + y + t, c);
//           }
//         }
//       }
//     }
//   }
// }
//
//
// /// Determine the best coordinates of drawing text + lines
// void GraphLegend::init(Graph *g) {
//   parent_ = g;
//
//   // Determine maximum expected text and value width / height
//   int txtw = 0, txth = 0;
//   int valw = 0, valh = 0;
//   int lt = 0;
//   for (auto *trace : g->traces_) {
//     std::string txtstr = trace->get_name();
//     int fw, fos, fbl, fh;
//     this->font_label_->measure(txtstr.c_str(), &fw, &fos, &fbl, &fh);
//     if (fw > txtw)
//       txtw = fw;
//     if (fh > txth)
//       txth = fh;
//     if (trace->get_line_thickness() > lt)
//       lt = trace->get_line_thickness();
//     ESP_LOGI(TAGL, "  %s %d %d", txtstr.c_str(), fw, fh);
//
//     if (this->values_ != VALUE_POSITION_TYPE_NONE) {
//       std::stringstream ss;
//       ss << std::fixed << std::setprecision(trace->sensor_->get_accuracy_decimals()) << trace->sensor_->get_state();
//       std::string valstr = ss.str();
//       if (this->units_) {
//         valstr += trace->sensor_->get_unit_of_measurement();
//       }
//       this->font_value_->measure(valstr.c_str(), &fw, &fos, &fbl, &fh);
//       if (fw > valw)
//         valw = fw;
//       if (fh > valh)
//         valh = fh;
//       ESP_LOGI(TAGL, "    %s %d %d", valstr.c_str(), fw, fh);
//     }
//   }
//   // Add extra margin
//   txtw *= 1.2;
//   valw *= 1.2;
//
//   uint8_t n = g->traces_.size();
//   uint16_t w = this->width_;
//   uint16_t h = this->height_;
//   DirectionType dir = this->direction_;
//   ValuePositionType valpos = this->values_;
//   if (!this->font_value_) {
//     valpos = VALUE_POSITION_TYPE_NONE;
//   }
//   // Line sample always goes below text for compactness
//   this->yl_ = txth + (txth / 4) + lt / 2;
//
//   if (dir == DIRECTION_TYPE_AUTO) {
//     dir = DIRECTION_TYPE_HORIZONTAL;  // as default
//     if (h > 0) {
//       dir = DIRECTION_TYPE_VERTICAL;
//     }
//   }
//
//   if (valpos == VALUE_POSITION_TYPE_AUTO) {
//     // TODO: do something smarter?? - fit to w and h?
//     valpos = VALUE_POSITION_TYPE_BELOW;
//   }
//
//   if (valpos == VALUE_POSITION_TYPE_BELOW) {
//     this->yv_ = txth + (txth / 4);
//     if (this->lines_)
//       this->yv_ += txth / 4 + lt;
//   } else if (valpos == VALUE_POSITION_TYPE_BESIDE) {
//     this->xv_ = (txtw + valw) / 2;
//   }
//
//   // If width or height is specified we divide evenly within, else we do tight-fit
//   if (w == 0) {
//     this->x0_ = txtw / 2;
//     this->xs_ = txtw;
//     if (valpos == VALUE_POSITION_TYPE_BELOW) {
//       this->xs_ = std::max(txtw, valw);
//       ;
//       this->x0_ = this->xs_ / 2;
//     } else if (valpos == VALUE_POSITION_TYPE_BESIDE) {
//       this->xs_ = txtw + valw;
//     }
//     if (dir == DIRECTION_TYPE_VERTICAL) {
//       this->width_ = this->xs_;
//     } else {
//       this->width_ = this->xs_ * n;
//     }
//   } else {
//     this->xs_ = w / n;
//     this->x0_ = this->xs_ / 2;
//   }
//
//   if (h == 0) {
//     this->ys_ = txth;
//     if (valpos == VALUE_POSITION_TYPE_BELOW) {
//       this->ys_ = txth + txth / 2 + valh;
//       if (this->lines_) {
//         this->ys_ += lt;
//       }
//     } else if (valpos == VALUE_POSITION_TYPE_BESIDE) {
//       if (this->lines_) {
//         this->ys_ = std::max(txth + txth / 4 + lt + txth / 4, valh + valh / 4);
//       } else {
//         this->ys_ = std::max(txth + txth / 4, valh + valh / 4);
//       }
//       this->height_ = this->ys_ * n;
//     }
//     if (dir == DIRECTION_TYPE_HORIZONTAL) {
//       this->height_ = this->ys_;
//     } else {
//       this->height_ = this->ys_ * n;
//     }
//   } else {
//     this->ys_ = h / n;
//   }
//
//   if (dir == DIRECTION_TYPE_HORIZONTAL) {
//     this->ys_ = 0;
//   } else {
//     this->xs_ = 0;
//   }
// }
//
// void Graph::draw_legend(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color) {
//   if (!legend_)
//     return;
//
//   /// Plot border
//   if (this->border_) {
//     int w = legend_->width_;
//     int h = legend_->height_;
//     buff->horizontal_line(x_offset, y_offset, w, color);
//     buff->horizontal_line(x_offset, y_offset + h - 1, w, color);
//     buff->vertical_line(x_offset, y_offset, h, color);
//     buff->vertical_line(x_offset + w - 1, y_offset, h, color);
//   }
//
//   int x = x_offset + legend_->x0_;
//   int y = y_offset;
//   for (auto *trace : traces_) {
//     std::string txtstr = trace->get_name();
//     ESP_LOGV(TAG, "  %s", txtstr.c_str());
//
//     buff->printf(x, y, legend_->font_label_, trace->get_line_color(), TextAlign::TOP_CENTER, "%s", txtstr.c_str());
//
//     if (legend_->lines_) {
//       uint16_t thick = trace->get_line_thickness();
//       for (int i = 0; i < legend_->x0_ * 4 / 3; i++) {
//         uint8_t b = (i % (thick * LineType::PATTERN_LENGTH)) / thick;
//         if (((uint8_t) trace->get_line_type() & (1 << b)) == (1 << b)) {
//           buff->vertical_line(x - legend_->x0_ * 2 / 3 + i, y + legend_->yl_ - thick / 2, thick,
//                               trace->get_line_color());
//         }
//       }
//     }
//
//     if (legend_->values_ != VALUE_POSITION_TYPE_NONE) {
//       int xv = x + legend_->xv_;
//       int yv = y + legend_->yv_;
//       std::stringstream ss;
//       ss << std::fixed << std::setprecision(trace->sensor_->get_accuracy_decimals()) << trace->sensor_->get_state();
//       std::string valstr = ss.str();
//       if (legend_->units_) {
//         valstr += trace->sensor_->get_unit_of_measurement();
//       }
//       buff->printf(xv, yv, legend_->font_value_, trace->get_line_color(), TextAlign::TOP_CENTER, "%s", valstr.c_str());
//       ESP_LOGV(TAG, "    value: %s", valstr.c_str());
//     }
//     x += legend_->xs_;
//     y += legend_->ys_;
//   }
// }
//
// void Graph::setup() {
//   for (auto *trace : traces_) {
//     trace->init(this);
//   }
// }
//
// void Graph::dump_config() {
//   for (auto *trace : traces_) {
//     ESP_LOGCONFIG(TAG, "Graph for sensor %s", trace->get_name().c_str());
//   }
// }
//
// }  // namespace graph
// }  // namespace esphome
// // class Test : public esphome::graph::Graph{
// // };
// myimpl
// class NewHistoryData : public esphome::graph::HistoryData {
//     public:
//         ~NewHistoryData(){};
//
//         void setData(std::vector<float> samples, int currentSize) {
//             samples_ = samples;
//             count_ = currentSize;
//         }
// };
// class NewGraphTrace : public esphome::graph::GraphTrace {
//     public:
//
//   const void set_tracedata(esphome::graph::HistoryData data) { data_ = data; }
// };
esphome::Color myC(100,100,100,100);

class GraphHT {
 public:
     explicit GraphHT(DisplayBuffer *db/*, const std::vector<float> &data*/){
         // _raw_data = data;
         _db = db;
         // processData();
         // ng.add_traceNew(&nt);
     }
     // ht.init(_db);

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
    void prepareGraph(){
        ESP_LOGD("Graph", "prepare graph");
        if (_initialized == 0){
            ++_initialized;
            ESP_LOGD("Graph", "initializing _single_temperature_graph object");
            _single_temperature_graph = new esphome::graph::Graph();
        }
        _single_temperature_graph->set_duration(3600);
        _single_temperature_graph->set_width(230);
        _single_temperature_graph->set_height(100);
        _single_temperature_graph->set_component_source("graph");
        // App.register_component(_single_temperature_graph);
        _single_temperature_graph->set_grid_x(600);
        _single_temperature_graph->set_grid_y(1.0f);
        _single_temperature_graph->set_max_range(30);
        ESP_LOGD("Graph", "done prepare graph");
    }
    void prepareGraphTrace(){
        ESP_LOGD("Graph", "Prepare GraphTrace");
        if (_initialized == 1){
            ++_initialized;
        ESP_LOGD("Graph", "initializing gt object");
            _graph_graphtrace = new esphome::graph::GraphTrace(esphome::graph::GraphTrace());
        }
        _graph_graphtrace->set_sensor(at_down_bathroom_1_temperature);
        _graph_graphtrace->set_name("at_down_bathroom_1_temperature");
        _graph_graphtrace->set_line_thickness(2);
        _graph_graphtrace->set_line_type(esphome::graph::LINE_TYPE_SOLID);
        _graph_graphtrace->set_line_color(id(my_red));
        ESP_LOGD("Graph", "done prepare GraphTrace");
    }
    void drawGraph(){
      ESP_LOGD("Graph", "enter draw()");
      if (!ini){
          ini = true;
          prepareGraph();
          prepareGraphTrace();
          //
          int width = 230;
          _nhd.init(width);
          std::vector<float> d = {
              14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0,
          23.0};//{19.0,21.0,21.0,21.0,19.0,20.0,20.0,21.0,20.0,21.0,22.0,22.0,19.0,19.0,22.0,19.0,20.0,22.0,20.0,19.0};

          int currentSize = d.size();
          d.resize(width, NAN);
          _nhd.setData(d, currentSize);
          _nhd.set_update_time_ms(_single_temperature_graph->get_duration() * 1000 / _single_temperature_graph->get_width());
          _nhd.take_sample(21.0);
          ESP_LOGD("Graph", "add tracedata");
          _graph_graphtrace->set_tracedata(_nhd);
          ESP_LOGD("Graph", "added tracedata");
          _single_temperature_graph->add_trace(_graph_graphtrace);
          uint16_t x = 5;
          uint16_t y = 30;
          _single_temperature_graph->draw(_db, 0, y, id(color_accent_primary));
      }else{
          // float inc = 1;
          // if (value_ > 230){
          //     inc = -1.0;
          // }else if(value_ <=0){
          //     inc = 1.0;;
          // }
          // value_ += inc;
        ESP_LOGD("Graph", "add new value");
          value_ += 1.0;
          _nhd.take_sample(value_);
          _single_temperature_graph->addValue(2.3);
          _single_temperature_graph->draw(_db, 0, 30, id(color_accent_primary));

      }
        // int x_offset = 4;
        // int y_offset = 38;
        // int width = 230;
        // int height = 90;
        // id(_db).horizontal_line(x_offset, y_offset, width, _axisColor);
        // id(_db).horizontal_line(x_offset, y_offset + height - 1, width, _axisColor);
        // id(_db).vertical_line(x_offset, y_offset, height, _axisColor);
        // id(_db).vertical_line(x_offset + width - 1, y_offset, height, _axisColor);
        // float ymin = 0;
        // float ymax = 6;
        // float yrange = ymax - ymin;
        // Color c = _graphColor;  

    //     y_offset = 110;
    //     int oldx = 0;
    //     int oldy = 0;
    //     ESP_LOGD("Graph", "start line drawing");
    //     for (int i = 0; i < _binned_data.size(); ++i) {
    //         int x2 = x_offset + i;
    //         int y2 = y_offset - _binned_data[i];
    //         if (oldx == 0) {
    //             oldx = x2;
    //             oldy = y2;
    //         }
    //         id(_db).line(oldx, oldy, x2, y2, c);
    //         oldx = x2;
    //         oldy = y2;
    // }
    };


 private:
  // tdisplay res 240x135
  int _bin_size = 240 - 10; // TODO make display resolution global var per device
  Color _graphColor = id(my_green);
  Color _axisColor = id(my_blue);
  std::vector<float> _raw_data;
  std::vector<float> _binned_data;
  DisplayBuffer *_db = nullptr;
  esphome::graph::Graph *_single_temperature_graph;
  esphome::graph::GraphTrace *_graph_graphtrace;
  esphome::graph::HistoryData _nhd ;
  int _initialized = 0;
  bool ini = false;
  float value_{0};

};
