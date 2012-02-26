/*
Copyright (C) 2010 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "InteractiveRegionHistogramViewerWidget.h"

// Custom
#include "Helpers.h"
#include "RegionSelectionWidget.h"
#include "HistogramWidget.h"

// VTK
#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkMath.h>
#include <vtkImageData.h>
#include <vtkImageSlice.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWidgetRepresentation.h>
#include <vtkUnsignedCharArray.h>

// STL
#include <iostream>

// Qt
#include <QButtonGroup>
#include <QFileDialog>

// ITK
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

InteractiveRegionHistogramViewerWidget::InteractiveRegionHistogramViewerWidget(const std::string& fileName)
{
  SharedConstructor();
  OpenImage(fileName);
}

void InteractiveRegionHistogramViewerWidget::SharedConstructor()
{
  // Setup the GUI and connect all of the signals and slots
  setupUi(this);

  HistogramWidgets.push_back(HistogramWidgetR);
  HistogramWidgetR->SetBarColor(Qt::red);
  
  HistogramWidgets.push_back(HistogramWidgetG);
  HistogramWidgetG->SetBarColor(Qt::green);
  
  HistogramWidgets.push_back(HistogramWidgetB);
  HistogramWidgetB->SetBarColor(Qt::blue);
  
  this->InteractorStyleImage = vtkSmartPointer<vtkInteractorStyleImage>::New();
  this->InteractorStyleTrackballCamera = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  //this->InteractorStyle->TrackballStyle->AddObserver(CustomTrackballStyle::PatchesMovedEvent, this, &MainWindow::UserPatchMoved);

  this->Image = NULL;

  this->ImageRenderer = vtkSmartPointer<vtkRenderer>::New();
  this->ImageRenderer->AddViewProp(this->ImageLayer.ImageSlice);

  this->InteractorStyleImage->SetCurrentRenderer(this->ImageRenderer);
  this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->InteractorStyleImage);

  this->qvtkWidget->GetRenderWindow()->AddRenderer(this->ImageRenderer);

  this->RegionSelector = vtkSmartPointer<RegionSelectionWidget>::New();
  this->RegionSelector->SetInteractor(this->qvtkWidget->GetRenderWindow()->GetInteractor());
  this->RegionSelector->CreateDefaultRepresentation();
  this->RegionSelector->SelectableOff();
  this->RegionSelector->On();

  this->RegionSelector->Renderer = this->ImageRenderer;

  this->RegionSelector->AddObserver(this->RegionSelector->ChangedEvent,
                                    this, &InteractiveRegionHistogramViewerWidget::EndSelectionSlot);
}
  
InteractiveRegionHistogramViewerWidget::InteractiveRegionHistogramViewerWidget(QWidget *parent)
{
  SharedConstructor();
}

void InteractiveRegionHistogramViewerWidget::EndSelectionSlot()
{
  if(Image.GetPointer() == NULL)
  {
    return;
  }
  
  double* lowerLeft = static_cast<vtkBorderRepresentation*>(this->RegionSelector->GetRepresentation())->
                      GetPositionCoordinate()->GetComputedWorldValue (this->ImageRenderer);
  //std::cout << "Lower left: " << lowerLeft[0] << " " << lowerLeft[1] << std::endl;

  double* upperRight = static_cast<vtkBorderRepresentation*>(this->RegionSelector->GetRepresentation())->
                       GetPosition2Coordinate()->GetComputedWorldValue (this->ImageRenderer);
  //std::cout << "Upper right: " << upperRight[0] << " " << upperRight[1] << std::endl;
  
  itk::Index<2> corner;
  corner[0] = std::max(round(lowerLeft[0]), 0.0d);
  corner[1] = std::max(round(lowerLeft[1]), 0.0d);
  
  itk::Size<2> size;
  size[0] = round(upperRight[0] - corner[0]);
  size[1] = round(upperRight[1] - corner[1]);
  
  SelectedRegion.SetIndex(corner);
  SelectedRegion.SetSize(size);

  bool atLeastPartialOverlap = SelectedRegion.Crop(Image->GetLargestPossibleRegion());
  if(atLeastPartialOverlap)
  {
    CreateHistogramsFromRegion(SelectedRegion);
  }

}

void InteractiveRegionHistogramViewerWidget::CreateHistogramsFromRegion(const itk::ImageRegion<2>& region)
{
  for(unsigned int component = 0; component < Image->GetNumberOfComponentsPerPixel(); ++component)
  {
    itk::ImageRegionConstIterator<ImageType> imageIterator(Image, region);
    imageIterator.GoToBegin();

    std::vector<float> values;
    while(!imageIterator.IsAtEnd())
      {
      values.push_back(imageIterator.Get()[component]);
      ++imageIterator;
      }

    // Convert values to bins
    const unsigned int numberOfBins = 20;
    std::vector<unsigned int> bins(numberOfBins);
    const float minValue = 0;
    const float maxValue = 255;

    const float binWidth = (maxValue - minValue) / static_cast<float>(numberOfBins);

    for(unsigned int i = 0; i < values.size(); ++i)
    {
      unsigned int bin = (values[i] - minValue) / binWidth;
      bins[bin]++;
    }

    HistogramWidgets[component]->SetVector(bins);
  }
}

void InteractiveRegionHistogramViewerWidget::OpenImage(const std::string& fileName)
{

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();

  this->Image = reader->GetOutput();

  Helpers::ITKImageToVTKRGBImage(reader->GetOutput(), this->ImageLayer.ImageData);
  this->ImageLayer.ImageData->Modified();
  this->ImageRenderer->ResetCamera();


  vtkBorderRepresentation* representation = static_cast<vtkBorderRepresentation*>(this->RegionSelector->GetRepresentation());
//   representation->GetPositionCoordinate()->SetCoordinateSystemToWorld();
//   representation->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
//   representation->SetPosition(0, 0);
//   representation->SetPosition2(20, 20);
}

void InteractiveRegionHistogramViewerWidget::on_actionOpen_activated()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                    "OpenFile", ".", "All Files (*.*)");

  if(!fileName.isEmpty())
  {
    OpenImage(fileName.toStdString());
  }
}

