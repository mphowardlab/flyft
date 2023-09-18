import numpy as np
import pytest

import flyft
from pytest_lazyfixture import lazy_fixture


@pytest.fixture
def spherical_mesh_grand():
    return flyft.state.SphericalMesh(10.,100, "repeat") 

@pytest.fixture
def state_grand(spherical_mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(spherical_mesh_grand), ("A", ))

def test_ideal(grand,ig,rpy,state_grand):
    state = state_grand

    volume = state.mesh.full.volume()
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    rpy.diameters['A'] = 1.0
    rpy.viscosity = 1.0

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)

    # same thing with all ones
    x = state.mesh.local.centers
    state.fields['A'][:] = 1
    grand.constrain('A', volume, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)
    
    x = state.mesh.local.centers
    state.fields['A'][:] = 3.0/state.mesh.full.L*(x[:])
    grand.constrain('A', state.mesh.full.L*1.5, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    for i in range(0,len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if i==0:
            M = 0
            bd = 0
        elif i>0 and i<10:
            bd = 1/(3*np.pi*1.*1.)*3.0/state.mesh.full.L
            M = left_edge*(-5+left_edge**2)/(18)*3.0/state.mesh.full.L
        else:
            bd = 1/(3*np.pi*1.*1.)*3.0/state.mesh.full.L
            M = -(1/18)*(5-1/(left_edge**2))*3.0/state.mesh.full.L
        flux[i] = -((3.0/state.mesh.full.L)*left_edge*M+bd)
        print(left_edge,rpy.fluxes['A'][i],flux[i])
    assert np.allclose(rpy.fluxes['A'][:90], flux[:90],atol = 5e-3)

   

def test_excess(grand,ig,rpy,state_grand):
    state = state_grand
    virial = flyft.functional.VirialExpansion()
    
    B = 5.0

    ig.volumes['A'] = 1.0
    virial.coefficients['A','A'] = B
    grand.ideal = ig
    grand.excess = virial
    
    volume = state.mesh.full.volume()
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    rpy.diameters['A'] = 1.0
    rpy.viscosity = 1

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)

    # same thing with all ones
    x = state.mesh.local.centers
    state.fields['A'][:] = 1
    grand.constrain('A', volume, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)
    
    x = state.mesh.local.centers
    state.fields['A'][:] = 3.0/state.mesh.full.L*(x[:])
    grand.constrain('A', state.mesh.full.L*1.5, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    for i in range(0,len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if i==0:
            M = 0
            bd = 0
        elif i>0 and i<10:
            bd = 1/(3*np.pi*1.*1.)*3.0/state.mesh.full.L
            M =  left_edge*(-5+left_edge**2)/(18)*3.0/state.mesh.full.L+left_edge*(1+2*np.pi*left_edge)*(left_edge**2-5)/60*3.0/state.mesh.full.L
        else:
            bd = 1/(3*np.pi*1.*1.)*3.0/state.mesh.full.L
            M = -(1/18)*(5-1/(left_edge**2))*3.0/state.mesh.full.L+(1+2*np.pi*left_edge)*(1-5*left_edge**2)/(60*left_edge**2)*3.0/state.mesh.full.L
        flux[i] = -((3.0/state.mesh.full.L)*left_edge*M+bd)
        print(left_edge,rpy.fluxes['A'][i],flux[i])
    assert np.allclose(rpy.fluxes['A'][:90], flux[:90],atol = 5e-3)
