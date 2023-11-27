/*******************************************************************************
 * @file endian.hpp
 * @author Vinícius Gabriel Linden
 * @date 2023-11-10
 * @version v1.0
 * @brief Endian related functions.
 *
 * @copyright Copyright (c) 2023 nubix Software-Design GmbH, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include <cstdint>

//! @todo make these macros with VA_ARGS
//! @todo improve these macros

#define SWAP_ENDIAN_16(var) ((uint16_t)( \
                            ((var) << 8) | \
                            ((var) >> 08)))
#define SWAP_ENDIAN_32(var) ((uint32_t)( \
                            ((var) >> 24) | \
                            (((var) >> 8) & 0xFF00) | \
                            (((var) << 8) & 0xFF0000) | \
                            ((var) << 24)))
#define SWAP_ENDIAN_64(var) ((uint64_t)(\
                            ((var) >> 56) | \
                            (((var) >> 40) & 0xFF00) | \
                            (((var) >> 24) & 0xFF0000) | \
                            (((var) >> 8) & 0xFF000000) | \
                            (((var) << 8) & 0xFF00000000) | \
                            (((var) << 24) & 0xFF0000000000) | \
                            (((var) << 40) & 0xFF000000000000) | \
                            ((var) << 56)))

#endif /* ENDIAN_HPP */
