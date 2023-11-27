/*******************************************************************************
 * @file Physics.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the physics math used by the game
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
#include "Physics.hpp"

#include <cmath>

Vector::Vector() {
  x_ = 1;
  y_ = 1;
  angle_ = GetAngle();
}

Vector::Vector(float x, float y) : x_(x), y_(y) { angle_ = GetAngle(); }

Vector::~Vector() {}

Vector Vector::operator=(const Vector& other) {
  if (this != &other) {
    x_ = other.x_;
    y_ = other.y_;
    angle_ = other.angle_;
  }
  return *this;
}

void Vector::SetLength(float length) {
  angle_ = GetAngle();

  x_ = length * std::cos(angle_);
  y_ = length * std::sin(angle_);
}

void Vector::SetAngle(float angle) {
  float length = GetLength();

  x_ = length * std::cos(angle);
  y_ = length * std::sin(angle);
}

float Vector::GetLength() { return std::sqrt(x_ * x_ + y_ * y_); }

float Vector::GetAngle() { return std::atan2(y_, x_); }

float Vector::GetX() { return x_; }

float Vector::GetY() { return y_; }

Vector& Vector::AddTo(Vector& vector) {
  x_ += vector.GetX();
  y_ += vector.GetY();

  return *this;
}

Vector& Vector::SubtractFrom(Vector& vector) {
  x_ -= vector.GetX();
  y_ -= vector.GetY();

  return *this;
}

Vector& Vector::Reverse() {
  x_ *= -1;
  y_ *= -1;
  angle_ = GetAngle();

  return *this;
}

Vector& Vector::ReverseX() {
  x_ *= -1;
  angle_ = GetAngle();

  return *this;
}

Vector& Vector::ReverseY() {
  y_ *= -1;
  angle_ = GetAngle();

  return *this;
}

Particle::Particle(float x, float y) : position_(x, y) {}

Particle::~Particle() {}

void Particle::SetMass(int mass) { mass_ = mass; }

int Particle::GetMass() { return mass_; }

Vector& Particle::GetPosition() { return position_; }

Vector& Particle::GetVelocity() { return velocity_; }

void Particle::Update() { position_.AddTo(velocity_); }

float Particle::DistanceTo(Particle& particle) {
  Vector& vec = particle.GetPosition();

  float dx = vec.GetX() - this->position_.GetX();
  float dy = vec.GetY() - this->position_.GetY();

  return std::sqrt(dx * dx + dy * dy);
}

void Particle::GravitateTo(Particle& particle) {
  Vector gravity(1, 0);
  float distance = DistanceTo(particle);

  float force = static_cast<float>(particle.GetMass() / (distance * distance));

  if (force > 3) force = 3;

  gravity.SetLength(force);
  gravity.SetAngle(AngleTo(particle));

  velocity_.AddTo(gravity);
}

float Particle::AngleTo(Particle& particle) {
  Vector& vec = particle.GetPosition();

  return std::atan2(vec.GetY() - position_.GetY(),
                    vec.GetX() - position_.GetX());
}
