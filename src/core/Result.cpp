#include "ForgeSim/core/Result.hpp"

Result::Result()
    : value(0.0), valid(false) {
}

Result::Result(double value)
    : value(value), valid(true) {
}

double Result::getValue() const {
    return value;
}

bool Result::isValid() const {
    return valid;
}

void Result::setValue(double value) {
    this->value = value;
    this->valid = true;
}

void Result::invalidate() {
    valid = false;
}