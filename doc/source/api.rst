API
---

Free-energy models for hard-sphere fluids
-----------------------------------------

Free-energy functionals is an important component of the dynamic density functional functional
theory (DDFT) framework. Free-energy functional has three components: ideal-gas, excess, and external
potential contributions. ``flyft`` package provides several built-in free-energy
functionals for hard-sphere fluids to model the different excess and external contributions.

Excess free-energy functional
=============================

Depending on the required accuracy and density of the system, different
free-energy functionals can be employed.

.. autosummary::
    :nosignatures:
    :toctree: generated/

        flyft.functional.IdealGas
        flyft.functional.VirialExpansion
        flyft.functional.BoublikHardSphere
        flyft.functional.RosenfeldFMT

External free-energy functional
===============================

We also provide several built-in external potential models to represent
various confinement geometries and external fields for hard-sphere fluids.

.. autosummary::
    :nosignatures:
    :toctree: generated/

        flyft.external.LinearPotential
        flyft.external.HardWall
        flyft.external.HarmonicWall

Mobility tensor for hard-sphere fluids
--------------------------------------

In addition to free-energy functionals, flux models are also a key component of the DDFT
framework. Mobility tensor define how particles move in response to gradients in chemical potential
and external forces. The ``flyft`` package provides several built-in flux models for hard-sphere
fluids. These include:

.. autosummary::
    :nosignatures:
    :toctree: generated/

        flyft.dynamics.BrownianDiffusiveFlux
        flyft.dynamics.CompositeFlux
        flyft.dynamics.RPYDiffusiveFlux

Iterative solvers
-----------------

The ``flyft`` package also provides iterative solvers to find equilibrium density profiles
for hard-sphere fluids under various free-energy functionals and external potentials.

.. autosummary::
    :nosignatures:
    :toctree: generated/

        flyft.solver.PicardIteration
