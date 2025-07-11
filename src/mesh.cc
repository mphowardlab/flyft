#include "flyft/mesh.h"

#include <assert.h>
#include <cmath>

namespace flyft
    {
Mesh::Mesh(int num_orientation_dim,
           int* shape,
           double lower_bound,
           double upper_bound,
           BoundaryType lower_bc,
           BoundaryType upper_bc)
    : num_orientation_dim_(num_orientation_dim), num_dim_(num_position_dim_ + num_orientation_dim_),
      start_(0), lower_bc_(lower_bc), upper_bc_(upper_bc)
    {
    allocate();

    // copy position information
    assert(num_position_dim_ == 1);
    shape_[0] = shape[0];
    lower_[0] = lower_bound;
    step_[0] = (upper_bound - lower_bound) / shape[0];

    // copy angle bounds
    double angle_lower_bounds[3] {0, 0, 0};
    double angle_upper_bounds[3] {2 * M_PI, M_PI, 2 * M_PI};
    for (int dim = num_position_dim_; dim < num_dim_; ++dim)
        {
        shape_[dim] = shape_[dim];
        lower_[dim] = angle_lower_bounds[dim - num_position_dim_];
        step_[dim] = (angle_upper_bounds[dim - num_position_dim_] - lower_[dim]) / shape_[dim];
        }
    }

Mesh::Mesh(const Mesh& other)
    : num_orientation_dim_(other.num_orientation_dim_), num_dim_(other.num_dim_),
      start_(other.start_), lower_bc_(other.lower_bc_), upper_bc_(other.upper_bc_)
    {
    allocate();
    for (int dim = 0; dim < num_dim_; ++dim)
        {
        shape_[dim] = other.shape_[dim];
        lower_[dim] = other.lower_[dim];
        step_[dim] = other.step_[dim];
        }
    }

Mesh& Mesh::operator=(const Mesh& other)
    {
    if (this != &other)
        {
        num_orientation_dim_ = other.num_orientation_dim_;
        num_dim_ = other.num_dim_;
        lower_bc_ = other.lower_bc_;
        upper_bc_ = other.upper_bc_;
        start_ = other.start_;

        allocate();
        for (int dim = 0; dim < num_dim_; ++dim)
            {
            shape_[dim] = other.shape_[dim];
            lower_[dim] = other.lower_[dim];
            step_[dim] = other.step_[dim];
            }
        }
    return *this;
    }

Mesh::Mesh(const Mesh&& other)
    : num_orientation_dim_(std::move(other.num_orientation_dim_)),
      num_dim_(std::move(other.num_dim_)), shape_(std::move(other.shape_)),
      start_(std::move(other.start_)), lower_(std::move(other.lower_)),
      step_(std::move(other.step_)), lower_bc_(std::move(other.lower_bc_)),
      upper_bc_(std::move(other.upper_bc_))

    {
    }

Mesh& Mesh::operator=(const Mesh&& other)
    {
    num_orientation_dim_ = std::move(other.num_orientation_dim_);
    num_dim_ = std::move(other.num_dim_);
    shape_ = std::move(other.shape_);
    start_ = std::move(other.start_);
    lower_ = std::move(other.lower_);
    step_ = std::move(other.step_);
    lower_bc_ = std::move(other.lower_bc_);
    upper_bc_ = std::move(other.upper_bc_);
    return *this;
    }

Mesh::~Mesh()
    {
    delete[] shape_;
    delete[] lower_;
    delete[] step_;
    }

std::shared_ptr<Mesh> Mesh::slice(int start, int end) const
    {
    auto m = clone();

    assert(num_position_dim_ == 1);
    m->start_ = start;
    m->shape_[0] = end - start;
    if (start > 0)
        {
        m->lower_bc_ = BoundaryType::internal;
        }
    if (end < shape_[0])
        {
        m->upper_bc_ = BoundaryType::internal;
        }

    return m;
    }

void Mesh::cell_lower_bound(double* coordinate, const int* cell) const
    {
    cell_position_lower_bound(coordinate, cell);
    cell_orientation_lower_bound(coordinate + num_position_dim_, cell);
    }

double Mesh::cell_lower_bound(int dimension, int index) const
    {
    if (dimension < num_position_dim_)
        {
        assert(num_position_dim_ == 1);
        return lower_[0] + (start_ + index) * step_[0];
        }
    else
        {
        return lower_[dimension] + index * step_[dimension];
        }
    }

void Mesh::cell_position_lower_bound(double* coordinate, const int* cell) const
    {
    assert(num_position_dim_ == 1);
    coordinate[0] = cell_lower_bound(0, cell[0]);
    }

void Mesh::cell_orientation_lower_bound(double* coordinate, const int* cell) const
    {
    for (int orientation_dim = 0; orientation_dim < num_orientation_dim_; ++orientation_dim)
        {
        coordinate[orientation_dim]
            = cell_lower_bound(num_position_dim_ + orientation_dim, cell[orientation_dim]);
        }
    }

void Mesh::cell_center(double* coordinate, const int* cell) const
    {
    cell_position_center(coordinate, cell);
    cell_orientation_center(coordinate + num_position_dim_, cell);
    }

double Mesh::cell_center(int dimension, int index) const
    {
    if (dimension < num_position_dim_)
        {
        assert(num_position_dim_ == 1);
        return lower_[0] + (start_ + index + 0.5) * step_[0];
        }
    else
        {
        return lower_[dimension] + (index + 0.5) * step_[dimension];
        }
    }

void Mesh::cell_position_center(double* coordinate, const int* cell) const
    {
    assert(num_position_dim_ == 1);
    coordinate[0] = cell_center(0, cell[0]);
    }

void Mesh::cell_orientation_center(double* coordinate, const int* cell) const
    {
    for (int orientation_dim = 0; orientation_dim < num_orientation_dim_; ++orientation_dim)
        {
        coordinate[orientation_dim]
            = cell_center(num_position_dim_ + orientation_dim, cell[orientation_dim]);
        }
    }

void Mesh::cell_upper_bound(double* coordinate, const int* cell) const
    {
    cell_position_upper_bound(coordinate, cell);
    cell_orientation_upper_bound(coordinate + num_position_dim_, cell);
    }

double Mesh::cell_upper_bound(int dimension, int index) const
    {
    if (dimension < num_position_dim_)
        {
        assert(num_position_dim_ == 1);
        return lower_[0] + (start_ + index + 1) * step_[0];
        }
    else
        {
        return lower_[dimension] + (index + 1) * step_[dimension];
        }
    }

void Mesh::cell_position_upper_bound(double* coordinate, const int* cell) const
    {
    assert(num_position_dim_ == 1);
    coordinate[0] = cell_upper_bound(0, cell[0]);
    }

void Mesh::cell_orientation_upper_bound(double* coordinate, const int* cell) const
    {
    for (int orientation_dim = 0; orientation_dim < num_orientation_dim_; ++orientation_dim)
        {
        coordinate[orientation_dim]
            = cell_upper_bound(num_position_dim_ + orientation_dim, cell[orientation_dim]);
        }
    }

double Mesh::cell_volume(const int* cell) const
    {
    return cell_position_volume(cell) * cell_orientation_volume(cell);
    }

double Mesh::cell_orientation_volume(const int* cell) const
    {
    // start with integral being zero in case no orientation coordinates
    double V = 0;

    // alpha integral is step size
    if (num_orientation_dim_ >= 1)
        {
        V = step_[num_position_dim_ + 0];
        }

    // beta integral is -cos(beta)
    if (num_orientation_dim_ >= 2)
        {
        const int beta_dim = num_position_dim_ + 1;
        const double beta_0 = cell_lower_bound(beta_dim, cell[beta_dim]);
        const double beta_1 = cell_upper_bound(beta_dim, cell[beta_dim]);
        V *= (std::cos(beta_0) - std::cos(beta_1));
        }

    // gamma integral is step size
    if (num_orientation_dim_ == 3)
        {
        V *= step_[num_position_dim_ + 2];
        }

    return V;
    }

void Mesh::compute_cell(int* cell, const double* coordinate) const
    {
    for (int dim = 0; dim < num_dim_; ++dim)
        {
        cell[dim] = static_cast<int>((coordinate[dim] - lower_[dim]) / step_[dim]);
        }

    // shift position coordinate
    assert(num_position_dim_ == 1);
    cell[0] -= start_;
    }

void Mesh::length_as_shape(int* shape, const double* length) const
    {
    for (int dim = 0; dim < num_dim_; ++dim)
        {
        shape[dim] = static_cast<int>(std::ceil(length[dim] / step_[dim]));
        }
    }

void Mesh::shape_as_length(double* length, const int* shape) const
    {
    for (int dim = 0; dim < num_dim_; ++dim)
        {
        length[dim] = shape[dim] * step_[dim];
        }
    }

int Mesh::num_position_coordinates() const
    {
    return num_position_dim_;
    }

int Mesh::num_orientation_coordinates() const
    {
    return num_orientation_dim_;
    }

int Mesh::num_coordinates() const
    {
    return num_dim_;
    }

const int* Mesh::shape() const
    {
    return shape_;
    }

const double* Mesh::step() const
    {
    return step_;
    }

BoundaryType Mesh::lower_boundary_condition() const
    {
    return lower_bc_;
    }

BoundaryType Mesh::upper_boundary_condition() const
    {
    return upper_bc_;
    }

#if 0
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
#endif

bool Mesh::operator==(const Mesh& other) const
    {
    bool is_same = (typeid(*this) == typeid(other) && num_position_dim_ == other.num_position_dim_
                    && num_orientation_dim_ == other.num_orientation_dim_ && start_ == other.start_
                    && lower_bc_ == other.lower_bc_ && upper_bc_ == other.upper_bc_);
    for (int dim = 0; dim < num_dim_ && is_same; ++dim)
        {
        is_same |= (shape_[dim] == other.shape_[dim] && lower_[dim] == other.lower_[dim]);
        }
    return is_same;
    }

bool Mesh::operator!=(const Mesh& other) const
    {
    return !(*this == other);
    }

bool Mesh::validate_boundary_conditions() const
    {
    bool invalid
        = ((upper_bc_ == BoundaryType::periodic
            && !(lower_bc_ == BoundaryType::periodic || lower_bc_ == BoundaryType::internal))
           || (lower_bc_ == BoundaryType::periodic
               && !(upper_bc_ == BoundaryType::periodic || upper_bc_ == BoundaryType::internal)));
    return !invalid;
    }

void Mesh::allocate()
    {
    delete[] shape_;
    shape_ = new int[num_dim_];

    delete[] lower_;
    lower_ = new double[num_dim_];

    delete[] step_;
    step_ = new double[num_dim_];
    }

    } // namespace flyft
