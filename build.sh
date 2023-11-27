#!/usr/bin/env bash
# Main build file
# Copyright nubix Software-Design GmbH
# Author Vinícius Gabriel Linden
# Date 2022-10-18

set -eu
set -o pipefail

declare -r SCRIPT="${BASH_SOURCE[0]##*/}"
DIR_CURRENT="$(readlink -f -- .)"
readonly DIR_CURRENT
ROOT_DIR="$(dirname -- "$(readlink -f -- "${BASH_SOURCE[0]}")")"
readonly ROOT_DIR
BUILD_DIR="${ROOT_DIR}/build"
readonly BUILD_DIR
declare -ra SEARCH=("cpp" "micropython")
readonly BOARD_FILE="${ROOT_DIR}/firmware/board/nubix_opnic.h"

PROJECT="OPNIC" # will set by the projects

source "${ROOT_DIR}/utils/utils.sh"

_usage() {
    cat << EOF
Perform action related to the projects.
Languages in use are: $(echo "${SEARCH[@]}" | sed 's/ /, /g')

SYNTAX
    ./${SCRIPT} [COMMANDS / PROJECT] [COMMANDS [COMMANDS [...]]]

ARGUMENTS
  micropython   Program the latest micropython uf2
  picoprobe     Program the latest picoprobe uf2

C(++) ARGUMENTS RELATED COMMANDS
  clean,clear   Clear the cached files
  purge         Clear all cached files
  debug         Build the project with debugging options
  release       Build the project with release options
  uf2           Find the uf2 file and program the OPNIC board with it
  swd           Program the OPNIC board via the picoprobe SWD programmer
                interface
  serial        Open up the serial interface on the terminal
  ssh_program   Program the OPNIC board with a "normal" Raspberry Pi via SSH
  doc           Build code documentation (doxygen)

MICROPYTHON OPTIONS
  -p=PATH       Set/add PATH to upload list
                PATH may be folder or a file
                If not used before, assumed the whole source folder
                automatically - may be used multiple times

MICROPYTHON SPECIFIC COMMANDS
  shell         Get an rshell prompt
                CTRL-D to exit
  remove        Remove loaded files inside the OPNIC board
  up            Upload files to the OPNIC board
  repl          Get a Micropython prompt
                CTRL-X to exit; CTRL-D to perform software reboot
  run           Import main ie. run current main program
  edit          Edit using rshell
                There is no advantage to this function

NOTES
- OPNIC board will stall (not respond) if you cancel this script in the middle
  of an micropython operation
- Micropython searches for \`boot.py\` and then \`main.py\` on initialization
- For uf2, OPNIC board has to be connected while BOOTSEL is pressed, see
  'README.md' for more information

EXAMPLE

  Clean previous build, compile for relase and program the snake project by
  using a coin
    ./${SCRIPT} clean snake release uf2

EOF
    exit "${1}"
}

help() {
    _usage 0
}

--help() {
    help
}

-h() {
    help
}

_main() {
    declare -a cmake_targets
    for subdir in "${ROOT_DIR}/firmware/"**"/"*; do
        [[ -f "${subdir}/CMakeLists.txt" ]] &&
            cmake_targets+=("$(basename "${subdir}")")
    done

    [[ "${#}" -eq 0 ]] &&
        _log info "nothing to be done" &&
        return 0

    while [[ "${#}" -ne 0 ]]; do

        argument="${1}"
        if [[ "${argument}" =~ ^_ ]]; then
            _log warn "argument not allowed, skipping"
        elif _element_exists "${argument}" "${cmake_targets[@]}"; then
            _log info "entering project ${argument}"
            PROJECT="${argument}"
        elif type "${argument}" &>/dev/null; then
            _log info "calling ${argument}"
            "${argument}"
        else
            _log error "unknown \"${argument}\" argument"
            return 1
        fi
        shift 1
    done
}

_main "${@}"
