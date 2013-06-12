/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerBRPProstateNavModuleWidget.h"
#include "ui_qSlicerBRPProstateNavModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerBRPProstateNavModuleWidgetPrivate: public Ui_qSlicerBRPProstateNavModuleWidget
{
public:
  qSlicerBRPProstateNavModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerBRPProstateNavModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModuleWidgetPrivate::qSlicerBRPProstateNavModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerBRPProstateNavModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModuleWidget::qSlicerBRPProstateNavModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerBRPProstateNavModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModuleWidget::~qSlicerBRPProstateNavModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerBRPProstateNavModuleWidget::setup()
{
  Q_D(qSlicerBRPProstateNavModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
}

