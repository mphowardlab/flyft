#ifndef FLYFT_TEMPLATED_EXTERNAL_POTENTIAL_H_
#define FLYFT_TEMPLATED_EXTERNAL_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/state.h"

#include <memory>
#include <string>

namespace flyft
{

template<class PotentialFunction>
class TemplatedExternalPotential : public ExternalPotential
    {
    public:
        using Function = PotentialFunction;

    protected:
        void computePotentials(std::shared_ptr<State> state) override
            {
            const auto mesh = *state->getMesh()->local();

            for (const auto& t : state->getTypes())
                {
                const auto potential = makePotentialFunction(state,t);
                auto d = derivatives_(t)->view();

                // compute on outside edge first and communicate
                const auto deriv_buffer = derivatives_(t)->buffer_shape();
                for (int idx=0; idx < deriv_buffer; ++idx)
                    {
                    d(idx) = potential(mesh.coordinate(idx));
                    }
                for (int idx=mesh.shape()-deriv_buffer; idx < mesh.shape(); ++idx)
                    {
                    d(idx) = potential(mesh.coordinate(idx));
                    }
                state->getMesh()->startSync(derivatives_(t));

                // compute on inside
                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(params,mesh) \
                shared(d,deriv_buffer,potential)
                #endif
                for (int idx=0; idx < mesh.shape()-deriv_buffer; ++idx)
                    {
                    d(idx) = potential(mesh.coordinate(idx));
                    }
                }

            // finalize communication of all buffers
            for (const auto& t : state->getTypes())
                {
                state->getMesh()->endSync(derivatives_(t));
                }
            }

        virtual Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) = 0;
    };

}

#endif // FLYFT_TEMPLATED_EXTERNAL_POTENTIAL_H_
