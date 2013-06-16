/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLIGTLSessionManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLAnnotationTextNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLIGTLStatusNode.h"
#include "vtkIGTLToMRMLString.h"
#include "vtkIGTLToMRMLStatus.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLIGTLSessionManagerNode);

//----------------------------------------------------------------------------
vtkMRMLIGTLSessionManagerNode::vtkMRMLIGTLSessionManagerNode()
{
  this->ConnectorNodeIDInternal = 0;
  this->ConnectorNodeReferenceRole = 0;
  this->ConnectorNodeReferenceMRMLAttributeName = 0;
  this->MessageNodeReferenceRole = 0;
  this->MessageNodeReferenceMRMLAttributeName = 0;

  this->SetConnectorNodeReferenceRole("connector");
  this->SetConnectorNodeReferenceMRMLAttributeName("connectorNodeRef");

  this->SetMessageNodeReferenceRole("message");
  this->SetMessageNodeReferenceMRMLAttributeName("messageNodeRef");

  this->HideFromEditors = 0;
  this->AddNodeReferenceRole(this->GetConnectorNodeReferenceRole(),
                             this->GetConnectorNodeReferenceMRMLAttributeName());
  this->AddNodeReferenceRole(this->GetMessageNodeReferenceRole(),
                             this->GetMessageNodeReferenceMRMLAttributeName());

  this->StringMessageConverter = NULL;
  this->StatusMessageConverter = NULL;


  this->CommunicationStatus = 0;
}

//----------------------------------------------------------------------------
vtkMRMLIGTLSessionManagerNode::~vtkMRMLIGTLSessionManagerNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  const char* connectorNodeID = this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole());

  os << indent << "ConnectorNodeID: " <<
    (connectorNodeID ? connectorNodeID : "(none)") << "\n";
}


//----------------------------------------------------------------------------
const char* vtkMRMLIGTLSessionManagerNode::GetConnectorNodeID()
{
  return this->GetConnectorNodeIDInternal();
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::SetAndObserveConnectorNodeID(const char *connectorNodeID)
{
  // ------------------------------------------------------------
  // Set reference node to start observing events

  if (connectorNodeID == 0 && this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole()) == 0)
    {
    // was NULL and still NULL, nothing to do
    return;
    }

  if (connectorNodeID != 0 && this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole()) != 0 &&
      !strcmp(connectorNodeID, this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole())))
    {
    //the same ID, nothing to do
    return;
    }

  vtkMRMLScene * scene = this->GetScene();
  if (!scene)
    {
    return;
    }

  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(scene->GetNodeByID(connectorNodeID));
  if (!cnode) // There is no connector node with the specified MRML ID
    {
    return;
    }

  // use vtkMRMLNode call to set and observe reference
  vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  this->SetAndObserveNodeReferenceID(this->GetConnectorNodeReferenceRole(), connectorNodeID, events);
  //this->InvokeEvent(vtkMRMLIGTLSessionManagerNode::TransformModifiedEvent, NULL);
  this->SetConnectorNodeIDInternal(connectorNodeID);

  // ------------------------------------------------------------
  // Remove outgoing message nodes currently registered.

  while (cnode->GetNumberOfOutgoingMRMLNodes() > 0)
    {
    vtkMRMLNode* node = cnode->GetOutgoingMRMLNode(0);
    const char* attr = node->GetAttribute("IGTLSessionManager.Created");
    if (strcmp(attr, "1") == 0)
      {
      cnode->UnregisterOutgoingMRMLNode(node);
      scene->RemoveNode(node);
      }
    else
      {
      cnode->UnregisterOutgoingMRMLNode(node);
      }
    }

  // Remove incoming message nodes currently registered.
  while (cnode->GetNumberOfIncomingMRMLNodes() > 0)
    {
    vtkMRMLNode* node = cnode->GetIncomingMRMLNode(0);
    const char* attr = node->GetAttribute("IGTLSessionManager.Created");
    if (strcmp(attr, "1") == 0)
      {
      cnode->UnregisterIncomingMRMLNode(node);
      scene->RemoveNode(node);
      }
    else
      {
      cnode->UnregisterIncomingMRMLNode(node);
      }
    }

  // ------------------------------------------------------------
  // Register message converter

  if (!this->StringMessageConverter)
    {
    this->StringMessageConverter = vtkIGTLToMRMLString::New();
    }
  cnode->RegisterMessageConverter(this->StringMessageConverter);

  if (!this->StatusMessageConverter)
    {
    this->StatusMessageConverter = vtkIGTLToMRMLStatus::New();
    }
  cnode->RegisterMessageConverter(this->StatusMessageConverter);


  // ------------------------------------------------------------
  // Register message nodes

  this->RegisterMessageNodes(cnode);
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::AddAndObserveMessageNodeID(const char *messageNodeID)
{
  int n = this->GetNumberOfNodeReferences(this->GetMessageNodeReferenceRole());
  for (int i = 0; i < n; i ++)
    {
    const char* id = GetNthNodeReferenceID(this->GetMessageNodeReferenceRole(), i);
    if (strcmp(messageNodeID, id) == 0)
      {
      // Alredy on the list. Remove it.
      return;
      }
    }

  vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  this->AddAndObserveNodeReferenceID(this->GetMessageNodeReferenceRole(), messageNodeID, events);

  //this->InvokeEvent(vtkMRMLIGTLSessionManagerNode::TransformModifiedEvent, NULL);
}

//---------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::ProcessMRMLEvents ( vtkObject *caller,
                                                  unsigned long event, 
                                                  void *vtkNotUsed(callData) )
{
  // as retrieving the parent transform node can be costly (browse the scene)
  // do some checks here to prevent retrieving the node for nothing.
  if (caller == NULL ||
      (event != vtkCommand::ModifiedEvent))
    {
    return;
    }
  //vtkMRMLTransformNode *tnode = this->GetParentTransformNode();
  //if (tnode == caller)
  //  {
  //  //TODO don't send even on the scene but rather have vtkMRMLSliceLayerLogic listen to
  //  // TransformModifiedEvent
  //  //this->GetScene()->InvokeEvent(vtkCommand::ModifiedEvent, NULL);
  //  this->InvokeEvent(vtkMRMLIGTLSessionManagerNode::TransformModifiedEvent, NULL);
  //  }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceAdded(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(reference->GetReferencedNodeID());
  if (!node)
    {
    return;
    }

  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceRemoved(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(reference->GetReferencedNodeID());
  if (!node)
    {
    return;
    }

  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceModified(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(reference->GetReferencedNodeID());
  if (!node)
    {
    return;
    }

  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::ConfigureMessageNode(vtkMRMLIGTLConnectorNode* cnode, vtkMRMLNode* node, int io)
{
  if (!cnode || !node)
    {
    return 0;
    }

  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return 0;
    }

  node->SetAttribute("IGTLSessionManager.Created", "1");
  scene->AddNode(node);

  if (io == vtkMRMLIGTLConnectorNode::IO_OUTGOING)
    {
    cnode->RegisterOutgoingMRMLNode(node);
    }
  else // vtkMRMLIGTLConnectorNode::IO_INCOMING
    {
    cnode->RegisterIncomingMRMLNode(node);
    }

  this->AddAndObserveMessageNodeID(node->GetID());

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::RegisterMessageNodes(vtkMRMLIGTLConnectorNode* cnode)
{
  if (!cnode)
    {
    return 0;
    }

  // --------------------------------------------------
  // Outgoing nodes

  vtkSmartPointer< vtkMRMLAnnotationTextNode > outCommand = vtkSmartPointer< vtkMRMLAnnotationTextNode >::New();
  outCommand->SetName("COMMAND");
  this->ConfigureMessageNode(cnode, outCommand, vtkMRMLIGTLConnectorNode::IO_OUTGOING);
  this->SetOutCommandNodeIDInternal(outCommand->GetID());

  vtkSmartPointer< vtkMRMLLinearTransformNode > calibTrans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  calibTrans->SetName("CALIBRATION");
  this->ConfigureMessageNode(cnode, calibTrans, vtkMRMLIGTLConnectorNode::IO_OUTGOING);
  this->SetOutCalibrationTransformNodeIDInternal(calibTrans->GetID());

  vtkSmartPointer< vtkMRMLLinearTransformNode > targetTrans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  calibTrans->SetName("TARGET");
  this->ConfigureMessageNode(cnode, targetTrans, vtkMRMLIGTLConnectorNode::IO_OUTGOING);
  this->SetOutTargetTransformQueryNodeIDInternal(targetTrans->GetID());

  // --------------------------------------------------
  // Incoming nodes
  vtkSmartPointer< vtkMRMLAnnotationTextNode > ack = vtkSmartPointer< vtkMRMLAnnotationTextNode >::New();
  ack->SetName("ACK");
  this->ConfigureMessageNode(cnode, ack, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInAckknowledgeStringNodeIDInternal(ack->GetID());

  vtkSmartPointer< vtkMRMLAnnotationTextNode > inCommand = vtkSmartPointer< vtkMRMLAnnotationTextNode >::New();
  inCommand->SetName("COMMAND");
  this->ConfigureMessageNode(cnode, inCommand, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInCommandStringNodeIDInternal(inCommand->GetID());  

  vtkSmartPointer< vtkMRMLIGTLStatusNode > startUpStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  startUpStatus->SetName("START_UP");
  this->ConfigureMessageNode(cnode, startUpStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInStartUpStatusNodeIDInternal(startUpStatus->GetID());
  
  vtkSmartPointer< vtkMRMLIGTLStatusNode > calibrationStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  calibrationStatus->SetName("CALIBRATION");
  this->ConfigureMessageNode(cnode, calibrationStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInCalibrationStatusNodeIDInternal(calibrationStatus->GetID());
  
  vtkSmartPointer< vtkMRMLIGTLStatusNode > targetingStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  targetingStatus->SetName("TARGETING");
  this->ConfigureMessageNode(cnode, targetingStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInTargetingStatusNodeIDInternal(targetingStatus->GetID());
  
  vtkSmartPointer< vtkMRMLIGTLStatusNode > movingStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  movingStatus->SetName("MOVING_DONE");
  this->ConfigureMessageNode(cnode, movingStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInMovingStatusNodeIDInternal(movingStatus->GetID());
  
  vtkSmartPointer< vtkMRMLIGTLStatusNode > manualStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  manualStatus->SetName("MANUAL");
  this->ConfigureMessageNode(cnode, manualStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInManualStatusNodeIDInternal(manualStatus->GetID());
  
  vtkSmartPointer< vtkMRMLIGTLStatusNode > errorStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  errorStatus->SetName("ERROR");
  this->ConfigureMessageNode(cnode, errorStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInErrorStatusNodeIDInternal(errorStatus->GetID());

  vtkSmartPointer< vtkMRMLLinearTransformNode > currentPosition = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  currentPosition->SetName("CURRENT_POSITION");
  this->ConfigureMessageNode(cnode, currentPosition, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInCurrentPositionTransformNodeIDInternal(currentPosition->GetID());

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::IsConnected()
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return 0;
    }

  vtkMRMLNode* node = scene->GetNodeByID(this->GetConnectorNodeIDInternal());
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return 0;
    }

  if (cnode->GetState() == vtkMRMLIGTLConnectorNode::STATE_CONNECTED)
    {
    return 1;
    }
  else
    {
    return 0;
    }
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendStartUpCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendPlanningCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendCalibrationCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendCalibrationTransform(vtkMatrix4x4* transform)
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingTarget(double r, double a, double s)
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingMove()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendManualCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendStopCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendEmergencyCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendGetStatusCommand()
{
  if (!this->IsConnected())
    {
    return 0;
    }
  
  return 1;
}
