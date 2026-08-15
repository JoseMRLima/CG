#pragma once

class Transformation {
  public:
    virtual ~Transformation() = default;
    virtual void apply() const = 0;
};

class Translate : public Transformation {
    float x, y, z;

  public:
    Translate(float x, float y, float z);
    void apply() const override;
};

class Rotate : public Transformation {
    float angle, x, y, z;

  public:
    Rotate(float angle, float x, float y, float z);
    void apply() const override;
};

class Scale : public Transformation {
    float x, y, z;

  public:
    Scale(float x, float y, float z);
    void apply() const override;
};
