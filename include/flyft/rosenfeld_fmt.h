#ifndef FLYFT_ROSENFELD_FMT_H_
#define FLYFT_ROSENFELD_FMT_H_

#include "flyft/field.h"
#include "flyft/free_energy_functional.h"
#include "flyft/state.h"

#include <memory>
#include <vector>

namespace flyft
{

class RosenfeldFMT : public FreeEnergyFunctional
    {
    public:
        RosenfeldFMT() = delete;
        RosenfeldFMT(const std::vector<double>& diameters);

        void compute(std::shared_ptr<State> state) override;

        const std::vector<double>& getDiameters();
        double getDiameter(int idx) const;
        void setDiameters(const std::vector<double>& diameters);
        void setDiameter(int idx, double diameter);

    protected:
        std::vector<double> diameters_;

        bool check_diameters_;
        bool checkDiameters() const;

        void transformDensities(std::shared_ptr<State> state);
    };

}

#endif // FLYFT_ROSENFELD_FMT_H_
