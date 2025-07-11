#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/communicator.h"
#include "flyft/field.h"
#include "flyft/mesh.h"
#include "flyft/parallel_mesh.h"
#include "flyft/tracked_object.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>
#include <vector>

namespace flyft
    {

//! Physical state
/*!
 * The State defines the density fields that can be operated on. It consists of a mesh that
 * discretizes space (and may be parallelized to multiple processors) and one density field on that
 * mesh for each type in its specified list. The state also has a time associated with it, which may
 * be used for advanced by dynamic calculations.
 */
class State : public TrackedObject
    {
    public:
    State() = delete;

    //! Constructor (single type).
    /*!
     * \param mesh Mesh defining discretized position and orientation coordinates.
     * \param type Single type for density field.
     */
    State(std::shared_ptr<ParallelMesh> mesh, const std::string& type);

    //! Constructor (multiple types).
    /*!
     * \param mesh Mesh defining discretized position and orientation coordinates.
     * \param type List of types for density fields.
     */
    State(std::shared_ptr<ParallelMesh> mesh, const std::vector<std::string>& types);

    //! Copy constructor.
    State(const State& other);

    //! Move constructor.
    State(State&& other);

    //! Copy assignment operator.
    State& operator=(const State& other);

    //! Move assignment operator.
    State& operator=(State&& other);

    //! Parallel mesh.
    std::shared_ptr<ParallelMesh> getMesh();

    //! Parallel mesh.
    std::shared_ptr<const ParallelMesh> getMesh() const;

    //! Communicator.
    std::shared_ptr<Communicator> getCommunicator();

    //! Communicator.
    std::shared_ptr<const Communicator> getCommunicator() const;

    //! Number of fields.
    int getNumFields() const;

    //! List of types.
    const std::vector<std::string>& getTypes();

    //! Get type name associated with type index.
    const std::string getType(int idx) const;

    //! Get type index associated with type name.
    int getTypeIndex(const std::string& type) const;

    //! Get fields for all types.
    const TypeMap<std::shared_ptr<Field>>& getFields();

    //! Get field for a single type.
    std::shared_ptr<Field> getField(const std::string& type);

    //! Get field for a single type.
    std::shared_ptr<const Field> getField(const std::string& type) const;

    //! Request a buffer of a particle shape for a field of a single type.
    void requestFieldBuffer(const std::string& type, const std::vector<int>& buffer_request);

    //! Gather all fields onto a rank.
    TypeMap<std::shared_ptr<Field>> gatherFields(int rank) const;

    //! Gather a field for a single type onto a rank.
    std::shared_ptr<Field> gatherField(const std::string& type, int rank) const;

    //! Synchronize all fields in the state.
    void syncFields();

    //! Start synchronizing all fields in the state.
    /*!
     * This is a nonblocking function, so work can be overlapped with the synchronization.
     * Call endSyncFields to finalize.
     */
    void startSyncFields();

    //! Finalize sychronization of all fields in the state.
    void endSyncFields();

    //! Make a set of fields with same types and buffers as the state.
    void matchFields(TypeMap<std::shared_ptr<Field>>& fields) const;

    //! Make a set of fields with same types and buffers as the state, with buffers requested.
    void matchFields(TypeMap<std::shared_ptr<Field>>& fields,
                     const TypeMap<std::vector<int>>& buffer_requests) const;

    //! Get the time.
    double getTime() const;

    //! Set the time.
    void setTime(double time);

    //! Advance the time.
    void advanceTime(double timestep);

    Token token() override;

    private:
    std::shared_ptr<ParallelMesh> mesh_;     //<! Discretized coordinates with parallelization.
    std::vector<std::string> types_;         //!< Types contained in the state.
    TypeMap<std::shared_ptr<Field>> fields_; //!< Density fields for the state.
    double time_;                            //!< Time.

    Dependencies depends_; //!< Dependencies on other objects.
    };

    } // namespace flyft

#endif // FLYFT_STATE_H_
