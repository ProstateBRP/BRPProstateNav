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
#include <QtPlugin>

// BRPProstateNav Logic includes
#include <vtkSlicerBRPProstateNavLogic.h>

// BRPProstateNav includes
#include "qSlicerBRPProstateNavModule.h"
#include "qSlicerBRPProstateNavModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerBRPProstateNavModule, qSlicerBRPProstateNavModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerBRPProstateNavModulePrivate
{
public:
  qSlicerBRPProstateNavModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerBRPProstateNavModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModulePrivate
::qSlicerBRPProstateNavModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerBRPProstateNavModule methods

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModule
::qSlicerBRPProstateNavModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerBRPProstateNavModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerBRPProstateNavModule::~qSlicerBRPProstateNavModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerBRPProstateNavModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerBRPProstateNavModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerBRPProstateNavModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerBRPProstateNavModule::icon()const
{
  return QIcon(":/Icons/BRPProstateNav.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerBRPProstateNavModule::categories() const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerBRPProstateNavModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerBRPProstateNavModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerBRPProstateNavModule
::createWidgetRepresentation()
{
  return new qSlicerBRPProstateNavModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerBRPProstateNavModule::createLogic()
{
  return vtkSlicerBRPProstateNavLogic::New();
}
