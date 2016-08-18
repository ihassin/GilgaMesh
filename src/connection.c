#include <ble_conn_state.h>
#include "connection.h"
#include "gap.h"


static connection (*activeConnections)[MAX_TOTAL_CONNECTIONS];

static inline uint32_t get_device_id() {
  // define out direct device memory access when testing
  #ifdef TESTING
    return 0xDEADBEEF; // arbitrary device id for tests
  #else
    return NRF_FICR->DEVICEID[1];
  #endif
}

void connections_initialize() {
  deviceId = get_device_id();
  nodeName = malloc(NODE_NAME_SIZE);
  set_node_name_from_device_id(deviceId, nodeName);
  activeConnections = malloc(sizeof(*activeConnections));
  memset(activeConnections, 0, sizeof(*activeConnections));

  NRF_LOG_PRINTF("NodeName: %s, DeviceId: %u\r\n", nodeName, deviceId);
}


connection* set_connection(uint16_t connectionHandle, ConnectionType connectionType) {
  for (int i = 0; i < MAX_TOTAL_CONNECTIONS; i++){
    connection *conn = &(*activeConnections)[i];
    if (!conn->active){
      conn->handle = connectionHandle;
      conn->type = connectionType;
      conn->active = true;
      return conn;
    }
  }
  return NULL;
}


connection* find_active_connection_by_handle(uint16_t connectionHandle) {
  for (int i = 0; i < MAX_TOTAL_CONNECTIONS; i++){
    connection *connection = &(*activeConnections)[i];
    if (connection->active && connection->handle == connectionHandle){
      return connection;
    }
  }
  return 0;
}


ConnectionType unset_connection(uint16_t connectionHandle) {
  connection *lostConnection = find_active_connection_by_handle(connectionHandle);
  if (lostConnection != 0){
    ConnectionType lostConnectionType = lostConnection->type;
    memset(lostConnection, 0, sizeof(connection));
    return lostConnectionType;
  } else {
    return INVALID;
  }
}


char* get_connection_type_name(uint16_t connHandle) {
  switch (ble_conn_state_role(connHandle))
  {
    case BLE_GAP_ROLE_CENTRAL:
      return "CENTRAL";
    case BLE_GAP_ROLE_PERIPH:
      return "PERIPHERAL";
    default:
      return "INVALID";
  }
}


void print_single_connection_info(uint16_t connectionHandle) {
  if (!is_connection_active(connectionHandle)) return;

  char result[50];
  char handle[6];
  sprintf(handle, "%u", connectionHandle);

  strcpy(result, "   [");
  strcat(result, get_connection_type_name(connectionHandle));
  strcat(result, "] [");
  strcat(result, handle);
  strcat(result, "] ");
  connection *conn = find_active_connection_by_handle(connectionHandle);
  if (conn->deviceId != 0) {
    char version[9];
    sprintf(version, "v%u.%u", conn->majorVersion, conn->minorVersion);

    char *nodeName = malloc(NODE_NAME_SIZE);
    set_node_name_from_device_id(conn->deviceId, nodeName);
    strcat(result, nodeName);
    strcat(result, ", ");
    strcat(result, version);
    free(nodeName);
  } else {
    strcat(result, "Unknown Node");
  }
  strcat(result, "\0");

  NRF_LOG_PRINTF("%s\r\n", result);
}

bool is_connected() {
  return ble_conn_state_n_connections() > 0;
}

bool central_connection_active() {
  return ble_conn_state_n_peripherals() > 0;
}

bool all_peripheral_connections_active() {
  return ble_conn_state_n_centrals() >= MAX_PERIPHERAL_CONNECTIONS;
}

bool is_connection_active(uint16_t connectionHandle) {
  return ble_conn_state_status(connectionHandle) == BLE_CONN_STATUS_CONNECTED;
}

uint32_t get_central_connection_device_id() {
  for(int i = 0; i < MAX_TOTAL_CONNECTIONS; i++) {
    connection *conn = &(*activeConnections)[i];
    if(conn->type == CENTRAL) {
      return conn->deviceId;
    }
  }
  return 0;
}

void print_all_connections(char** commandArray) {
  UNUSED_PARAMETER(commandArray);

  NRF_LOG_PRINTF("Connection details: %s\r\n", is_connected() ? "" : "DISCONNECTED");
  sdk_mapped_flags_key_list_t connKeyList = ble_conn_state_conn_handles();
  for (int i = 0; i < connKeyList.len; i++) {
    print_single_connection_info(connKeyList.flag_keys[i]);
  }
}
