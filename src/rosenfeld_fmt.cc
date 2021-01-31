#include "flyft/rosenfeld_fmt.h"

#include <algorithm>
#include <cmath>

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

    // compute weights n0, n3, and nv1 in fourier space
    auto mesh = state->getMesh();
    FourierTransform ft(mesh->shape());
    const double domega = 2.*M_PI/(mesh->shape()*mesh->step());
    //////// these should be moved to allocate to avoid allocs
    std::vector<double> n0(ft.getRealSize());
    std::vector<double> n3(ft.getRealSize());
    std::vector<double> nv1(ft.getRealSize());
    std::vector<FourierTransform::Complex> n0k(ft.getReciprocalSize(),0.0);
    std::vector<FourierTransform::Complex> n3k(ft.getReciprocalSize(),0.0);
    std::vector<FourierTransform::Complex> nv1k(ft.getReciprocalSize(),0.0);
    ////////
    for (int i=0; i < state->getNumFields(); ++i)
        {
        // hard-sphere radius
        const double R = 0.5*diameters_[i];

        // fft the density
        ft.setRealData(state->getField(i).get());
        ft.transform();
        const auto rhok = ft.getReciprocalData();

        // accumulate the fourier transformed densities into n
        for (int j=0; j < ft.getReciprocalSize(); ++j)
            {
            const double omega = domega*j;
            const double wr = omega*R;
            const double sinwr = std::sin(wr);
            const double coswr = std::cos(wr);
            const double diff = (sinwr-wr*coswr)/(wr*wr);

            // n0
            const FourierTransform::Complex w0 = (omega > 0) ? sinwr/wr : 1.0;
            n0k[j] += w0*rhok[j];

            // n3
            const FourierTransform::Complex w3 = (omega > 0) ? 4.*M_PI*(R*R*R)*diff/wr : (4.*M_PI*(R*R*R)/3.);
            n3k[j] += w3*rhok[j];

            // nv1
            const FourierTransform::Complex wv1 = (omega > 0) ? (-R*diff)*I : 0.0;
            nv1k[j] += wv1*rhok[j];
            }
        }

    // transform n0, n3, nv1 to real space
    ft.setComplexData(n0k.data());
    ft.transform();
    n0.assign(ft.getRealData(),ft.getRealData()+ft.getRealSize());

    ft.setComplexData(n3k.data());
    ft.transform();
    n3.assign(ft.getRealData(),ft.getRealData()+ft.getRealSize());

    ft.setComplexData(nv1.data());
    ft.transform();
    nv1.assign(ft.getRealData(),ft.getRealData()+ft.getRealSize());

    // evaluate phi and partial derivatives in real space

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
