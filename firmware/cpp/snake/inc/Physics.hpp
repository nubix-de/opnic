/*******************************************************************************
 * @file Physics.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Definition of the physics math used by the game
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights
 *reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#pragma once

/**
 * @brief Struct to represent a 2D position on screen alone
 */
typedef struct Position_t {
  float x;  ///< x-coordinate
  float y;  ///< y-coordinate
} Position_t;

/**
 * @class Vector
 * @brief Vector class for physics calculations on objects
 */
class Vector {
 private:
  float angle_;  ///< Stores the angle between x and y axis

 public:
  // We leave x_ and y_ to be directly modifiable if necessary
  float x_;  ///< x-coordinate
  float y_;  ///< y-coordinate

  /**
   * @brief Vector constructor
   *
   * @param x x-coordinate
   * @param y y-coordinate
   */
  Vector(float x, float y);

  /**
   * @brief Vector constructor
   */
  Vector();

  /**
   * @brief Vector destructor
   */
  ~Vector();

  /**
   * @brief Assignment operator
   */
  Vector operator=(const Vector& other);

  /**
   * @brief Set length of the vector (automatically recalculate angle)
   */
  void SetLength(float length);

  /**
   * @brief Set the angle of the vector (automatically recalculate x and y)
   */
  void SetAngle(float angle);

  /**
   * @brief Gets the length of the vector
   *
   * @return Returns the length
   */
  float GetLength();

  /**
   * @brief Gets the angle of the vector
   *
   * @return Returns the angle
   */
  float GetAngle();

  /**
   * @brief Get x-coordinate of the vector
   *
   * @return Returns the x-coordinate
   */
  float GetX();

  /**
   * @brief Get the y-coordinate of the vector
   *
   * @param Returns the y-coordinate
   */
  float GetY();

  /**
   * @brief Adds the passed vector to this one
   *
   * @return Returns the vector
   */
  Vector& AddTo(Vector& vector);

  /**
   * @brief Subtracts the passed vector from this one
   *
   * @return Returns the vector
   */
  Vector& SubtractFrom(Vector& vector);

  /**
   * @brief Reverse the vector axis
   *
   * @return Returns the vector
   */
  Vector& Reverse();

  /**
   * @brief Reverse the x-coordinate axis of the vector
   *
   * @return Returns the vector
   */
  Vector& ReverseX();

  /**
   * @brief Reverse the y-coordinate axis of the vector
   *
   * @return Returns the vector
   */
  Vector& ReverseY();
};

/**
 * @class Particle
 * @brief Class that represents the Particle object on the game
 */
class Particle {
 private:
  int mass_;         ///< Mass of the particle
  Vector position_;  ///< Vector that represents this particle's position
  Vector velocity_;  ///< Vector that represents this particle's velocity

 public:
  /**
   * @brief Class constructor
   *
   * @param x x-coordinate
   * @param y y-coordinate
   */
  Particle(float x, float y);

  /**
   * @brief Particle destructor
   */
  ~Particle();

  /**
   * @brief Sets the mass of the particle
   *
   * @param mass Mass to set
   */
  void SetMass(int mass);

  /**
   * @brief Gets the mass of the particle
   *
   * @return Returns the mass of the particle
   */
  int GetMass();

  /**
   * @brief Get the vector position of the particle
   *
   * @return Returns the vector position reference
   */
  Vector& GetPosition();

  /**
   * @brief Get the vector velocity of the particle
   *
   * @return Returns the vector velocity reference
   */
  Vector& GetVelocity();

  /**
   * @brief Do the update of the postion based on the velocity vector
   */
  void Update();

  /**
   * @brief Gets the particle distance related to the given particle
   *
   * @param particle Particle to calculate the distance
   *
   * @return Returns the distance between the two particles
   */
  float DistanceTo(Particle& particle);

  /**
   * @brief Gravitate the passed particle on this particle
   *
   * @param particle Particle that will suffer the gravity effect
   */
  void GravitateTo(Particle& particle);

  /**
   * @brief Returns the angle between this particle and the given particle
   *
   * @param particle Particle that will check against
   *
   * @return Returns the angle between the two particles
   */
  float AngleTo(Particle& particle);
};
