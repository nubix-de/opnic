#!/usr/bin/env bash
# Build file for micropython projects
# Copyright nubix Software-Design GmbH
# Author Vinícius Gabriel Linden
# Date 2023-01-24

set -eu
set -o pipefail

LANGUAGE_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
readonly LANGUAGE_DIR
ROOT_DIR="$(git rev-parse --show-toplevel)"
readonly ROOT_DIR
PROJECT="gc9106"
declare -r SCRIPT="${BASH_SOURCE[0]##*/}"

_usage() {
    cat << EOF
Perform action related to micropython projects.

SYNTAX
  ./${SCRIPT} [PROJECT] [OPTIONS] [ARGUMENTS]

OPTIONS
  -p PATH       Set a search file or folder inside the project to PATH.

ARGUMENTS
  shell         Get an rshell prompt
                CTRL-D to exit
  remove        Remove loaded files inside the pico
  up            Upload files to pico
  repl          Get a Micropython prompt
                CTRL-X to exit; CTRL-D to perform software reboot
  run           Import main ie. run current main program
  edit          Edit using rshell using ${EDITOR}
                There is no advantage to this function

NOTE:
With the OPNIC board already having the micropython `.uf2` file and assuming you
have made the connections described below: connect the USB cable **without**
pressing the BOOT button.

EXAMPLES

Upload the default ${PROJECT} project and run it
  ./${SCRIPT} up run

Upload the touch project, open the REPL and then run it
  ./${SCRIPT} touch up repl
  CTRL+D for soft-reset ie. run main

EOF
    exit "${1}"
}

_paths_to_files() {
    # convert from PATHS files mixed with directories to FILES
    declare -a FILES
    for path in "${PATHS[@]}"; do
        _element_exists "${path}" "${FILES[@]}" && continue
        if [[ -d "${path}" ]]; then
            for file in "${path}"/*; do
                _element_exists "${file}" "${FILES[@]}" && continue
                FILES+=("$(readlink -f -- "${file}")")
            done
        else
            FILES+=("$(readlink -f -- "${path}")")
        fi
    done
    echo "${FILES[@]}"
}

_files_to_targets() {
    declare -a TARGETS
    for file in "${@}"; do
        # this assumes that the target file is not inside a folder
        TARGETS+=("/pyboard/$(basename "${file}")")
    done
    echo "${TARGETS[@]}"
}

_path_set() {
    # try as file
    path="${PROJECT}/${1%%.py}.py"
    # if fail, try as folder
    [[ ! -f "${path}" ]] &&
        path="${LANGUAGE_DIR}/${OPTARG}" &&
        [[ ! -d "${path}" ]] &&
            _log error "path" "${path}" "does not exist" &&
            exit 1
    # if failed, throw error
    path="$(readlink -f -- "${path}")" # rshell doesn't seem to work with links
    PATHS+=("${path}")
    _log info "added" "${path}" "to list"
}

_rshell() {
    # automatically get a remote shell
    if [[ ! "${RSHELL_PORT}" ]]; then
        # skip this part if already set
        device=($(ls /dev/serial/by-id/usb-MicroPython*)) ||
            _log error "Pico with Micropython not found, call \"micropython\" function first"

        # assuming that we will never connect more than two pico at the same time
        device="$(readlink -f -- "${device}")" # rshell doesn't work with long names
        [[ ! -c "${device}" ]] &&
            _log error "Unexpected error, aborting function" &&
            _log info "device: ${device}" &&
            return 2
        ! command -v rshell &>/dev/null &&
            pip3 install rshell

        export RSHELL_BUFFER_SIZE=512
        export RSHELL_PORT="${device}"
    fi

    rshell --quiet "${@}"
}

shell() {
    # get a shell
    _rshell
}

remove() {
    # erase all files
    if [[ "${PATHS[@]}" ]]; then
        FILES=($(_paths_to_files))
        TARGETS=($(_files_to_targets "${FILES[@]}"))
        _rshell rm -rf "${TARGETS[@]}"
    else
        _rshell rm -rf "/pyboard/"
    fi
    _log info "Remaining content(s):" "$(_rshell --nocolor ls "/pyboard/")" "$(echo)"
}

up() {
    # upload files only
    [[ "${PATHS[@]}" ]] || PATHS=("${PROJECT}")
    FILES=($(_paths_to_files))
    _rshell cp -r "${FILES[@]}" "/pyboard/"
}

repl() {
    # get a REPL
    _rshell repl "${@}"
}

edit() {
    # edit a file inside the pico and copy it back
    # this function is a little dumb, because rshell copies the target to a temp file, you edit it
    # on your computer and then copy it back over
    [[ ! "${PATHS}" ]] &&
        _log error "Please specify file(s) with the path option" &&
        return 1
    FILES=($(_paths_to_files))
    [[ "${#FILES[@]}" -gt 1 ]] &&
        _log warn "rshell edit only supports one file, editing" \
            "$(basename "${FILES[0]}")" "only" &&
        read -p "Enter or wait to continue" -n 1 -st 2 || true
    TARGETS=($(_files_to_targets "${FILES[@]}"))

    _rshell edit "${TARGETS[0]}"

    # we could just copy from tmp files, to know the path, get the terminal title
    # but as we know the link, we can just copy it back over
    tmpdir="/tmp/micropython/"
    mkdir -p "${tmpdir}"
    _rshell cp -r "${TARGETS[0]}" "${tmpdir}" &> /dev/null
    cat "${tmpdir}/$(basename "${TARGETS[0]}")" > "${FILES[0]}"
}

run() {
    # run program
    repl "~ from main import *~"
}

_main() {

    cd "${LANGUAGE_DIR}"

    [[ -d "${1}" ]] &&
        PROJECT="${1}" &&
        shift
    readonly PROJECT

    declare -a FILES # files to be taken into consideration
    declare -a PATHS # paths to be taken into consideration

    [[ "${BASH_SOURCE[0]}" == "${0}" ]] &&
        source "${ROOT_DIR}/utils/utils.sh"

    local OPTIND
    while getopts 'hp:' option; do
        case "${option}" in
            p)
                _path_set "${OPTARG}"
                ;;
            h)
                _usage 0
                ;;
            *)
                _log error "Invalid option: ${OPTARG}"
                _usage 1
                ;;
        esac
    done
    shift "$((OPTIND - 1))"

    _call_cmd "${@}"
}

_main "${@}"
