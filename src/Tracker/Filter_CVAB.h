#pragma once

#include "Filter.h"

#include <optional>
#include <array>

template <int dim, class T>
class Filter_CVAB: public Filter<dim, T> {
public:
    Filter_CVAB(const std::array<double, dim> alpha,
                const std::array<double, dim> beta) :
        alpha(std::move(alpha)),
        beta(std::move(beta))
    {}
    ~Filter_CVAB() override = default;

    Measurment<dim, T>* predict(TOA_t predictionTime) override
    {
        if (!position.has_value())
            return nullptr;

        for (size_t i = 0; i < dim; i++) {
            double dt = predictionTime - position->getToa();
            predicted.arr[i] = position->arr[i];
            if (velocity[i].has_value()) {
                predicted.arr[i] += *velocity[i] * dt;
            }
        }
        predicted.toa = predictionTime;
        return &predicted;
    }

    Measurment<dim, T>* getPosition() override
    {
        return &position.value();
    }

    void update(Measurment<dim, T>* meas) override
    {
        if (position.has_value()) {
            double dt = meas->getToa() - position->getToa();
            for (size_t i = 0; i < dim; i++) {
                if (velocity[i].has_value()) {
                    position->arr[i] += *velocity[i] * dt;
                    double error = meas->getData((int) i) - position->getData((int) i);
                    position->arr[i] += alpha[i] * error;
                    *velocity[i] += (beta[i] * error) / dt;
                } else {
                    velocity[i] = (meas->getData((int) i) - position->getData((int) i)) / dt;
                    position->arr[i] = meas->getData((int) i);
                }
            }
        }
        else {
            position = Measurment_Impl();
            for (size_t i = 0; i < dim; i++) {
                position->arr[i] = meas->getData((int) i);
            }
        }
        position->toa = meas->getToa();
        isUpdated_flag = true;
    }

    bool isUpdated() override {return isUpdated_flag;}
    void clearUpdated() override {isUpdated_flag = false;}
    bool isInNormalState() override {return position.has_value() && velocity[0].has_value();}

protected:
    struct Measurment_Impl : public Measurment<dim, T> {
        std::array<T, dim> arr;
        TOA_t toa;

        TOA_t& getToa() override { return toa; }
        T& getData(int dimension) override { return arr[dimension]; }
    };
    Measurment_Impl predicted;
    std::optional<Measurment_Impl> position;
    std::array<std::optional<double>, dim> velocity;
    const std::array<double, dim> alpha;
    const std::array<double, dim> beta;

    bool isUpdated_flag = false;

    friend void dummy();
};



