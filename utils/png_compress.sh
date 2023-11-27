#!/usr/bin/env bash
# Compress image.
# Copyright nubix Software-Design GmbH
# Author Cassiano Silva de Campes
# Author Vinícius Gabriel Linden
# Date 2023-05-10
# Image must be a PNG.
# The output will be <name prefix>_s.png

set -eu
set -o pipefail

file="${1:-}"

[[ "${#}" -eq 0 ]] &&
    echo "Please provide a PNG file" &&
    exit 1

[[ ! -f "${file}" ]] &&
    echo "File does not exist" &&
    exit 1

[[ "$(file --mime-type "${file}" | awk '{print $2}')" != "image/png" ]] &&
    echo "File must be a png" &&
    exit 1

base="${file%.*}"
output="${base}_s.png"
temp="$(mktemp)"
rm -f -- "${temp}"

# Compress the PNG file
pngquant "${base}.png" --output "${temp}"

declare cmd
if command -v "pngnq" &>/dev/null; then
    cmd+="pngnq"
elif command -v "pngnq-s9" &>/dev/null; then
    cmd+="pngnq-s9"
else
    echo "pngnq command not found"
    exit 1
fi

# Reduce the PNG colors
"${cmd}" -n 32 "${temp}"

# Compress the PNG file
# pngcrush has a bug that forces us to call the tool twice
pngcrush "${temp}-nq8.png" "${output}" &>/dev/null
pngcrush "${temp}-nq8.png" "${output}" &>/dev/null

echo "Compressed image: ${output}"
