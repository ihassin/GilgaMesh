#include "gatt.h"
#include "cmocka_includes.h"

void send_to_central_connection(uint16_t originHandle, uint8_t *data, uint16_t dataLength) {
  check_expected(originHandle);
  check_expected(data);
  check_expected(dataLength);
}

void send_to_all_connections(uint16_t originHandle, uint8_t *data, uint16_t dataLength) {
  check_expected(originHandle);
  check_expected(data);
  check_expected(dataLength);
}

void send_to_single_connection(connection *targetConnection, uint8_t *data, uint16_t dataLength) {
  check_expected(targetConnection);
  check_expected(data);
  check_expected(dataLength);
}

bool gatt_initialize(void) {
  return true;
}

void propagate_message(ble_gatts_evt_t *gattsEvent) { }
void send_message_from_connection_queue(connection *targetConnection){ }
void handle_write_event(void * data, uint16_t dataLength) { }
uint8_t register_message_type(write_event_handler handler) {
  return mock_type(uint8_t);
}
