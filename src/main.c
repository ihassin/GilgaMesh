#include "main.h"

#include <app/custom_message_types/group_value_message.h>
#include <system/log.h>
#include "app/buzzer.h"
#include "app/feedback.h"
#include "app/led.h"
#include "app/rtc.h"
#include "message_types/heartbeat_message.h"
#include "connection.h"
#include "gap.h"
#include "softdevice.h"
#include "terminal.h"

// This variable ensures that the linker script will write the bootloader start
// address to the UICR register. This value will be written in the HEX file and
// thus written to UICR when MeshyMesh is flashed into the chip. */

#ifndef TESTING
volatile uint32_t m_uicr_bootloader_start_address __attribute__ ((section(".uicrBootStartAddress"))) = BOOTLOADER_REGION_START;
#endif

void HardFault_Handler(void)
{
  display_failure_feedback();
}


void initialize() {
  NRF_LOG_INIT();
  MESH_LOG("\r\n[[ MeshyMesh is booting ]]\r\n");
  APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

  init_module("softdevice", softdevice_initialize);
  init_module("timers", timer_initialize);
  init_module("leds", led_initialize);
#ifdef IS_PROD_BOARD
  init_module("buzzer", buzzer_initialize);
#endif
  init_module("terminal", terminal_initialize);
  init_module("commands", command_initialize);
  init_module("feedback", feedback_initialize);
  init_module("RTC", rtc_init);
  init_module("connections", connections_initialize);
  init_module("GATT", gatt_initialize);
  init_module("GAP", gap_initialize);
  init_module("heartbeat timer", heartbeat_initialize);

  mesh_add_terminal_command("grp", "Update group", broadcast_group_value_update);
  mesh_add_terminal_command("val", "Update value", broadcast_group_value_update);
  mesh_add_terminal_command("fun", "Play a song!", display_fun_feedback);
  add_write_event(Custom, receive_group_value_update);

  MESH_LOG("System ready.\r\n");
}

init_module(char* module_name, void (*function)()) {
  MESH_LOG("Init %s... \r\n", module_name);
  (*function)();
}

void run() {
  terminal_process_input();
  app_sched_execute();

  EC(sd_app_evt_wait());
}

#ifndef TESTING // exclude entry point from unit tests
int main()
{
  initialize();
  display_successful_start_up_feedback();
  //determine if there was a failure and then try to give "failure" feedback
  //display_catastrophic_failure_feedback();

  for(;;) {
    run();
  }
}
#endif
