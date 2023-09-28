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

@pytest.fixture
def binary_state_grand(spherical_mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(spherical_mesh_grand), ("A","B"))

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
        # Multiplying rho(x) into M at the end
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
            bd = -0.031831*(1 + 3*left_edge)
            M = 0.1*left_edge**2-0.005*left_edge**4
        else:
            bd = -0.031831*(1 + 3*left_edge)
            M =  (1/left_edge)*(-0.005+left_edge*(left_edge*(0.025+left_edge*(0.075))))
        flux[i] = (M+bd)
        print(left_edge,rpy.fluxes['A'][i],flux[i])
    assert np.allclose(rpy.fluxes['A'][:90], flux[:90],rtol = 2.5e-2, atol = 1e-2)

def test_binary_ideal(grand,ig,rpy,binary_state_grand):
    state = binary_state_grand

    volume = state.mesh.full.volume()
    ig.volumes['A'] = 1.0
    ig.volumes['B'] = 1.0
    grand.ideal = ig
    rpy.diameters['A'] = 1.0
    rpy.diameters['B'] = 3.0
    rpy.viscosity = 1.0

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    state.fields['B'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    grand.constrain('B', 0, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)
    assert np.allclose(rpy.fluxes['B'], 0.)

    # same thing with all ones
    state.fields['A'][:] = 1.
    state.fields['B'][:] = 1.
    grand.constrain('A', 0, grand.Constraint.N)
    grand.constrain('B', 0, grand.Constraint.N)
    rpy.compute(grand,state)
    assert np.allclose(rpy.fluxes['A'], 0.)
    assert np.allclose(rpy.fluxes['B'], 0.)
    
    x = state.mesh.local.centers
    state.fields['A'][:] = 3.0/state.mesh.full.L*(x[:])
    state.fields['B'][:] = 3.0/state.mesh.full.L*(x[:])
    grand.constrain('A', volume, grand.Constraint.N)
    grand.constrain('B', volume, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    for i in range(0,len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if i==0:
            M = 0
            bd = 0
        elif i>0 and i<10:
            bd = -0.0424413
            M = 0.0725*left_edge**2-0.007375*left_edge**4
        else:
            bd = -0.0424413
            M = -0.081/left_edge+0.12*left_edge
        flux[i] = (M+bd)
        print(left_edge,rpy.fluxes['A'][i],flux[i])
    assert np.allclose(rpy.fluxes['A'][:90], flux[:90],rtol = 2e-2, atol = 2e-2)
