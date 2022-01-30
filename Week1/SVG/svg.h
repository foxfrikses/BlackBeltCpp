#pragma once

#include <ostream>
#include <variant>
#include <vector>
#include <string>
#include <sstream>
#include <optional>

namespace Svg
{
  struct Point {double x, y;};
  struct Rgb {int red, green, blue;};

  using Color = std::variant<std::monostate, std::string, Rgb>;
  static const Color NoneColor;

  bool operator==(const Color& lhs, const Color& rhs);
  bool operator!=(const Color& lhs, const Color& rhs);

  template <class T>
  void PrintAttr(std::ostream& output, const std::string& attr, T&& value) {
    output << ' ' << attr << '=' << '"' << value << '"';
  }

  struct BaseData {
    Color fillColor_ = NoneColor;
    Color strokeColor_ = NoneColor;
    double strokeWidth_ = 1.0;
    std::optional<std::string> strokeLineCap_;
    std::optional<std::string> strokeLineJoin_;

    void RenderProperties(std::ostream&) const;
  };

  template <class Type>
  class BaseObject : protected BaseData {
    Type& GetSelf() {return *reinterpret_cast<Type*>(this);}
  public:
    Type& SetFillColor(Color fillColor) {fillColor_ = move(fillColor); return GetSelf();}
    Type& SetStrokeColor(Color strokeColor) {strokeColor_ = move(strokeColor); return GetSelf();}
    Type& SetStrokeWidth(double strokeWidth) {strokeWidth_ = strokeWidth; return GetSelf();}
    Type& SetStrokeLineCap(const std::string& strokeLineCap) {strokeLineCap_ = strokeLineCap; return GetSelf();}
    Type& SetStrokeLineJoin(const std::string& strokeLineJoin) {strokeLineJoin_ = strokeLineJoin; return GetSelf();}
  };

  class Polyline : public BaseObject<Polyline> {
  public:
    Polyline& AddPoint(Point);

    void Render(std::ostream& output) const;

  protected:
    void RenderProperties(std::ostream &output) const;

    std::vector<Point> points_;
  };

  class Circle : public BaseObject<Circle> {
  public:
    Circle& SetCenter(Point);
    Circle& SetRadius(double);

    void Render(std::ostream& output) const;

  protected:
    void RenderProperties(std::ostream& output) const;

    Point center_ = {0.0, 0.0};
    double radius_ = 1.0;
  };

  class Text : public BaseObject<Text> {
  public:
    Text& SetPoint(Point);
    Text& SetOffset(Point);
    Text& SetFontSize(uint32_t);
    Text& SetFontFamily(const std::string&);
    Text& SetData(const std::string&);

    void Render(std::ostream& output) const;

  protected:
    void RenderProperties(std::ostream& output) const;

    Point point_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t fontSize_ = 1;
    std::optional<std::string> fontFamily_;
    std::string data_;
  };

  class Document {
  public:
    template<class Item> void Add(Item&& item) {items_.push_back(std::move(item));}
    template<class Item> void Add(const Item& item) {items_.emplace_back(item);}

    void Render(std::ostream& output) const;

  protected:
    void RenderProperties(std::ostream& output) const;

    std::vector<std::variant<Polyline, Circle, Text>> items_;
  };
}
