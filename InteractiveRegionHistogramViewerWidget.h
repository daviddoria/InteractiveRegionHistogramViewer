/*
Copyright (C) 2011 David Doria, daviddoria@gmail.com

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

#ifndef InteractiveRegionHistogramViewerWidget_H
#define InteractiveRegionHistogramViewerWidget_H

// Custom
#include "Layer.h"
class RegionSelectionWidget;

// Qt
#include <QMainWindow>

// GUI
#include "ui_InteractiveRegionHistogramViewerWidget.h"

// VTK
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>

// Forward declarations
class vtkPolyDataMapper;
class vtkActor;
class vtkRenderer;
class vtkVertexGlyphFilter;
class vtkPoints;
class vtkPolyData;
class vtkUnsignedCharArray;

// ITK
#include "itkImage.h"
#include "itkCovariantVector.h"

class InteractiveRegionHistogramViewerWidget : public QMainWindow, private Ui::InteractiveRegionHistogramViewerWidget
{
Q_OBJECT
public:
  typedef itk::Image< itk::CovariantVector<unsigned char, 3>, 2 > ImageType;
  
  InteractiveRegionHistogramViewerWidget(QWidget *parent = 0);

  InteractiveRegionHistogramViewerWidget(const std::string& fileName);

  void SharedConstructor();

public slots:

  void on_actionOpen_activated();

private:

  void OpenImage(const std::string& fileName);
  
  void CreateHistogramsFromRegion(const itk::ImageRegion<2>& region);

  void EndSelectionSlot();

  ImageType::Pointer Image;
  Layer ImageLayer;
  
  vtkSmartPointer<vtkRenderer> ImageRenderer;
  vtkSmartPointer<vtkRenderer> ColorSpaceRenderer;

  vtkSmartPointer<vtkInteractorStyleImage> InteractorStyleImage;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> InteractorStyleTrackballCamera;
  
  vtkSmartPointer<RegionSelectionWidget> RegionSelector;
  
  itk::ImageRegion<2> SelectedRegion;

  /** These histogram widgets correspond to the index'th channel of the image */
  std::vector<HistogramWidget*> HistogramWidgets;

};

#endif
