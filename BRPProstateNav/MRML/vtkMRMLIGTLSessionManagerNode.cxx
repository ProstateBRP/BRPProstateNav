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

  this->InStartUpStatusNodeIDInternal            = NULL;
  this->InCalibrationStatusNodeIDInternal        = NULL;
  this->InTargetingStatusNodeIDInternal          = NULL;
  this->InTargetStatusNodeIDInternal             = NULL;
  this->InMovingStatusNodeIDInternal             = NULL;
  this->InManualStatusNodeIDInternal             = NULL;
  this->InErrorStatusNodeIDInternal              = NULL;
  this->InEmergencyStatusNodeIDInternal          = NULL;
  this->InCurrentPositionTransformNodeIDInternal = NULL;

  this->CommunicationStatus = 0;
  this->QueryCounter = 0;
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
  
  vtkMRMLNode * node = vtkMRMLNode::SafeDownCast(caller);
  if (!node)
    {
    return;
    }
  
  // Check connector event
  if (this->GetConnectorNodeIDInternal() && strcmp(node->GetID(), this->GetConnectorNodeIDInternal()) == 0)
    {
    vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(cnode);
    if (event == vtkMRMLIGTLConnectorNode::NewDeviceEvent)
      {
      
      int ni = cnode->GetNumberOfIncomingMRMLNodes();
      for (int i = 0; i < ni; i ++)
        {
        vtkMRMLNode* inode = cnode->GetIncomingMRMLNode(i);
        if (inode)
          {
          // Check if there is any corresponding name in the list of outgoing message
          int no = cnode->GetNumberOfOutgoingMRMLNodes();
          for (int j = 0; j < no; j ++)
            {
            vtkMRMLNode* onode = cnode->GetOutgoingMRMLNode(i);
            if (strcmp(inode->GetName(), onode->GetName()) == 0)
              {
              // There is a node with the same name
              vtkMRMLAnnotationTextNode * atnode = vtkMRMLAnnotationTextNode::SafeDownCast(inode);
              if (atnode)
                {
                ProcAcknowledgeString(atnode);
                this->GetScene()->RemoveNode(inode);
                this->GetScene()->RemoveNode(onode);
                }
              }
            }
          }
        }
      }
    return;
    }

  // Process incoming data
  vtkMRMLIGTLStatusNode * snode = vtkMRMLIGTLStatusNode::SafeDownCast(node);
  if (snode)
    {
    ProcStatus(snode);
    }
    
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

  // --------------------------------------------------
  // Incoming nodes

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
  movingStatus->SetName("MOVE_TO_TARGET");
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

  vtkSmartPointer< vtkMRMLIGTLStatusNode > emergencyStatus = vtkSmartPointer< vtkMRMLIGTLStatusNode >::New();
  emergencyStatus->SetName("EMERGENCY");
  this->ConfigureMessageNode(cnode, emergencyStatus, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInEmergencyStatusNodeIDInternal(errorStatus->GetID());

  vtkSmartPointer< vtkMRMLLinearTransformNode > actualTarget = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  actualTarget->SetName("TARGET");
  this->ConfigureMessageNode(cnode, actualTarget, vtkMRMLIGTLConnectorNode::IO_INCOMING);
  this->SetInActualTargetTransformNodeIDInternal(actualTarget->GetID());

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
int vtkMRMLIGTLSessionManagerNode::SetCommunicationStatus(int newState)
{
  this->CommunicationStatus = newState;
  this->InvokeEvent(StatusChangeEvent);
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendCommandWithQueryID(const char* command)
{
  // Check if the scene exists
  vtkMRMLScene* scene = this->GetScene();
  if (!scene)
    {
    return 0;
    }

  // Get a pointer to the connector node
  vtkMRMLNode* node = scene->GetNodeByID(this->GetConnectorNodeIDInternal());
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return 0;
    }

  // Check connection status
  if (cnode->GetState() != vtkMRMLIGTLConnectorNode::STATE_CONNECTED)
    {
    return 0;
    }

  // Generate a message name with a unique query ID.
  this->QueryCounter ++;
  std::stringstream ss;
  ss << "CMD_" << this->QueryCounter;

  // Create a message
  vtkSmartPointer< vtkMRMLAnnotationTextNode > outCommand = vtkSmartPointer< vtkMRMLAnnotationTextNode >::New();
  outCommand->SetName(ss.str().c_str());
  scene->AddNode(outCommand);
  this->ConfigureMessageNode(cnode, outCommand, vtkMRMLIGTLConnectorNode::IO_OUTGOING);

  // Update the node to send the message.
  outCommand->SetTextLabel(command);

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendStartUpCommand()
{
  return this->SendCommandWithQueryID("START_UP");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendPlanningCommand()
{
  return this->SendCommandWithQueryID("PLANNING");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendCalibrationCommand()
{
  return this->SendCommandWithQueryID("CALIBRATION");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingCommand()
{
  return this->SendCommandWithQueryID("TARGETING");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingTarget(double r, double a, double s)
{
  // Check if the scene exists
  vtkMRMLScene* scene = this->GetScene();
  if (!scene)
    {
    return 0;
    }

  // Get a pointer to the connector node
  vtkMRMLNode* node = scene->GetNodeByID(this->GetConnectorNodeIDInternal());
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return 0;
    }

  // Check connection status
  if (cnode->GetState() != vtkMRMLIGTLConnectorNode::STATE_CONNECTED)
    {
    return 0;
    }


  // Generate a message name with a unique query ID.
  this->QueryCounter ++;
  std::stringstream ss;
  ss << "TGT_" << this->QueryCounter;

  vtkSmartPointer< vtkMRMLLinearTransformNode > tnode = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  tnode->SetName(ss.str().c_str());
  scene->AddNode(tnode);
  this->ConfigureMessageNode(cnode, tnode, vtkMRMLIGTLConnectorNode::IO_OUTGOING);

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendTargetingMove()
{
  return this->SendCommandWithQueryID("MOVE_TO_TARGET");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendManualCommand()
{
  return this->SendCommandWithQueryID("MANUAL");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendStopCommand()
{
  return this->SendCommandWithQueryID("STOP");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendEmergencyCommand()
{
  return this->SendCommandWithQueryID("EMERGENCY");
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendGetStatusCommand()
{
  // TODO
  return 1;
}



//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::SendCalibrationTransform(vtkMatrix4x4* transform)
{
  // Check if the scene exists
  vtkMRMLScene* scene = this->GetScene();
  if (!scene)
    {
    return 0;
    }

  // Get a pointer to the connector node
  vtkMRMLNode* node = scene->GetNodeByID(this->GetConnectorNodeIDInternal());
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return 0;
    }

  // Check connection status
  if (cnode->GetState() != vtkMRMLIGTLConnectorNode::STATE_CONNECTED)
    {
    return 0;
    }

  // Generate a message name with a unique query ID.
  this->QueryCounter ++;
  std::stringstream ss;
  ss << "CLB_" << this->QueryCounter;

  // Create a message
  vtkSmartPointer< vtkMRMLLinearTransformNode > tnode = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  scene->AddNode(tnode);
  this->ConfigureMessageNode(cnode, tnode, vtkMRMLIGTLConnectorNode::IO_OUTGOING);
    
  // Update the node to send the message.
  tnode->Modified();

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::ProcAcknowledgeString(vtkMRMLAnnotationTextNode * node)
{
  if (!node)
    {
    return 0;
    }

  if (strcmp(node->GetTextLabel(), "START_UP") == 0)
    {
    if (this->CommunicationStatus == COMSTATE_STARTUP_CMD_SENT)
      {
      this->SetCommunicationStatus(COMSTATE_STARTUP_CMD_ACK);
      // Stop timer
      }
    }
  else if (strcmp(node->GetTextLabel(), "PLANNING") == 0)
    {
    if (this->CommunicationStatus == COMSTATE_PLANNING_CMD_SENT)
      {
      this->SetCommunicationStatus(COMSTATE_PLANNING_CMD_ACK);
      // Stop timer
      }
    }
  else if (strcmp(node->GetTextLabel(), "CALIBRATION") == 0)
    {
    if (this->CommunicationStatus == COMSTATE_CALIBRATION_CMD_SENT)
      {
      this->SetCommunicationStatus(COMSTATE_CALIBRATION_CMD_ACK);
      // Stop timer
      }
    }
  else if (strcmp(node->GetTextLabel(), "TARGETING") == 0)
    {
    if (this->CommunicationStatus == COMSTATE_TARGETING_CMD_SENT)
      {
      this->SetCommunicationStatus(COMSTATE_TARGETING_CMD_ACK);
      // Stop timer
      }
    }
  else if (strcmp(node->GetTextLabel(), "MANUAL") == 0)
    {
    if (this->CommunicationStatus == COMSTATE_MANUAL_CMD_SENT)
      {
      this->SetCommunicationStatus(COMSTATE_MANUAL_CMD_ACK);
      // Stop timer
      }
    }

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::ProcStatus(vtkMRMLIGTLStatusNode * node)
{
  if (!node)
    {
    return 0;
    }

  if (strcmp(node->GetName(), "START_UP"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_STARTUP_CMD_DONE);
      }
    else
      {
      // ERROR
      }
    }
  else if (strcmp(node->GetName(), "CALIBRATION"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_CALIBRATION_TRANS_DONE);
      }
    else
      {
      // ERROR
      }
    }
  else if (strcmp(node->GetName(), "TARGETING"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_TARGETING_CMD_DONE);
      }
    else if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_NOT_READY)
      {
      // ERROR: not able to enter targeting mode (i.e. calibration not received, reply with Code:13 (Device Not Ready)
      }
    }
  else if (strcmp(node->GetName(), "TARGET"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_TARGETING_TARGET_DONE);
      }
    else if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_NOT_READY)
      {
      //this->SetCommunicationStatus(COMSTATE_TARGETING_TARGET_DONE);
      }
    else if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_CONFIG_ERROR)
      {
      //this->SetCommunicationStatus(COMSTATE_TARGETING_TARGET_DONE);
      }
    else
      {
      // ERROR
      }
    }
  else if (strcmp(node->GetName(), "MOVE_TO_TARGET"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_TARGETING_MOVE_DONE);
      }
    else
      {
      // ERROR
      }
    }
  else if (strcmp(node->GetName(), "MANUAL"))
    {
    if (node->GetCode() == vtkMRMLIGTLStatusNode::STATUS_OK)
      {
      this->SetCommunicationStatus(COMSTATE_MANUAL_CMD_DONE);
      }
    else
      {
      // ERROR
      }
    }
  else if (strcmp(node->GetName(), "STOP"))
    {
    // TODO
    }
  else if (strcmp(node->GetName(), "EMERGENCY"))
    {
    // TODO
    }
  else if (strcmp(node->GetName(), "ERROR"))
    {
    // TODO
    }
  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLIGTLSessionManagerNode::ProcCurrentPositionTransform(vtkMRMLLinearTransformNode * node)
{
  if (!node)
    {
    return 0;
    }

  /// Set current position
  return 1;
}


