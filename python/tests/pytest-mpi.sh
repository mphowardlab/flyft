#!/bin/bash

# This is a wrapper for mpirun to call 'python3 -m pytest [args]'. It does the following:
# * Only write a junit-xml file on rank 0
# * Redirect ranks > 0 output to pytest.out.rank

args=("$@")

# try to guess the rank from OpenMPI and MVAPICH env vars, falling back on python mpi4py if all fails
rank=${OMPI_COMM_WORLD_RANK:--1}
if (( rank == -1 ));
then
    rank=${MV2_COMM_WORLD_RANK:--1}
fi
if (( rank == -1 ));
then
find_mpi4py=$(cat <<EOF
try:
    from mpi4py import MPI
    rank = MPI.COMM_WORLD.Get_rank()
    print(rank)
except:
    print(-1)
EOF
)
    rank=$(python3 -c "$find_mpi4py")
fi

if (( rank == -1 ));
then
    echo "MPI rank could not be detected"
    exit 1
fi

if (( rank > 0 ));
then
    for ((i=0; i<"${#args[@]}"; ++i)); do
        case ${args[i]} in
            --junit-xml*) unset args[i]; break;;
        esac
    done
    python3 -m pytest "${args[@]}" > pytest.out.${rank} 2>&1
else
    python3 -m pytest "${args[@]}"
fi
