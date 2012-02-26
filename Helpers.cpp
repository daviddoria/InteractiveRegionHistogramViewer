#include "Helpers.h"

// VTK
#include <vtkPolyData.h>

namespace Helpers
{

void CreateTransparentImage(vtkImageData* const input, vtkImageData* const output)
{
  int* dims = input->GetDimensions();
  output->SetDimensions(dims); 
  output->SetNumberOfScalarComponents(4);
  output->SetScalarTypeToUnsignedChar();
  output->AllocateScalars();
  
  for (int y = 0; y < dims[1]; y++)
    {
    for (int x = 0; x < dims[0]; x++)
      {
      unsigned char* outputPixel = static_cast<unsigned char*>(output->GetScalarPointer(x,y,0));
      unsigned char color = 255;
      outputPixel[0] = color;
      outputPixel[1] = color;
      outputPixel[2] = color;

      //outputPixel[3] = 0; // transparent
      outputPixel[3] = 100; // opaque
      //outputPixel[3] = 255; // opaque
      
      } // end x loop
    } // end y loop
}

void OutputImageSize(vtkImageData* const image)
{
  int* dims = image->GetDimensions();
  std::cout << dims[0] << " " << dims[1] << std::endl;
}

void OutputBounds(const std::string& name, double bounds[6])
{
  std::cout << name << " xmin: " << bounds[0] << " "
            << name << " xmax: " << bounds[1] << std::endl
            << name << " ymin: " << bounds[2] << " "
            << name << " ymax: " << bounds[3] << std::endl
            << name << " zmin: " << bounds[4] << " "
            << name << " zmax: " << bounds[5] << std::endl;
}

} // end namespace
