#include "flyft/mesh.h"

#include <cmath>

namespace flyft
    {

Mesh::Mesh(std::vector<double> lower_bound,
           std::vector<double> upper_bound,
           std::vector<int> shape,
           BoundaryType lower_bc,
           BoundaryType upper_bc)
    : lower_(lower_bound), shape_(shape), lower_bc_(lower_bc), upper_bc_(upper_bc), start_(0)
    {
    std::vector<double> step_;
    std::transform(upper_bound.begin(),
                   upper_bound.end(),
                   lower_bound.begin(),
                   step_.begin(),
                   [](int i, int j) { return i - j });
    std::transform(step_.begin(),
                   step_.end(),
                   shape_.begin(),
                   step_.begin(),
                   std::divides<double>());
    validateBoundaryCondition();
    }

std::shared_ptr<Mesh> Mesh::slice(std::vector<int> start, std::vector<int> end) const
    {
    if (lower_bc_ == BoundaryType::internal || upper_bc_ == BoundaryType::internal)
        {
        throw std::runtime_error("Cannot slice a Mesh more than once.");
        }

    auto m = clone();
    m->start_ = start;
    m->shape_ = end - start;
    if (start > 0)
        {
        m->lower_bc_ = BoundaryType::internal;
        }
    if (end < shape_)
        {
        m->upper_bc_ = BoundaryType::internal;
        }
    return m;
    }

std::vector<double> Mesh::center(std::vector<int> i) const
    {
    std::vector<double> temp;
    std::transform(i.begin(),
                   i.end(),
                   start_.begin(),
                   temp.begin(),
                   [](int i, int j) { return static_cast<double>(i + j + 0.5) * step_ });
    std::transform(temp.begin(), temp.end(), lower_.begin(), temp.begin(), std::plus<int>());
    return temp;
    }

std::vector<int> Mesh::bin(std::vector<double> x) const
    {
    std::vector<int> temp;
    std::transform(x.begin(),
                   x.end(),
                   lower_.begin(),
                   temp.begin(),
                   [](int i, int j) { return static_cast<int>((i - j) / step_) });
    std::transform(temp.begin(), temp.end(), start_.begin(), temp.begin().std::minus<int>);
    return temp;
    }

double Mesh::lower_bound() const
    {
    return lower_bound(0);
    }

std::vector<double> Mesh::lower_bound(std::vector<int> i) const
    {
    std::vector<double> temp;
    std::transform(i.begin(),
                   i.end(),
                   start_.begin(),
                   temp.begin(),
                   [](int i, int j) { return static_cast<double>((i + j) * step_) });
    std::transform(temp.begin(), temp.end(), start_.begin(), temp.begin().std::minus<double>);
    return lower_ + (start_ + i) * step_;
    }

double Mesh::upper_bound() const
    {
    return lower_bound(shape_);
    }

double Mesh::upper_bound(int i) const
    {
    return lower_bound(i + 1);
    }

double Mesh::L() const
    {
    return asLength(shape_);
    }

std::vector<int> Mesh::shape() const
    {
    return shape_;
    }

std::vector<double> Mesh::step() const
    {
    return step_;
    }

int Mesh::asShape(double dx) const
    {
    return static_cast<int>(std::ceil(dx / step_));
    }

double Mesh::asLength(int shape) const
    {
    return shape * step_;
    }

double Mesh::integrateSurface(std::vector<int> idx, double j_lo, double j_hi) const
    {
    return area(idx) * j_lo - area(idx + 1) * j_hi;
    }

double Mesh::integrateSurface(std::vector<int> idx, const DataView<double>& j) const
    {
    return integrateSurface(idx, j(idx), j(idx + 1));
    }

double Mesh::integrateSurface(std::vector<int> idx, const DataView<const double>& j) const
    {
    return integrateSurface(std::vector<int>, j(idx), j(idx + 1));
    }

double Mesh::integrateVolume(std::vector<int> idx, double f) const
    {
    return volume(idx) * f;
    }

double Mesh::integrateVolume(std::vector<int> idx, const DataView<double>& f) const
    {
    return integrateVolume(std::vector<int> idx, f(idx));
    }

double Mesh::integrateVolume(std::vector<int> idx, const DataView<const double>& f) const
    {
    return integrateVolume(std::vector<int> idx, f(idx));
    }

double Mesh::gradient(int idx, const DataView<double>& f) const
    {
    if ((idx == 0 && lower_bc_ == BoundaryType::reflect)
        || (idx == shape_ - 1 && upper_bc_ == BoundaryType::reflect))
        {
        return 0;
        }
    else
        {
        return gradient(idx, f(idx - 1), f(idx));
        }
    }

double Mesh::gradient(int idx, const DataView<const double>& f) const
    {
    if ((idx == 0 && lower_bc_ == BoundaryType::reflect)
        || (idx == shape_ - 1 && upper_bc_ == BoundaryType::reflect))
        {
        return 0;
        }
    else
        {
        return gradient(idx, f(idx - 1), f(idx));
        }
    }

bool Mesh::operator==(const Mesh& other) const
    {
    return (typeid(*this) == typeid(other) && lower_ == other.lower_ && shape_ == other.shape_
            && lower_bc_ == other.lower_bc_ && upper_bc_ == other.upper_bc_
            && start_ == other.start_);
    }

bool Mesh::operator!=(const Mesh& other) const
    {
    return !(*this == other);
    }

BoundaryType Mesh::lower_boundary_condition() const
    {
    return lower_bc_;
    }

BoundaryType Mesh::upper_boundary_condition() const
    {
    return upper_bc_;
    }

void Mesh::validateBoundaryCondition() const
    {
    if ((upper_bc_ == BoundaryType::periodic
         && !(lower_bc_ == BoundaryType::periodic || lower_bc_ == BoundaryType::internal))
        || (lower_bc_ == BoundaryType::periodic
            && !(upper_bc_ == BoundaryType::periodic || upper_bc_ == BoundaryType::internal)))
        {
        throw std::invalid_argument("Both boundaries must be periodic if one is.");
        }
    }
    } // namespace flyft
