#pragma once

#include "Types.h"

#include <QVector>

namespace Eigen {
    template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
}

template <int dim, class T>
class Assignment
{
public:
    Assignment() {}
    virtual ~Assignment() {}

    virtual QVector<Edge> assign(Eigen::MatrixXd& weights,
                                 QVector<Measurment<dim, T>*>& meases) = 0;
};

