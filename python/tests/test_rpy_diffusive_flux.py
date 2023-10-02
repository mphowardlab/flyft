import numpy as np
import pytest

import flyft
from pytest_lazyfixture import lazy_fixture


@pytest.fixture
def spherical_mesh_grand():
    return flyft.state.SphericalMesh(10.,1000, "repeat") 

@pytest.fixture
def state_grand(spherical_mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(spherical_mesh_grand), ("A", ))

@pytest.fixture
def binary_state_grand(spherical_mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(spherical_mesh_grand), ("A","B"))

def test_ideal(grand,ig,rpy,state_grand):
    state = state_grand
    ai = 0.5
    R = state.mesh.full.L
    
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
    state.fields['A'][:] = 3.0/R*(x[:])
    grand.constrain('A', R*1.5, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    """The chemical potential of the ideal gas equation of state given by:            
    \beta \mu = \ln\lambda \rho(y)  
    
    This term is differentiated w.r.t. y and multiplied by \rho(y) to 
    generate the force produced. This term is then multiplied by the
    projection of the mobility integrated over the surface of the
    droplet and integrated with respect to y to generate velocity.
    This velocity term is then multiplied with the density along the 
    radial direction x to generate the flux. 
    
    In this calculation density field is assumed to be:
    
    \rho(x) = 3/10*x
    
    Using this the above mentioned procedure, calculation can be done analytically 
    for a given density field. The calculation of the integration of the mobility 
    was done in Mathematica and the equations for the two different limits of integration 
    calculation are implemented below."""
    for i in range(len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if left_edge==0:
            M = 0
            bd = 0
        elif left_edge>0 and left_edge< ai+ai:            
            #Limits of integration (ai+ak)-x to x+(ai+ak), where ai = ak = 0.5
            bd = 1/(3*np.pi*1.*1.)*3.0/R
            M = left_edge*(-5+left_edge**2)/(18)*3.0/R
        else:
            #Limits of integration x-(ai+ak) to x+(ai+ak), where ai = ak = 0.5
            bd = 1/(3*np.pi*1.*1.)*3.0/state.mesh.full.L
            """Integrated form of the mobility is equal to \int{M}\d y for
             the limits of integration x-(2*ai) to x+(2*ai), where ai = 0.5"""
            M = -(1/18)*(5-1/(left_edge**2))*3.0/state.mesh.full.L
        # Multiplying rho(x) into M at the end
        flux[i] = -((3.0/state.mesh.full.L)*left_edge*M+bd)
        
    #Shifting bin centers to the lower bound of each of bin
    flags = x-0.5*R/state.mesh.full.shape < R-(ai+ai)
    assert np.allclose(rpy.fluxes['A'][flags], flux[flags],atol = 1e-3)

def test_excess(grand,ig,rpy,state_grand):
    state = state_grand
    virial = flyft.functional.VirialExpansion()
    ai = 0.5
    R = state.mesh.full.L
    
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
    state.fields['A'][:] = 3.0/R*(x[:])
    grand.constrain('A', R*1.5, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    for i in range(len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if left_edge==0:
            M = 0
            bd = 0
        elif left_edge>0 and left_edge<ai+ai:
        
            """The chemical potential of the virial equation of state given by:
            
            \beta \mu = \ln\lambda \rho(y) + 2*B*\rho(y). 
            
            This term is differentiated w.r.t. y and multiplied by \rho(y) to 
            generate the force produced. This term is then multiplied by the
            projection of the mobility integrated over the surface of the
            droplet and integrated with respect to y to generate velocity.
            This velocity term is then multiplied with the density along the 
            radial direction x to generate the flux. 
            
            In this calculation density field is assumed to be:
            
            \rho(x) = 3/10*x
            
            and virial coefficient is:
            
            B = 5
            
            Using this the above mentioned procedure, calculation can be done analytically,
            considering the different limits of the mobility tensor as seen in the ideal
            gas calculation. The calculation of the flux was done in Mathematica and the 
            equations for the two different limits of integration as mentioned in the 
            ideal gas calculation are implemented below.
            """
            
            bd = -0.031831*(1 + 3*left_edge)
            M = 0.1*left_edge**2-0.005*left_edge**4
        else:
            bd = -0.031831*(1 + 3*left_edge)
            M =  (1/left_edge)*(-0.005+left_edge*(left_edge*(0.025+left_edge*(0.075))))
        flux[i] = (M+bd)
        
    #Shifting bin centers to the lower bound of each of bin   
    flags = x-0.5*R/state.mesh.full.shape < R-(ai+ai)
    assert np.allclose(rpy.fluxes['A'][flags], flux[flags],atol = 1e-2)

def test_binary_ideal_one(grand,ig,rpy,binary_state_grand):

    """In this test we are checking if a "two component" system of same size 
    would generate the same result as the one-compenent system. Same procedure
    is followed as shown in the ideal gas equation of state calculation however
    in this case two different types are considered with same particle radius, 
    i.e. ai = ak = 0.5. The resulting fluxes calculated should match the 
    one-component ideal gas fluxes.The flux equations are computed 
    in Mathematica and implemented below"""
    
    state = binary_state_grand
    ai = 0.5 #Type A
    ak = 0.5 #Type B
    R = state.mesh.full.L
    
    volume = state.mesh.full.volume()
    ig.volumes['A'] = 1.0
    ig.volumes['B'] = 1.0
    grand.ideal = ig
    rpy.diameters['A'] = 1.0
    rpy.diameters['B'] = 1.0
    rpy.viscosity = 1.0

    x = state.mesh.local.centers
    state.fields['A'][:] = 3/R*(x[:])
    state.fields['B'][:] = 3/R*(x[:])
    grand.constrain('A', volume, grand.Constraint.N)
    grand.constrain('B', volume, grand.Constraint.N)
    rpy.compute(grand,state)
    
    flux = np.zeros(len(x))
    M1 = 0
    M2 = 0
    bd = 0

    for i in range(len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if left_edge==0:
            M1 = 0
            bd = 0
        elif left_edge>0 and left_edge<ai+ai:
            bd = -0.031831
            M1 = 0.025*left_edge**2-0.005*left_edge**4
        else:
            bd =  -0.031831
            M1 = -0.005/left_edge+0.025*left_edge
        
        if left_edge==0:
            M2 = 0
            bd = 0
        elif left_edge>0 and left_edge<ai+ak:
            bd =  -0.031831
            M2 = 0.025*left_edge**2-0.005*left_edge**4
        else:
            bd =  -0.031831
            M2 = -0.005/left_edge+0.025*left_edge
        flux[i] = (M1+M2+bd)
    #Shifting bin centers to the lower bound of each of bin
    flags = x-0.5*R/state.mesh.full.shape < R-(ai+ak)
    assert np.allclose(rpy.fluxes['A'][flags], flux[flags],atol = 1e-3)
       


def test_binary_ideal(grand,ig,rpy,binary_state_grand):
    state = binary_state_grand
    ai = 0.5 #Small particle radius (Type A)
    ak = 1.5 #Big particle radius (Type B)
    R = state.mesh.full.L
    
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
    state.fields['A'][:] = 3.0/R*(x[:])
    state.fields['B'][:] = 3.0/R*(x[:])
    grand.constrain('A', volume, grand.Constraint.N)
    grand.constrain('B', volume, grand.Constraint.N)
    rpy.compute(grand,state)
    flux = np.zeros(len(x))
    M1 = 0
    M2 = 0
    bd = 0
    
    """Same procedure followed as shown in the ideal gas equation of state 
    calculation however in this case two different radii of particles are 
    considered ,i.e. ai = 0.5 and ak = 1.5. The fluxes are computed in 
    Mathematica and implemented below"""
    
    for i in range(len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        """Due to the different types, bd diffusive flux would be implemented at 
        different mesh points. E.g. bd flux corresponding to type B, would not be 
        implemented until x = 3 is reached since type B has particle diameter of
        3 d_B """ 
        if left_edge==0:
            M1 = 0
            bd = 0
        elif left_edge > 0 and left_edge < ai+ai:
            bd = -0.031831
            M1 = 0.025*left_edge**2-0.005*left_edge**4
        else:
            bd = -0.031831
            M1 = -0.005/left_edge+0.025*left_edge
        
        if left_edge==0:
            M2 = 0
            bd = 0
        elif left_edge > 0 and left_edge < ai+ak:
            bd = -0.031831
            M2= 0.0475*left_edge**2-0.002375*left_edge**4
        else:
            bd = -0.031831
            M2 = -0.076/left_edge+0.095*left_edge
        flux[i] = (M1+M2+bd)
        
    #Shifting bin centers to the lower bound of each of bin
    flags = x-0.5*R/state.mesh.full.shape < R-(ai+ak)
    assert np.allclose(rpy.fluxes['A'][flags], flux[flags], atol = 1e-3)
