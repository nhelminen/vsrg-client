#pragma once

#include "core/engine/shader.hpp"
#include "core/ui/spriteComponent.hpp"
#include "public/engineContext.hpp"


namespace mania {
class Strum : public vsrg::SpriteComponent {
public:
  Strum(vsrg::EngineContext *ctx, const std::string &spritePath,
        int column = 0);
  ~Strum();

  void setPosition(float x, float y);
  void setSize(float width, float height);
  void setPadding(float top, float bottom, float left, float right);

  virtual void update(float deltaTime);
  virtual void render();

  bool isPressed() const { return is_pressed; }

  void setPressed(bool pressed) { is_pressed = pressed; }

  int getColumn() const { return column; }

  glm::vec2 getPosition() const { return properties.position; }

private:
  int column;
  bool is_pressed = false;
};
}