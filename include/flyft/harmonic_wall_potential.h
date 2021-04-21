#ifndef FLYFT_HARMONIC_WALL_POTENTIAL_H_
#define FLYFT_HARMONIC_WALL_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class HarmonicWallPotential : public ExternalPotential
    {
    public:
        HarmonicWallPotential() = delete;
        HarmonicWallPotential(double origin, double normal);

        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getSpringConstants();
        double getSpringConstant(const std::string& type) const;
        void setSpringConstants(const TypeMap<double>& spring_constants);
        void setSpringConstant(const std::string& type, double spring_constant);

        const TypeMap<double>& getShifts();
        double getShift(const std::string& type) const;
        void setShifts(const TypeMap<double>& shift);
        void setShift(const std::string& type, double shift);

        double getOrigin() const;
        void setOrigin(double origin);

        double getNormal() const;
        void setNormal(double normal);

        const TypeMap<double>& getDiameters();

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> spring_constants_;
        TypeMap<double> shifts_;
        double origin_;
        double normal_;
    };

}

#endif // FLYFT_HARMONIC_WALL_POTENTIAL_H_
