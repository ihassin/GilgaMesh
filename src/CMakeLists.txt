cmake_minimum_required(VERSION 3.2)

# cross compiling configuration
SET(CMAKE_SYSTEM_NAME Generic)
include(CMakeForceCompiler)

# set compiler and paths
CMAKE_FORCE_C_COMPILER(${DEPLOY_PATH}/gcc-arm-none-eabi/bin/arm-none-eabi-gcc GNU)
SET(TOOLS_PATH              ${DEPLOY_PATH}/gcc-arm-none-eabi/bin)
SET(LINKER_SCRIPT           ${LINKER_PATH}/gilgamesh.ld)
SET(COMPILER_PATH           ${DEPLOY_PATH}/gcc-arm-none-eabi/)
SET(OUTPUT_FILE_NAME        GilgaMesh)
find_program(LINKER ${TOOLS_PATH}/arm-none-eabi-ld)
find_program(OBJCOPY ${TOOLS_PATH}/arm-none-eabi-objcopy)
find_program(SIZE ${TOOLS_PATH}/arm-none-eabi-size)
find_program(GDB ${TOOLS_PATH}/arm-none-eabi-gdb)
find_program(OBJDUMP ${TOOLS_PATH}/arm-none-eabi-objdump)
find_program(NM ${TOOLS_PATH}/arm-none-eabi-nm)
find_program(GCOV ${TOOLS_PATH}/arm-none-eabi-gcov)
find_program(GPROF ${TOOLS_PATH}/arm-none-eabi-gprof)
find_program(STRIP ${TOOLS_PATH}/arm-none-eabi-strip)
find_program(AS ${TOOLS_PATH}/arm-none-eabi-as)
find_program(READELF ${TOOLS_PATH}/arm-none-eabi-readelf)

# set compiler options
SET(CMAKE_C_FLAGS_DEBUG "-DNRF_LOG_USES_RTT -D${TARGET_BOARD} -DSOFTDEVICE_PRESENT -DNRF51422 -DNRF51 -D__HEAP_SIZE=0 -DS130 -DBLE_STACK_SUPPORT_REQD -DSWI_DISABLE0 -DBSP_UART_SUPPORT -mcpu=cortex-m0 -mthumb -mabi=aapcs --std=gnu11 -Wall -Og -g3 -mfloat-abi=soft -ffunction-sections -fdata-sections -flto -fno-strict-aliasing -fno-builtin --short-enums")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-DNRF_LOG_USES_RTT -D${TARGET_BOARD} -DSOFTDEVICE_PRESENT -DNRF51422 -DNRF51 -D__HEAP_SIZE=0 -DS130 -DBLE_STACK_SUPPORT_REQD -DSWI_DISABLE0 -DBSP_UART_SUPPORT -mcpu=cortex-m0 -mthumb -mabi=aapcs --std=gnu11 -Wall -Og -g1 -mfloat-abi=soft -ffunction-sections -fdata-sections -flto -fno-strict-aliasing -fno-builtin --short-enums")
SET(CMAKE_C_FLAGS_RELEASE "-DNRF_LOG_USES_RTT -D${TARGET_BOARD} -DSOFTDEVICE_PRESENT -DNRF51422 -DNRF51 -D__HEAP_SIZE=0 -DS130 -DBLE_STACK_SUPPORT_REQD -DSWI_DISABLE0 -DBSP_UART_SUPPORT -mcpu=cortex-m0 -mthumb -mabi=aapcs --std=gnu11 -Wall -O2 -g0 -mfloat-abi=soft -ffunction-sections -fdata-sections -flto -fno-strict-aliasing -fno-builtin --short-enums -fno-exceptions")
SET(CMAKE_C_FLAGS_MINSIZEREL "-DNRF_LOG_USES_RTT -D${TARGET_BOARD} -DSOFTDEVICE_PRESENT -DNRF51422 -DNRF51 -D__HEAP_SIZE=0 -DS130 -DBLE_STACK_SUPPORT_REQD -DSWI_DISABLE0 -DBSP_UART_SUPPORT -mcpu=cortex-m0 -mthumb -mabi=aapcs --std=gnu11 -Wall -Os -g0 -mfloat-abi=soft -ffunction-sections -fdata-sections -flto -fno-strict-aliasing -fno-builtin --short-enums -fno-exceptions")

# set linker options
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
SET(CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m0 -mthumb -mabi=aapcs -Os -fmessage-length=0 -fsigned-char -ffunction-sections -flto -fno-move-loop-invariants -Wextra -T${LINKER_SCRIPT} -Xlinker --gc-sections -Wl,--gc-sections -Wl,-Map,\"${CMAKE_BINARY_DIR}/GilgaMesh.map\" --specs=nano.specs -lgcc -lc -lnosys")
SET(CMAKE_ASM_FLAGS "-mcpu=cortex-m0 -mthumb -x assembler-with-cpp -DBOARD_PCA10028 -DSOFTDEVICE_PRESENT -DNRF51 -DS130 -DBLE_STACK_SUPPORT_REQD -DBSP_DEFINES_ONLY")


# generate outputs macro
macro(GEN_OUTPUTS target)
    get_filename_component(name ${target} NAME_WE)
    #copy elf and map to build output
    add_custom_command(
        OUTPUT ${name}_copy DEPENDS ${target}
        COMMAND cp ${target} ${BUILD_PATH}/${target}
        COMMAND cp ${CMAKE_BINARY_DIR}/${name}.map ${BUILD_PATH}/${name}.map
    )
    add_custom_command(
        OUTPUT ${name}.size DEPENDS ${target}
        COMMAND ${SIZE} ${target}
        COMMAND ${SIZE} --format=berkeley ${target} > ${BUILD_PATH}/${name}.size
    )
    #generate .bin
    add_custom_command(
        OUTPUT ${OUTPUT_FILE_NAME}.bin DEPENDS ${target}
        COMMAND ${OBJCOPY} -O binary ${target} ${BUILD_PATH}/${name}.bin
    )
    #generate .hex
    add_custom_command(
        OUTPUT ${OUTPUT_FILE_NAME}.hex DEPENDS ${target}
        COMMAND ${OBJCOPY} -O ihex ${target} ${BUILD_PATH}/${name}.hex
    )
    #generate .dump
    add_custom_command(
        OUTPUT ${name}.dump DEPENDS ${target}
        COMMAND ${OBJDUMP} -DSC ${target} > ${BUILD_PATH}/${name}.dump
    )
    #generate .header
    add_custom_command(
        OUTPUT ${name}.header DEPENDS ${target}
        COMMAND ${READELF} -h ${target} > ${BUILD_PATH}/${name}.header
    )
    add_custom_target(
        ${name}_outputs ALL DEPENDS ${name}_copy ${name}.size ${name}.bin ${name}.hex ${name}.header #${name}.dump
    )

endmacro(GEN_OUTPUTS)

# includes common to all targets

include_directories(${GilgaMesh_SOURCE_DIR}/inc)
include_directories(${GilgaMesh_SOURCE_DIR}/config)

# includes for nordic nrf51
include_directories(${COMPONENTS}/ble/ble_db_discovery)
include_directories(${COMPONENTS}/ble/ble_radio_notification)
include_directories(${COMPONENTS}/ble/common)
include_directories(${COMPONENTS}/ble/device_manager)
include_directories(${COMPONENTS}/device)
include_directories(${COMPONENTS}/libraries/button)
include_directories(${COMPONENTS}/libraries/experimental_section_vars)
include_directories(${COMPONENTS}/libraries/fds)
include_directories(${COMPONENTS}/libraries/fifo)
include_directories(${COMPONENTS}/libraries/fstorage)
include_directories(${COMPONENTS}/libraries/gpiote)
include_directories(${COMPONENTS}/libraries/hardfault)
include_directories(${COMPONENTS}/libraries/ppi)
include_directories(${COMPONENTS}/libraries/pwm)
include_directories(${COMPONENTS}/libraries/scheduler)
include_directories(${COMPONENTS}/libraries/timer)
include_directories(${COMPONENTS}/libraries/twi)
include_directories(${COMPONENTS}/libraries/uart)
include_directories(${COMPONENTS}/libraries/util)
include_directories(${COMPONENTS}/libraries/log)
include_directories(${COMPONENTS}/libraries/log/src)
include_directories(${COMPONENTS}/nfc/ndef/parser/record)
include_directories(${COMPONENTS}/nfc/ndef/generic/message)
include_directories(${COMPONENTS}/nfc/t2t_parser)
include_directories(${COMPONENTS}/nfc/ndef/parser/message)
include_directories(${COMPONENTS}/nfc/ndef/generic/record)
include_directories(${COMPONENTS}/softdevice/common/softdevice_handler)
include_directories(${COMPONENTS}/softdevice/s130/headers)
include_directories(${COMPONENTS}/softdevice/s130/headers/nrf51)
include_directories(${COMPONENTS}/toolchain)
include_directories(${COMPONENTS}/toolchain/cmsis/include)
include_directories(${COMPONENTS}/toolchain/arm)
include_directories(${COMPONENTS}/toolchain/gcc)
include_directories(${COMPONENTS}/drivers_nrf/clock)
include_directories(${COMPONENTS}/drivers_nrf/common)
include_directories(${COMPONENTS}/drivers_nrf/config)
include_directories(${COMPONENTS}/drivers_nrf/delay)
include_directories(${COMPONENTS}/drivers_nrf/gpiote)
include_directories(${COMPONENTS}/drivers_nrf/hal)
include_directories(${COMPONENTS}/drivers_nrf/ppi)
include_directories(${COMPONENTS}/drivers_nrf/timer)
include_directories(${COMPONENTS}/drivers_nrf/twi_master)
include_directories(${COMPONENTS}/drivers_nrf/uart)
include_directories(${EXTERNAL}/segger_rtt)
include_directories(${SOFTDEVICE}/include)
include_directories(${SOFTDEVICE}/include/nrf51)

# link directories
link_directories(${LINKER_PATH})
link_directories(${COMPONENTS}/toolchain/gcc)

# add framework sources
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/ble/common/ble_conn_state.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/clock/nrf_drv_clock.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/common/nrf_drv_common.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/timer/nrf_drv_timer.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/gpiote/nrf_drv_gpiote.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/ppi/nrf_drv_ppi.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/twi_master/nrf_drv_twi.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/drivers_nrf/uart/nrf_drv_uart.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/button/app_button.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/fds/fds.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/fstorage/fstorage.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/gpiote/app_gpiote.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/hardfault/nrf51/handler/hardfault_handler_gcc.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/log/src/nrf_log_frontend.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/log/src/nrf_log_backend_serial.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/pwm/app_pwm.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/scheduler/app_scheduler.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/timer/app_timer.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/timer/app_timer_appsh.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/twi/app_twi.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/uart/app_uart.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/util/app_error.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/util/app_util_platform.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/libraries/util/sdk_mapped_flags.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/ndef/generic/message/nfc_ndef_msg.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/ndef/parser/message/nfc_ndef_msg_parser.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/ndef/parser/message/nfc_ndef_msg_parser_local.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/ndef/generic/record/nfc_ndef_record.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/ndef/parser/record/nfc_ndef_record_parser.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/nfc/t2t_parser/nfc_t2t_parser.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/softdevice/common/softdevice_handler/softdevice_handler.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/softdevice/common/softdevice_handler/softdevice_handler_appsh.c")
list(APPEND GilgaMesh_SOURCES "${COMPONENTS}/toolchain/system_nrf51.c")
list(APPEND GilgaMesh_SOURCES "${EXTERNAL}/segger_rtt/SEGGER_RTT.c")
list(APPEND GilgaMesh_SOURCES "${EXTERNAL}/segger_rtt/SEGGER_RTT_printf.c")

# compile the ARM6 assembly startup code through the C compiler
list(APPEND GilgaMesh_SOURCES "gcc_startup_nrf51.s")
set_property(SOURCE gcc_startup_nrf51.s PROPERTY LANGUAGE C)

# add system shim sources
list(APPEND GilgaMesh_SOURCES "system/delay.c")
list(APPEND GilgaMesh_SOURCES "system/gpio.c")
list(APPEND GilgaMesh_SOURCES "system/i2c.c")
list(APPEND GilgaMesh_SOURCES "system/log.c")
list(APPEND GilgaMesh_SOURCES "system/platform.c")
list(APPEND GilgaMesh_SOURCES "system/pwm.c")
list(APPEND GilgaMesh_SOURCES "system/timer.c")

# add mesh sources
list(APPEND GilgaMesh_SOURCES "command.c")
list(APPEND GilgaMesh_SOURCES "connection.c")
list(APPEND GilgaMesh_SOURCES "conversion.c")
list(APPEND GilgaMesh_SOURCES "device.c")
list(APPEND GilgaMesh_SOURCES "error.c")
list(APPEND GilgaMesh_SOURCES "gap.c")
list(APPEND GilgaMesh_SOURCES "gatt.c")
list(APPEND GilgaMesh_SOURCES "message_types/broadcast_message.c")
list(APPEND GilgaMesh_SOURCES "message_types/heartbeat_message.c")
list(APPEND GilgaMesh_SOURCES "message_types/handshake_message.c")
list(APPEND GilgaMesh_SOURCES "softdevice.c")
list(APPEND GilgaMesh_SOURCES "terminal.c")
list(APPEND GilgaMesh_SOURCES "queue.c")

list(APPEND GilgaMesh_SOURCES "main.c")

# add application sources
list(APPEND GilgaMesh_SOURCES "app/blinky_app.c")

add_executable(${OUTPUT_FILE_NAME}.elf ${GilgaMesh_SOURCES} system/log.c ../inc/system/log.h)
GEN_OUTPUTS(${OUTPUT_FILE_NAME}.elf)

