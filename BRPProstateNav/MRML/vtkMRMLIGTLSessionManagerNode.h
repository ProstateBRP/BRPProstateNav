/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#ifndef __vtkMRMLIGTLSessionManagerNode_h
#define __vtkMRMLIGTLSessionManagerNode_h

// MRML includes
#include "vtkMRMLNode.h"
#include "vtkSlicerBRPProstateNavModuleMRMLExport.h"

class vtkMatrix4x4;
class vtkIGTLToMRMLString;
class vtkIGTLToMRMLStatus;
class vtkMRMLIGTLConnectorNode;

/// \brief MRML node to manage OpenIGTLink 
///
/// Configure vtkMRMLIGTLConnectorNode and manage incoming and outgoing
/// data during a OpenIGTLink session.
class VTK_SLICER_BRPPROSTATENAV_MODULE_MRML_EXPORT vtkMRMLIGTLSessionManagerNode : public vtkMRMLNode
{
public:
  static vtkMRMLIGTLSessionManagerNode *New();

  vtkTypeMacro(vtkMRMLIGTLSessionManagerNode,vtkMRMLNode);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// 
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// 
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "IGTLSessionManager";};

  ///
  /// Set and configure MRMLIGTLconnector node.
  /// Once the new connector node is set, RegisterMessageNodes() is called
  /// to register all message nodes used in the communication.
  void SetAndObserveConnectorNodeID(const char *connectorNodeID);

  ///
  /// Set a reference to transform node
  void AddAndObserveMessageNodeID(const char *transformNodeID);

  /// 
  /// Associated transform MRML node
  //vtkMRMLTransformNode* GetParentTransformNode();

  /// 
  /// alternative method to propagate events generated in Transform nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );

  //BTX
  virtual void OnNodeReferenceAdded(vtkMRMLNodeReference *reference);

  virtual void OnNodeReferenceRemoved(vtkMRMLNodeReference *reference);

  virtual void OnNodeReferenceModified(vtkMRMLNodeReference *reference);
  //ETX

  /// Get referenced connector node id
  const char *GetConnectorNodeID();

  /// Application specific API (will be moved to child class in the future)
  ///
  enum
  {
    StatusChangeEvent  = 20000,
    TimeOutEvent       = 20001,
    DeviceErrorEvent   = 20002,
    NewMessageEvent    = 20003  // New messages other than status change
  };

  // Communication status (not device status)
  enum
  {
    COMSTATE_NONE = 0,
    COMSTATE_STARTUP_CMD_SENT,
    COMSTATE_STARTUP_CMD_ACK,
    COMSTATE_STARTUP_CMD_DONE, // SUCCESS
    COMSTATE_PLANNING_CMD_SENT,
    COMSTATE_PLANNING_CMD_ACK,
    COMSTATE_CALIBRATION_CMD_SENT,
    COMSTATE_CALIBRATION_CMD_ACK,
    COMSTATE_CALIBRATION_TRANS_SENT,
    COMSTATE_CALIBRATION_TRANS_ACK,
    COMSTATE_CALIBRATION_TRANS_DONE, // SUCCESS
    COMSTATE_TARGETING_CMD_SENT,
    COMSTATE_TARGETING_CMD_ACK,
    COMSTATE_TARGETING_CMD_DONE, // SUCCESS or ERROR
    COMSTATE_TARGETING_TARGET_SENT,
    COMSTATE_TARGETING_TARGET_ACK,
    COMSTATE_TARGETING_TARGET_DONE, // SUCCESS or ERROR (not ready or invalid target)
    COMSTATE_TARGETING_MOVE_SENT,
    COMSTATE_TARGETING_MOVE_ACK,
    COMSTATE_TARGETING_MOVE_DONE, // SUCCESS
    COMSTATE_MANUAL_MOVE_SENT,
    COMSTATE_MANUAL_MOVE_ACK,
    COMSTATE_MANUAL_MOVE_DONE, // SUCCESS
    COMSTATE_STOP_CMD_SENT,
    COMSTATE_STOP_CMD_ACK,
    COMSTATE_EMERGENCY_CMD_SENT,
    COMSTATE_EMERGENCY_CMD_ACK,
    COMSTATE_GET_STATUS_SENT,
    COMSTATE_GET_STATUS_ACK,
  };

  inline int GetComStatus() { return this->CommunicationStatus; };

  virtual int SendStartUpCommand();
  virtual int SendPlanningCommand();
  virtual int SendCalibrationCommand();
  virtual int SendCalibrationTransform(vtkMatrix4x4* transform);
  virtual int SendTargetingCommand();
  virtual int SendTargetingTarget(double r, double a, double s);
  virtual int SendTargetingMove();
  virtual int SendManualCommand();
  virtual int SendStopCommand();
  virtual int SendEmergencyCommand();
  virtual int SendGetStatusCommand();

protected:
  vtkMRMLIGTLSessionManagerNode();
  ~vtkMRMLIGTLSessionManagerNode();
  vtkMRMLIGTLSessionManagerNode(const vtkMRMLIGTLSessionManagerNode&);
  void operator=(const vtkMRMLIGTLSessionManagerNode&);

  ///
  /// Add a message node to the MRML scene and register to the connector node. 
  /// This is a utility function for RegisterMessageNodes().
  /// io must be either vtkMRMLIGTLConnectorNode::IO_OUTGOING or 
  /// vtkMRMLIGTLConnectorNode::IO_INCOMING
  int ConfigureMessageNode(vtkMRMLIGTLConnectorNode* cnode, vtkMRMLNode* node, int io);

  ///
  /// Register message nodes to the converter node
  /// This function will be called whenever a new connector node is set.
  /// This function has to be implemented per application.
  virtual int RegisterMessageNodes(vtkMRMLIGTLConnectorNode* cnode);

  ///
  /// Check connection status of the OpenIGTLink connector. 
  /// Return non-zero value if it is connected, otherwise return 0.
  int IsConnected();

  //----------------------------------------------------------------
  // Reference role strings
  //----------------------------------------------------------------
  char* ConnectorNodeReferenceRole;
  char* ConnectorNodeReferenceMRMLAttributeName;

  vtkSetStringMacro(ConnectorNodeReferenceRole);
  vtkGetStringMacro(ConnectorNodeReferenceRole);

  vtkSetStringMacro(ConnectorNodeReferenceMRMLAttributeName);
  vtkGetStringMacro(ConnectorNodeReferenceMRMLAttributeName);

  char* MessageNodeReferenceRole;
  char* MessageNodeReferenceMRMLAttributeName;

  vtkSetStringMacro(MessageNodeReferenceRole);
  vtkGetStringMacro(MessageNodeReferenceRole);

  vtkSetStringMacro(MessageNodeReferenceMRMLAttributeName);
  vtkGetStringMacro(MessageNodeReferenceMRMLAttributeName);

private:
  // OpenIGTLink connector Node
  char* ConnectorNodeIDInternal;
  vtkSetStringMacro(ConnectorNodeIDInternal);
  vtkGetStringMacro(ConnectorNodeIDInternal);

  // Outgoing message nodes
  char* OutCommandNodeIDInternal;
  vtkSetStringMacro(OutCommandNodeIDInternal);
  vtkGetStringMacro(OutCommandNodeIDInternal);

  char* OutCalibrationTransformNodeIDInternal;
  vtkSetStringMacro(OutCalibrationTransformNodeIDInternal);
  vtkGetStringMacro(OutCalibrationTransformNodeIDInternal);

  char* OutTargetTransformQueryNodeIDInternal;
  vtkSetStringMacro(OutTargetTransformQueryNodeIDInternal);
  vtkGetStringMacro(OutTargetTransformQueryNodeIDInternal);

  // Incoming message nodes
  char* InAckknowledgeStringNodeIDInternal;
  vtkSetStringMacro(InAckknowledgeStringNodeIDInternal);
  vtkGetStringMacro(InAckknowledgeStringNodeIDInternal);

  char* InCommandStringNodeIDInternal;
  vtkSetStringMacro(InCommandStringNodeIDInternal);
  vtkGetStringMacro(InCommandStringNodeIDInternal);

  char* InStartUpStatusNodeIDInternal;
  vtkSetStringMacro(InStartUpStatusNodeIDInternal);
  vtkGetStringMacro(InStartUpStatusNodeIDInternal);

  char* InCalibrationStatusNodeIDInternal;
  vtkSetStringMacro(InCalibrationStatusNodeIDInternal);
  vtkGetStringMacro(InCalibrationStatusNodeIDInternal);

  char* InTargetingStatusNodeIDInternal;
  vtkSetStringMacro(InTargetingStatusNodeIDInternal);
  vtkGetStringMacro(InTargetingStatusNodeIDInternal);
  
  char* InMovingStatusNodeIDInternal;
  vtkSetStringMacro(InMovingStatusNodeIDInternal);
  vtkGetStringMacro(InMovingStatusNodeIDInternal);

  char* InManualStatusNodeIDInternal;
  vtkSetStringMacro(InManualStatusNodeIDInternal);
  vtkGetStringMacro(InManualStatusNodeIDInternal);

  char* InErrorStatusNodeIDInternal;
  vtkSetStringMacro(InErrorStatusNodeIDInternal);
  vtkGetStringMacro(InErrorStatusNodeIDInternal);

  char* InCurrentPositionTransformNodeIDInternal;
  vtkSetStringMacro(InCurrentPositionTransformNodeIDInternal);
  vtkGetStringMacro(InCurrentPositionTransformNodeIDInternal);

  vtkIGTLToMRMLString * StringMessageConverter;
  vtkIGTLToMRMLStatus * StatusMessageConverter;

  int CommunicationStatus; // COMSTATE_*
  
};

#endif
