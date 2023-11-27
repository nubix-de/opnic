#!/usr/bin/env bash
# This file contains functions used accross the projects
# Language specific functions should be implemented inside each _build.sh
# Copyright nubix Software-Design GmbH
# Author Vinícius Gabriel Linden
# Date 2022-10-18

set -eu
set -o pipefail

# rshell changes attributes, if we use pkill, these will not be restored
attributes="$(stty -g 2>/dev/null || true)"
readonly attributes
_restore_attributes() {
   stty "${attributes}" 2>/dev/null || true
}
trap _restore_attributes EXIT

_log() {
    [[ "${#}" -lt 2 ]] && return 1
    level="${1}"
    shift
    [[ "${FUNCNAME[1]}" =~ ^_ ]] || \
        FUNCTION=" ${FUNCNAME[1]}"
    declare -r msg="\033[1m[${PROJECT}${FUNCTION:-}]:\033[0m ${*}"
    case "${level}" in
        error)
            echo -e "\033[31m${msg}" 1>&2
            ;;
        warn*)
            echo -e "\033[33m${msg}" 1>&2
            ;;
        att*) # attention
            echo -e "\033[36m${msg}"
            ;;
        *)
            echo -e "${msg[@]}"
            ;;
    esac
}

_element_exists() {
  local element="${1}"
  shift
  local array=("${@}")

  for i in "${array[@]}"; do
    [[ "${element}" == "${i}" ]] &&
      return 0
  done
  return 1
}

_print_baud() {
    # this function is right now only available for C(++)
    _log attention "$(awk \
        '/UART_BAUD_RATE/ {gsub("[()]", ""); print "UART baudrate: " $3 " bps"}' \
        "${BOARD_FILE}")"
}

_open() {
    local cmd
    if command -v xdg-open &>/dev/null; then
        cmd="xdg-open"
    elif command -v open &>/dev/null; then
        cmd="open"
    elif command -v cygstart &>/dev/null; then
        cmd="cygstart"
    fi
    [[ "${cmd}" == "" ]] &&
        _log warn "no open tool found" &&
        return 0
    for i in "${@}"; do
        # this is a non-essential command, it is ok if if does not work
        "${cmd}" "${i}" 2>/dev/null || true
    done
}

_kill() {
    [[ "${#}" -eq 0 ]] &&
        return 0
    killall -KILL "${@}" &>/dev/null || true
    # TODO implement wait
    # wait $$
}

clean() {
    _log info "cleaning directories"
    rm -rf "${BUILD_DIR}" &>/dev/null || true
    if [[ -e "${BUILD_DIR}" ]]; then
        _log error "can not delete _build dir at \'${BUILD_DIR}\'"
        return 1
    fi
    _log info "not cleaning the pico-sdk directory"
}

clear() {
    clean
}

purge() {
    _log info "purging all cached files and directories"
    rm -rf "${BUILD_DIR}/" \
           "${ROOT_DIR}/compile_commands.json" \
           "${ROOT_DIR}/deploy/" \
           "${ROOT_DIR}/firmware/pico_sdk/" \
           &>/dev/null || true
}

_build() {
    # usage: `_build [build_generate] [FLAG1, [FLAG2, ...]] [custom_build] ...

    mkdir -p "${BUILD_DIR}"
    # cd "${BUILD_DIR}"

    msg=("cmake options:")
    while read -r line; do
        msg+=("${line}")
    done < <( env | grep 'CMAKE' || echo "None" )
    _log info "${msg[@]}"

    CMD=("cmake" "${@}" "-S" "${ROOT_DIR}" "-B" "${BUILD_DIR}")
    _log info "${CMD[@]}"
    eval "${CMD[@]}"

    CMD=("cmake" "--build" "${BUILD_DIR}" "--target" "${PROJECT}")
    _log info "${CMD[@]}"
    eval "${CMD[@]}"

    _log info "finished building"
}

debug() {
    _log info "start building for debug"
    _build -D CMAKE_BUILD_TYPE=Debug
    _print_baud
}

release() {
    _log info "start building for release"
    _build -D CMAKE_BUILD_TYPE=Release
    _print_baud
}

serial() {
    # because this gets the baudrate dynamically, via a C++ specific file, it
    # should be declared here
    _log info "opening up serial communication"
    baudrate=$(awk \
            '/UART_BAUD_RATE/ {gsub("[()]", ""); print $3}' \
            "${BOARD_FILE}")

    if device="$(ls "/dev/ttyACM0" 2>/dev/null)"; then
        _log info "trying ${device}"
    elif device=($(ls "/dev/serial/by-id/"*"UART"* 2>/dev/null)); then
        _log info "using UART bridge"
    elif device=($(ls "/dev/serial/by-id/usb-Raspberry_Pi_Picoprobe_"* 2>/dev/null)); then
        _log info "using picocom as the UART bridge"
    else
        _log warn "no serial device found"
        return 1
    fi

    if command -v picocom &>/dev/null; then
        _log info "picocom host software detected"
        picocom --quiet --echo -b "${baudrate}" "${device}"
    elif command -v minicom &>/dev/null; then
        _log info "minicom host software detected"
        minicom -D "${device}" -b "${baudrate}"
    else
        _log warn "did not recognize host-side serial software"
        return 1
    fi
}

uf2() {
    _log info "programming Pico via UF2 mount"

    if [[ "${#}" -eq 0 ]]; then
        uf2="$(_file_find ".uf2")"
        # TODO function is continue on failure return
        [[ "${uf2}" == "" ]] &&
            return 1
        echo "uf2 file is ${uf2}"
    else
        uf2="${1}"
    fi

    if rpi=$(ls "/dev/disk/by-label/RPI-RP2" 2>/dev/null); then
        :
    elif rpi=$(ls "/dev/disk/by-label/rp2040ulp*" 2>/dev/null); then
        :
    else
        _log error "pico microcontroller is not connected"
        return 1
    fi
    # RPI was connected with BOOTSEL

    rpi="$(readlink -f -- "${rpi}")"
    _log info "pico is ${rpi}"

    rpi_mount="$(lsblk -no MOUNTPOINT "${rpi}")"
    if [[ ! "${rpi_mount}" ]]; then
        # not mounted
        rpi_mount="/tmp/rp-${RANDOM}"
        mkdir -p "${rpi_mount}"
        sudo mount "${rpi}" "${rpi_mount}"
    fi

    _log info "pico mounted at ${rpi_mount}"
    _log info "programming ${uf2} file"
    sudo cp "${uf2}" "${rpi_mount}" || true
    sudo umount "${rpi}"
}

picoprobe_openocd() {
    _kill openocd
    openocd \
        -s "${ROOT_DIR}/utils/" \
        -f "probe.cfg"
}

_file_find() {
    [[ "${#}" -eq 0 ]] &&
        _log error "missing file extension" &&
        return 1

    mapfile -t path < <(find "${BUILD_DIR}/firmware/" -type f -iname "*${1}")

    [[ "${#path[@]}" -eq 0 ]] &&
        _log error "file not found" &&
        return 1

    if [[ "${#path[@]}" -ne 1 ]]; then
        _log warn "multiple files were found"
        if [[ "${PROJECT}" != "OPNIC" ]]; then
            _log info "searching for ${PROJECT}${1}" &&
            if result=$(_file_find "${PROJECT}${1}"); then
                echo "${result}"
                return 0
            fi
        fi
        return 1
    fi

    echo "${path[0]}"
}

swd() {
    _log info "programming via a picoprobe"

    if [[ "${#}" -eq 0 ]]; then
        elf="$(_file_find ".elf")"
        # TODO function is continue on failure return
        [[ "${elf}" == "" ]] &&
            return 1
        echo "elf file is ${elf}"
    else
        elf="${1}"
    fi

    _kill openocd
    openocd \
        -s "${ROOT_DIR}/utils/" \
        -f "probe.cfg" \
        -c "write ${elf}"
}

ssh_program() {
    _log info "programming with another Raspberry Pi via SSH"

    if [[ "${#}" -eq 0 ]]; then
        elf="$(_file_find ".elf")"
        # TODO function is continue on failure return
        [[ "${elf}" == "" ]] &&
            return 1
        echo "elf file is ${elf}"
    else
        elf="${1}"
    fi

    scp "${ROOT_DIR}/utils/ssh.cfg" raspberrypi.local:/tmp/

    scp "${elf}" raspberrypi.local:/tmp/
    ssh "raspberrypi.local" \
        killall openocd &> /dev/null || true
        ssh "raspberrypi.local" \
            openocd \
            -s "/tmp/" \
            -f "ssh.cfg" \
            -c "\"write /tmp/$(basename "${elf}")\""
}

ssh_openocd() {
    _log info "opening OpenOCD via SSH"
    _log info "ports are remapped to localhost, use as if it was running in this device"

    scp "${ROOT_DIR}/utils/ssh.cfg" raspberrypi.local:/tmp/

    ssh "raspberrypi.local" \
        killall openocd &>/dev/null || true
    # gdb: 3333
    # tcl: 6666
    # telnet: 4444
    ssh "raspberrypi.local" \
        -L 3333:localhost:3333 \
        -L 6666:localhost:6666 \
        -L 4444:localhost:4444 \
        openocd \
            -s "/tmp/" \
            -f "ssh.cfg"
        # monitor flash erase_sector 0 0 last
}

gdb() {
    # this also programs the OPNIC board
    _log info "starting gdb client"
    gdb="gdb-multiarch"
    if ! command -v "${gdb}" &>/dev/null; then
        gdb="arm-none-eabi-gdb"
    fi

    if [[ "${#}" -eq 0 ]]; then
        elf="$(_file_find ".elf")"
        # TODO function is continue on failure return
        [[ "${elf}" == "" ]] &&
            return 1
        echo "elf file is ${elf}"
    else
        elf="${1}"
    fi

    # _kill openocd
    # NOTE do not pass -s to the openocd here
    "${gdb}" \
        -iex "tar ext | openocd -f '${ROOT_DIR}/utils/probe.cfg' -c 'gdb_port pipe'" \
        -ex "load" \
        "${elf}"
    # _kill openocd
}

micropython() {
    _log info "program latest Micropython"
    file="$(mktemp /tmp/micropython-XXXX.uf2)"
    curl -o "${file}" "https://micropython.org/download/rp2-pico/rp2-pico-latest.uf2"
    uf2 "${file}"
}

picoprobe() {
    _log info "program latest picoprobe"
    file="$(mktemp /tmp/picoprobe-XXXX.uf2)"
    curl -o "${file}" "https://datasheets.raspberrypi.com/soft/picoprobe.uf2"
    uf2 "${file}"
}

_doxygen() {
    _log info "building code documentation"
    echo "${BUILD_DIR}"
    [[ ! -d "${BUILD_DIR}" ]] &&
        _log error "build the project first" &&
        return 1
    cmake --build "${BUILD_DIR}" --target "${PROJECT}_doc"
    # TODO remove "cpp" hard path
    _open "${BUILD_DIR}/firmware/cpp/${PROJECT}/doxygen/index.html"
}

install() {
    [[ ! -d "${BUILD_DIR}" ]] &&
        _log error nothing was built, aborting function &&
        return 1
    _log information installing generated files to "${ROOT_DIR}"
    cmake --install "${BUILD_DIR}" --prefix "${ROOT_DIR}"
}

_call_cmd() {
    # generic call command used inside _build.sh _main
    for i in "${@}"; do
        if [[ "$(command -v "${i}")" ]]; then
            _log info "calling" "\"${i}\""
            "${i}"
        else
            _log error "\"${i}\"" "is not a command" &&
            break
        fi
    done
}

doc() {
    _doxygen
}
