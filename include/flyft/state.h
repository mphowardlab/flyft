#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>
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

        double getTime() const;
        void setTime(double time);
        void advanceTime(double timestep);

    private:
        std::shared_ptr<const Mesh> mesh_;
        std::vector<std::string> types_;
        TypeMap<std::shared_ptr<Field>> fields_;
        double time_;
    };

}

#endif // FLYFT_STATE_H_
