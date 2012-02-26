#ifndef HistogramWidget_HPP
#define HistogramWidget_HPP

#include "HistogramWidget.h" // Appease syntax parser

template <typename T>
void HistogramWidget::SetVector(const std::vector<T>& vec)
{
  Vector.clear();
  Vector.resize(vec.size());
  for(unsigned int i = 0; i < vec.size(); ++i)
  {
    Vector[i] = vec[i];
  }
  
  DrawHistogram();
}

#endif
