#pragma once

class IProgram
{
public:
  virtual bool render() = 0;
  virtual ~IProgram() {}
};