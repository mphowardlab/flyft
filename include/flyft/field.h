#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/mesh.h"

#include <memory>

namespace flyft
{

class Field
    {
    public:
        Field() = delete;
        Field(std::shared_ptr<Mesh> mesh);

        // noncopyable / nonmovable
        Field(const Field&) = delete;
        Field(Field&&) = delete;
        Field& operator=(const Field&) = delete;
        Field& operator=(Field&&) = delete;

        virtual ~Field();

        double* data();
        const double* data() const;

        std::shared_ptr<Mesh> getMesh();
        void setMesh(std::shared_ptr<Mesh> mesh);

    private:
        std::shared_ptr<Mesh> mesh_;
        double* data_;

        int alloc_shape_;
        void resize(int shape);
    };

}

#endif // FLYFT_FIELD_H_
