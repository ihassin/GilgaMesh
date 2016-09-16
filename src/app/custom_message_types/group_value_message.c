#include "app/custom_message_types/group_value_message.h"

#include <app_scheduler.h>
#include <stdlib.h>
#include <command.h>
#include "device.h"
#include "gatt.h"

void group_value_message_initialize() {
  mesh_add_terminal_command("vc", "Print current vote configuration", print_current_vote_config);
  mesh_add_terminal_command("grp", "Update group", broadcast_group_value_message);
  mesh_add_terminal_command("val", "Update value", broadcast_group_value_message);
  add_write_event(Custom, receive_group_value_message);
}

void broadcast_group_value_message(char **parsedCommandArray, uint8_t numCommands) {
  UNUSED_PARAMETER(numCommands);

  BleMessageGroupValueReq request;
  memset(&request, 0, sizeof(request));
  request.head.messageType = Custom;
  request.deviceId = (uint32_t) atoll(parsedCommandArray[1]);

  bool isGroupUpdate = (strcmp(parsedCommandArray[0], "grp") == 0) ? true : false;
  if (isGroupUpdate) {
    request.setGroup = true;
    request.newGroup = (uint8_t) atoi(parsedCommandArray[2]);
  } else {
    request.setValue = true;
    request.newValue = (uint8_t) atoi(parsedCommandArray[2]);
  }

  receive_group_value_message(BLE_CONN_HANDLE_INVALID, &request);
  send_to_all_connections(BLE_CONN_HANDLE_INVALID, (uint8_t *)&request, sizeof(BleMessageGroupValueReq));
}

MessagePropagationType receive_group_value_message(uint16_t connectionHandle, uint8_t *dataPacket) {
  UNUSED_PARAMETER(connectionHandle);
  BleMessageGroupValueReq *req = (BleMessageGroupValueReq *) dataPacket;
  if (req->deviceId == get_raw_device_id()) {
    if (req->setGroup) update_voting_group(req->newGroup);
    if (req->setValue) update_voting_value(req->newValue);
    return DoNotPropagate;
  }
  return PropagateToAll;
}