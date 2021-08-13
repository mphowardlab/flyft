#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/communicator.h"
#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/parallel_mesh.h"
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
        State(double L, int shape, const std::string& type);
        State(double L, int shape, const std::vector<std::string>& types);
        State(double L, int shape, const std::string& type, std::shared_ptr<Communicator> comm);
        State(double L, int shape, const std::vector<std::string>& types, std::shared_ptr<Communicator> comm);
        State(const State& other);
        State(State&& other);
        State& operator=(const State& other);
        State& operator=(State&& other);

        std::shared_ptr<ParallelMesh> getMesh();
        std::shared_ptr<const ParallelMesh> getMesh() const;
        std::shared_ptr<Communicator> getCommunicator();
        std::shared_ptr<const Communicator> getCommunicator() const;

        int getNumFields() const;
        const std::vector<std::string>& getTypes();
        const std::string getType(int idx) const;
        int getTypeIndex(const std::string& type) const;

        const TypeMap<std::shared_ptr<Field>>& getFields();
        std::shared_ptr<Field> getField(const std::string& type);
        std::shared_ptr<const Field> getField(const std::string& type) const;
        void requestFieldBuffer(const std::string& type, int buffer_request);

        void syncFields();
        void syncFields(const TypeMap<std::shared_ptr<Field>>& fields) const;
        void matchFields(TypeMap<std::shared_ptr<Field>>& fields) const;
        void matchFields(TypeMap<std::shared_ptr<Field>>& fields, const TypeMap<int>& buffer_requests) const;

        double getTime() const;
        void setTime(double time);
        void advanceTime(double timestep);

    private:
        std::shared_ptr<ParallelMesh> mesh_;
        std::vector<std::string> types_;
        TypeMap<std::shared_ptr<Field>> fields_;
        double time_;
    };

}

#endif // FLYFT_STATE_H_
