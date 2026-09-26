#pragma once

class Result {
private:
    double value;
    bool valid;

public:
    Result();
    explicit Result(double value);

    double getValue() const;
    bool isValid() const;

    void setValue(double value);
    void invalidate();
};