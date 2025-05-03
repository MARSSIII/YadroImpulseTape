#pragma once

class TapeInterface {
public:
  virtual ~TapeInterface() = default;

  virtual int read() = 0;
  virtual void write(int data) = 0;
  virtual void moveLeft() = 0;
  virtual void moveRight() = 0;
  virtual void rewind() = 0;
  virtual bool isAtEnd() const = 0;
  virtual int getSize() const = 0;
};
