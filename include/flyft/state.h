#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/field.h"
#include "flyft/mesh.h"

#include <memory>
#include <vector>

namespace flyft
{

class State
    {
    public:
        State() = delete;
        State(std::shared_ptr<const Mesh> mesh, int num_fields);
        State(std::shared_ptr<const Mesh> mesh, const std::vector<double>& Ns);

        // noncopyable / nonmovable
        State(const State&) = delete;
        State(State&&) = delete;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;

        std::shared_ptr<const Mesh> getMesh() const;

        int getNumFields() const;
        const std::vector<std::shared_ptr<Field>>& getFields();
        std::shared_ptr<Field> getField(int idx);
        std::shared_ptr<const Field> getField(int idx) const;

        const std::vector<double>& getDiameters();
        double getDiameter(int idx) const;
        void setDiameters(const std::vector<double>& diameters);
        void setDiameter(int idx, double diameter);

        const std::vector<double>& getIdealVolumes();
        double getIdealVolume(int idx) const;
        void setIdealVolumes(const std::vector<double>& ideal_volumes);
        void setIdealVolume(int idx, double ideal_volume);

        const std::vector<double>& getNs();
        double getN(int idx) const;
        void setNs(const std::vector<double>& Ns);
        void setN(int idx, double N);

    private:
        std::shared_ptr<const Mesh> mesh_;

        int num_fields_;
        std::vector<std::shared_ptr<Field>> fields_;
        std::vector<double> diameters_;
        std::vector<double> ideal_volumes_;
        std::vector<double> Ns_;
    };

}

#endif // FLYFT_STATE_H_
