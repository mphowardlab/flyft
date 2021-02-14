#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/type_map.h"

#include <memory>
#include <vector>

namespace flyft
{

class State
    {
    public:
        State() = delete;
        State(std::shared_ptr<const Mesh> mesh, const std::string& type);
        State(std::shared_ptr<const Mesh> mesh, const std::vector<std::string>& types);

        // noncopyable / nonmovable
        State(const State&) = delete;
        State(State&&) = delete;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;

        std::shared_ptr<const Mesh> getMesh() const;

        int getNumFields() const;
        const std::vector<std::string>& getTypes();
        const std::string getType(int idx) const;
        int getTypeIndex(const std::string& type) const;

        const TypeMap<std::shared_ptr<Field>>& getFields();
        std::shared_ptr<Field> getField(const std::string& type);
        std::shared_ptr<const Field> getField(const std::string& type) const;

        const TypeMap<double>& getDiameters();
        double getDiameter(const std::string& type) const;
        void setDiameters(const TypeMap<double>& diameters);
        void setDiameter(const std::string& type, double diameter);

        const TypeMap<double>& getIdealVolumes();
        double getIdealVolume(const std::string& type) const;
        void setIdealVolumes(const TypeMap<double>& ideal_volumes);
        void setIdealVolume(const std::string& type, double ideal_volume);

        enum class Constraint
            {
            compute,
            N,
            mu
            };
        const TypeMap<double>& getConstraints();
        double getConstraint(const std::string& type);
        const TypeMap<Constraint>& getConstraintTypes();
        Constraint getConstraintType(const std::string& type);
        void setConstraint(const std::string& type, double value, Constraint ctype);
        void setConstraintType(const std::string& type, Constraint ctype);

    private:
        std::shared_ptr<const Mesh> mesh_;

        std::vector<std::string> types_;
        TypeMap<std::shared_ptr<Field>> fields_;
        TypeMap<double> diameters_;
        TypeMap<double> ideal_volumes_;
        TypeMap<double> constraints_;
        TypeMap<Constraint> constraint_types_;

        void checkConstraint(const std::string& type);
    };

}

#endif // FLYFT_STATE_H_
