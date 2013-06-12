/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerBRPProstateNavFooBarWidget.h"
#include "ui_qSlicerBRPProstateNavFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_BRPProstateNav
class qSlicerBRPProstateNavFooBarWidgetPrivate
  : public Ui_qSlicerBRPProstateNavFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerBRPProstateNavFooBarWidget);
protected:
  qSlicerBRPProstateNavFooBarWidget* const q_ptr;

public:
  qSlicerBRPProstateNavFooBarWidgetPrivate(
    qSlicerBRPProstateNavFooBarWidget& object);
  virtual void setupUi(qSlicerBRPProstateNavFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerBRPProstateNavFooBarWidgetPrivate
::qSlicerBRPProstateNavFooBarWidgetPrivate(
  qSlicerBRPProstateNavFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerBRPProstateNavFooBarWidgetPrivate
::setupUi(qSlicerBRPProstateNavFooBarWidget* widget)
{
  this->Ui_qSlicerBRPProstateNavFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerBRPProstateNavFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavFooBarWidget
::qSlicerBRPProstateNavFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerBRPProstateNavFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerBRPProstateNavFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavFooBarWidget
::~qSlicerBRPProstateNavFooBarWidget()
{
}
