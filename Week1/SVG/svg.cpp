#include "svg.h"

using namespace std;

namespace Svg
{
  bool operator==(const Color& lhs, const Color& rhs) {
    if (holds_alternative<monostate>(lhs))
      return holds_alternative<monostate>(rhs);
    if (holds_alternative<string>(lhs))
      return holds_alternative<string>(rhs) && get<string>(lhs) == get<string>(rhs);
    if (holds_alternative<Rgb>(lhs))
      return holds_alternative<Rgb>(rhs) && get<Rgb>(lhs) == get<Rgb>(rhs);
    return false;
  }
  bool operator!=(const Color& lhs, const Color& rhs) {
    return !(lhs == rhs);
  }

  string ToString(const Rgb& rgb) {
    stringstream ss;
    ss << "rgb(" << rgb.red << ',' << rgb.green << ',' << rgb.blue << ')';
    return ss.str();
  }
  string ToString(const Color& color) {
    if (holds_alternative<string>(color)) {
      return get<string>(color);
    }
    else if (holds_alternative<Rgb>(color)) {
      return ToString(get<Rgb>(color));
    }
    return "none";
  }
  string ToString(const std::vector<Point>& points) {
    stringstream ss;
    bool isFirst = true;
    for (auto& point : points) {
      if (!exchange(isFirst, false)) ss << ' ';
      ss << point.x << ',' << point.y;
    }
    return ss.str();
  }

  Polyline& Polyline::AddPoint(Point point) {points_.push_back(point); return *this;}

  Circle& Circle::SetCenter(Point center) {center_ = center; return *this;}

  Circle& Circle::SetRadius(double radius) {radius_ = radius; return *this;}

  Text& Text::SetPoint(Point point) {point_ = point; return *this;}

  Text& Text::SetOffset(Point offset) {offset_ = offset; return *this;}

  Text& Text::SetFontSize(uint32_t fontSize) {fontSize_ = fontSize; return *this;}

  Text& Text::SetFontFamily(const string& fontFamily) {fontFamily_ = fontFamily; return *this;}

  Text& Text::SetData(const string& data) {data_ = data; return *this;}

  void BaseData::RenderProperties(std::ostream& output) const {
    PrintAttr(output, "fill", ToString(fillColor_));
    PrintAttr(output, "stroke", ToString(strokeColor_));
    PrintAttr(output, "stroke-width", strokeWidth_);
    if (strokeLineCap_)
      PrintAttr(output, "stroke-linecap", strokeLineCap_.value());
    if (strokeLineJoin_)
      PrintAttr(output, "stroke-linejoin", strokeLineJoin_.value());
  }

  void Polyline::RenderProperties(std::ostream& output) const {
    BaseObject::RenderProperties(output);
    PrintAttr(output, "points", ToString(points_));
  }

  void Circle::RenderProperties(std::ostream& output) const {
    BaseObject::RenderProperties(output);
    PrintAttr(output, "cx", center_.x);
    PrintAttr(output, "cy", center_.y);
    PrintAttr(output, "r", radius_);
  }

  void Text::RenderProperties(std::ostream& output) const {
    BaseObject::RenderProperties(output);
    PrintAttr(output, "x", point_.x);
    PrintAttr(output, "y", point_.y);
    PrintAttr(output, "dx", offset_.x);
    PrintAttr(output, "dy", offset_.y);
    PrintAttr(output, "font-size", fontSize_);
    if (fontFamily_) {
      PrintAttr(output, "font-family", fontFamily_.value());
    }
  }

  void Polyline::Render(ostream& output) const {
    output << "<polyline";
    RenderProperties(output);
    output << "/>";
  }

  void Circle::Render(ostream& output) const {
    output << "<circle";
    RenderProperties(output);
    output << "/>";
  }

  void Text::Render(ostream& output) const {
    output << "<text";
    RenderProperties(output);
    output << ">";
    output << data_;
    output << "</text>";
  }

  void Document::Render(ostream& output) const {
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    output << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
    for (auto& item : items_) {
      visit([&output](auto& item) {item.Render(output);}, item);
    }
    output << "</svg>";
  }
}

