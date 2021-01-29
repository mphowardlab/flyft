#include "flyft/rosenfeld_fmt.h"

#include <algorithm>

namespace flyft
{

RosenfeldFMT::RosenfeldFMT(const std::vector<double>& diameters)
    : check_diameters_(true)
    {
    setDiameters(diameters);
    }

void RosenfeldFMT::compute(std::shared_ptr<State> state)
    {
    if (state->getNumFields() != static_cast<int>(diameters_.size()))
        {
        // check diameters have right shape
        }

    if (!checkDiameters())
        {
        // error setting up
        }

    allocate(state);

    // compute weights n by summing over types
    for (int i=0; i < state->getNumFields(); ++i)
        {
        // fft the density

        // for each independent weight, multiply k-space density against analytical
        // transform of w and accumulate
        }
    // inverse fft to get in n in real space

    // evaluate partial derivatives of phi w.r.t. n at each site
    // phi,

    // compute functional derivatives by convolution

    // compute total value of functional by summing phi
    }

const std::vector<double>& RosenfeldFMT::getDiameters()
    {
    return diameters_;
    }

double RosenfeldFMT::getDiameter(int idx) const
    {
    return diameters_[idx];
    }

void RosenfeldFMT::setDiameters(const std::vector<double>& diameters)
    {
    diameters_.resize(diameters.size());
    std::copy(diameters.begin(), diameters.end(), diameters_.begin());
    check_diameters_ = true;
    }

void RosenfeldFMT::setDiameter(int idx, double diameter)
    {
    diameters_[idx] = diameter;
    check_diameters_ = true;
    }

bool RosenfeldFMT::checkDiameters() const
    {
    if (!check_diameters_) return true;

    // diameters should be positive
    for (const auto& d : diameters_)
        {
        if (d < 0)
            {
            return false;
            }
        }

    return true;
    }

}
