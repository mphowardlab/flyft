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
    for (int idx = 0; idx < shape_.size(); ++idx)
        {
        step_.push_back((upper_bound[idx] - lower_bound[idx]) / shape_[idx]);
        }
    validateBoundaryCondition();
    }

Mesh::~Mesh() {}

std::shared_ptr<Mesh> Mesh::slice(const std::vector<int>& start, const std::vector<int>& end) const
    {
    if (lower_bc_ == BoundaryType::internal || upper_bc_ == BoundaryType::internal)
        {
        throw std::runtime_error("Cannot slice a Mesh more than once.");
        }

    auto m = clone();
    for (int idx = 0; idx < start.size(); ++idx)
        {
        m->start_[idx] = start[idx];
        m->shape_[idx] = end[idx] - start[idx];
        if (start[idx] > 0)
            {
            m->lower_bc_ = BoundaryType::internal;
            }
        if (end < shape_)
            {
            m->upper_bc_ = BoundaryType::internal;
            }
        }

    return m;
    }

std::vector<double> Mesh::center(const std::vector<int>& i) const
    {
    std::vector<double> temp;
    for (int idx; idx < shape_.size(); ++idx)
        {
        temp.push_back(lower_[idx] + static_cast<double>(start_[idx] + i[idx] + 0.5));
        }
    return temp;
    }

std::vector<int> Mesh::bin(const std::vector<double>& x) const
    {
    std::vector<int> temp;
    for (int idx; idx < shape_.size(); ++idx)
        {
        temp.push_back(static_cast<int>((x[idx] - lower_[idx]) / step_[idx]) - start_[idx]);
        }
    return temp;
    }

std::vector<double> Mesh::lower_bound() const
    {
    std::vector<int> temp(shape_.size(), 0);
    return lower_bound(temp);
    }

std::vector<double> Mesh::lower_bound(const std::vector<int>& i) const
    {
    std::vector<double> temp;
    for (int idx; idx < shape_.size(); ++idx)
        {
        temp.push_back(lower_[idx] + (start_[idx] + i[idx]) * step_[idx]);
        }
    return temp
    }

std::vector<double> Mesh::upper_bound() const
    {
    return lower_bound(shape_);
    }

std::vector<double> Mesh::upper_bound(const std::vector<int>& i) const
    {
    std::vector<double> temp;
    for (int idx; idx < shape_.size(); ++idx)
        {
        temp.push_back(i[idx] + 1);
        }
    return lower_bound(temp);
    }

std::vector<double> Mesh::L() const
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

std::vector<int> Mesh::asShape(const std::vector<double>& dx) const
    {
    std::vector<int> temp;
    for (int idx = 0; idx < shape_.size(); ++idx)
        {
        temp.push_back(static_cast<int>(std::ceil(dx[idx] / step_[idx])));
        }
    return temp;
    }

std::vector<double> Mesh::asLength(const std::vector<int>& shape) const
    {
    std::vector<int> temp;
    for (int idx = 0; idx < shape_.size(); ++idx)
        {
        temp.push_back(shape[idx] * step_[idx]);
        }
    return temp;
    }

double Mesh::integrateSurface(const std::vector<int>& idx, double j_lo, double j_hi) const
    {
    return area(idx) * j_lo - area(idx + 1) * j_hi;
    }

double Mesh::integrateSurface(const std::vector<int>& idx, const DataView<double>& j) const
    {
    return integrateSurface(idx, j(idx), j(idx + 1));
    }

double Mesh::integrateSurface(const std::vector<int>& idx, const DataView<const double>& j) const
    {
    return integrateSurface(const std::vector<int>& idx, j(idx), j(idx + 1));
    }

double Mesh::integrateVolume(std::vector<int> idx, double f) const
    {
    return volume(idx) * f;
    }

double Mesh::integrateVolume(const std::vector<int>& idx, const DataView<double>& f) const
    {
    return integrateVolume(const std::vector<int>& idx, f(idx));
    }

double Mesh::integrateVolume(const std::vector<int>& idx, const DataView<const double>& f) const
    {
    return integrateVolume(const std::vector<int>& idx, f(idx));
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
