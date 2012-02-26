#ifndef HELPERS_H
#define HELPERS_H

// VTK
#include <vtkSmartPointer.h>
#include <vtkImageData.h>


class vtkPolyData;

namespace Helpers
{

void CreateTransparentImage(vtkImageData* const input, vtkImageData* const output);

void OutputImageSize(vtkImageData* const image);

void OutputBounds(const std::string& name, double bounds[6]);

template <typename TImage>
void ITKImageToVTKRGBImage(const TImage* const image, vtkImageData* outputImage);

}

#include "Helpers.hpp"

#endif
