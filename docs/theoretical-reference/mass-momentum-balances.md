`Marlim3` transient solver uses an one-dimensional drift-flux model to describe three-phase gas-liquid-liquid flow in petroleum production systems. The model accounts for temperature variation along the pipe and heat exchange with the surroundings, as well as the coupling with a gas injection line, whose mass flow rate through the gas-lift valve is determined by the pressure difference between the service and production lines.

Before presenting the governing equations, it is useful to clarify the physical role of each balance in transient multiphase flow. In the class of problems targeted by `Marlim3`, not all information propagates through the system at the same speed. Pressure disturbances are associated with the fastest wave families and tend to adjust much more rapidly, whereas changes in void fraction and liquid distribution are predominantly linked to slower waves whose propagation speeds are of the order of the phase velocities. This separation of transport scales is one of the main reasons why `Marlim3` can treat holdup-related variables explicitly while reserving an implicit coupling for pressure and mixture mass flow rate. The derivation presente in this section should therefore be read as the basis for identifying which parts of the transient are fast, which are slow, and which simplifications are acceptable for the intended applications.

The key modeling hypotheses are:

- **No-slip between liquid phases**: the two liquid phases (produced liquid and complementary liquid) are assumed to travel at the same velocity;
- **Slip between gas and liquid mixture**: represented by a drift-flux model, where the distribution parameter and drift velocity are functions of flow conditions (pressure, temperature, superficial velocities) and flow pattern;
- **Four flow patterns** are considered: stratified, annular, dispersed bubble, and slug flow;
- **Reservoir inflow** mass source term for the produced liquid and gas conservation equations are based on IPR (Inflow Performance Relationship) or direct flow-defined liquid sources;
- **Complementary fluid** (e.g., diesel, ethanol, glycol) may be injected into the production system to prevent hydrate formation during shut-in periods, and is accounted for through a source term in its own mass conservation equation;
- **Gas-lift valve** is modeled either as a converging nozzle (orifice valve) or as a Venturi valve, with opening corrections applied when a pressure valve is indicated.

The one-dimensional mass conservation equations for each phase are given below.

## Complete set of equations

### Produced liquid balance

$$
A \frac{\partial \rho_{lp}(1-\alpha)(1-\beta)}{\partial t} + \frac{\partial \dot{M}_p}{\partial x} = \frac{\Gamma_{lp}}{\Delta L} - \psi
\label{eq:prod_liquid_balance1}
$$

### Complementary liquid balance

$$
A \frac{\partial \rho_{lc}(1-\alpha)\beta}{\partial t} + \frac{\partial \dot{M}_c}{\partial x} = \frac{\Gamma_{cp}}{\Delta L}
\label{eq:comp_liquid_balance1}
$$

### Gas balance:

$$
A \frac{\partial \rho_g \alpha}{\partial t} + \frac{\partial \dot{M}_g}{\partial x} = \frac{\Gamma_g}{\Delta L} + \psi
\label{eq:gas_balance1}
$$

Where:

- $\alpha$ is the void fraction of the fluid mixture;
- $\beta$ is the instantaneous volumetric fraction of the complementary liquid in the liquid mixture;
- $\psi$ is the mass transfer term between phases, positive if evaporation occurs, negative if condensation occurs;
- $\Gamma_{lp}$ is the mass source related to the produced liquid;
- $\Gamma_{cp}$ is the mass source related to the complementary liquid;
- $\Gamma_{g}$ is the mass source related to the gas phase;
- $\Delta L$ is the reference length associated with the mass sources;
- $\dot{M}_p$ is the mass flow rate of the produced liquid;
- $\dot{M}_c$ is the mass flow rate of the complementary liquid;
- $\dot{M}_g$ is the mass flow rate of the gas;
- $A$ is the pipe cross-sectional area;
- $t$ is the time;
- $x$ is the pipe length coordinate.


### Mixture momentum equation

$$
\frac{\partial \dot{M}_m}{\partial t} + \frac{\partial \left[ u_l(\dot{M}_p + \dot{M}_c) + u_g \dot{M}_g \right]}{\partial x} + A\frac{\partial p}{\partial x} = f_m \frac{\rho_m j^2}{2} S_w + \rho_m g A \sin(\theta)
\label{eq:mixture_momentum1}
$$

Where:

- $\dot{M}_m$ is the total mixture mass flow rate;
- $u_l$ is the liquid mixture local velocity;
- $u_g$ is the gas phase local velocity;
- $p$ is the pressure;
- $f_m$ is the fluid mixture Fanning friction factor;
- $\rho_m$ is the mixture density;
- $j$ is the mixture volumetric flux;
- $S_w$ is the wetted perimeter, defined by the tubing hydraulic diameter;
- $g$ is the gravitational acceleration;
- $\theta$ is the pipe inclination angle with respect to the horizontal.

### Drift-flux closure equation

The drift-flux model provides the **kinematic closure** that relates gas velocity to mixture velocity. Instead of solving separate momentum equations for each phase, the gas velocity is expressed as:

$$
u_g = C_0 \, j + u_d
\label{eq:drift_closure}
$$

where:

- $j = u_{sg} + u_{sl}$ is the mixture superficial velocity
- $C_0$ is the **distribution parameter** (accounts for non-uniform void fraction and velocity profiles across the pipe cross-section; $C_0 > 1$ means gas concentrates in the faster-flowing core)
- $u_d$ is the **drift velocity** (accounts for buoyancy-driven relative motion between phases).

### Mixture properties

For the calculation of mixture properties, the following relations are used:

**Liquid mixture density**

$$
\rho_l = (1-\beta)\rho_{lp} + \beta\rho_{lc}
\label{eq:rho_l}
$$

**Fluid mixture density**

$$
\rho_m = (1-\alpha)\rho_l + \alpha\rho_g
\label{eq:rho_m}
$$

**Liquid mixture dynamic viscosity**

$$
\mu_l = (1-\beta)\mu_{lp} + \beta\mu_{lc}
\label{eq:mu_l}
$$

**Fluid mixture dynamic viscosity**

$$
\mu_m = (1-\alpha)\mu_l + \alpha\mu_g
\label{eq:mu_m}
$$

Where:

- $\rho_{lp}$, $\rho_{lc}$, $\rho_g$ are the densities of the produced liquid, complementary liquid, and gas, respectively;
- $\mu_{lp}$, $\mu_{lc}$, $\mu_g$ are the dynamic viscosities of the produced liquid, complementary liquid, and gas, respectively.

### Flow rate equations

The numerical approach will be detailed further ahead; however, the equations presented in this section are reformulated to suit the intended numerical methodology. 

The problem is solved in a **semi-implicit** fashion: the void fraction $\alpha$ and the liquid volumetric fraction $\beta$ are updated through an **explicit scheme**, while pressure is resolved through an **implicit scheme** coupled with the mixture mass flow rate $\dot{M}_m$. Therefore, it is necessary to express the individual phase mass and volumetric flow rates in terms of the mixture mass flow rate, which is the primary variable of the problem. This is obtained from Equation \eqref{eq:drift_closure}:

$$
\dot{M}_l = \frac{1-\alpha C_0}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)}\dot{M}_m - \frac{\alpha A u_d \rho_g}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)} = T_1\dot{M}_m + T_2
\label{eq:M_l_from_M_m}
$$

$$
\dot{M}_g = \left[1 - \frac{1-\alpha C_0}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)}\right]\dot{M}_m + \frac{\alpha A u_d \rho_g}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)} = (1-T_1)\dot{M}_m - T_2
\label{eq:M_g_from_M_m}
$$

Where the auxiliary coefficients $T_1$ and $T_2$ are defined as:

$$
T_1 = \frac{1-\alpha C_0}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)}
\label{eq:T1}
$$

$$
T_2 = -\frac{\alpha A u_d \rho_g}{1-\alpha C_0\left(1-\frac{\rho_g}{\rho_l}\right)}
\label{eq:T2}
$$

The liquid volumetric flow rate and the individual liquid phase mass flow rates are then obtained as:

$$
Q_l = \frac{\dot{M}_L}{\rho_l}
\label{eq:Q_l}
$$

$$
\dot{M}_p = \frac{\rho_{lp}}{\rho_l}\dot{M}_L
\label{eq:M_p_from_M_l}
$$

$$
\dot{M}_c = \frac{\rho_{lc}}{\rho_l}\dot{M}_L
\label{eq:M_c_from_M_l}
$$

### Energy balance for the fluid mixture

The energy conservation equation for the fluid mixture is given by:

$$
\rho_l \alpha_l A \left[ \frac{\partial}{\partial t}\left(e_l+\frac{u_l^2}{2}\right) +u_l\frac{\partial}{\partial x}\left(h_l+\frac{u_l^2}{2}\right) \right]
+ \rho_g \alpha_g A \left[ \frac{\partial}{\partial t}\left(e_g+\frac{u_g^2}{2}\right) +u_g\frac{\partial}{\partial x}\left(h_g+\frac{u_g^2}{2}\right) \right]
+ p\left( \frac{1}{\rho_l}\frac{\partial (\rho_l u_l \alpha_l A)}{\partial x} + \frac{1}{\rho_g}\frac{\partial (\rho_g u_g \alpha_g A)}{\partial x} \right)
= -\left[ \left(e_g+\frac{u_g^2}{2}\right) - \left(e_l+\frac{u_l^2}{2}\right) \right]\psi A
- \left(e_g-h_{\mathrm{Source,g}}+\frac{u_g^2}{2}\right)\frac{\Gamma_g}{\Delta L}
- \left(e_{lp}-h_{\mathrm{Source,lp}}+\frac{u_l^2}{2}\right)\frac{\Gamma_{lp}}{\Delta L}
- \left(e_{lc}-h_{\mathrm{Source,lc}}+\frac{u_l^2}{2}\right)\frac{\Gamma_{lc}}{\Delta L}
- (\rho_l u_l \alpha_l)Ag - (\rho_g u_g \alpha_g)Ag + Q_w
\label{eq:energy_balance}
$$

Where

- $e_l$ is the internal energy of the liquid mixture;
- $e_g$ is the internal energy of the gas phase;
- $h_l$ is the enthalpy of the liquid mixture;
- $h_g$ is the enthalpy of the gas phase;
- $h_{lp}$ is the enthalpy of the produced liquid;
- $h_{lc}$ is the enthalpy of the complementary liquid;
- $h_{\mathrm{Source,g}}$ is the enthalpy associated with the gas source;
- $h_{\mathrm{Source,lp}}$ is the enthalpy associated with the produced liquid source;
- $h_{\mathrm{Source,lc}}$ is the enthalpy associated with the complementary liquid source;
- $Q_w$ is the heat flux.

### Interphase mass-transfer

The interphase mass-transfer term is adapted from the formulation presented in Oliveira (1995), now accounting for the instantaneous complementary liquid fraction $\beta$ and the in-situ water fraction $F_w$. The resulting expression estimates the mass-transfer rate between phases using fluid-property correlations typical of black-oil models:

$$\psi = -A\frac{\partial}{\partial t} \left((1-\alpha)(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o} \right) - \frac{\partial}{\partial x} \left( (1-\beta)(1-F_w)\frac{ R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l \right)
\label{eq:psi_blackoil}
$$

Where:

- $R_s$ is the gas solubility ratio in the oil;
- $B_o$ is the oil formation volume factor;
- $\gamma_g$ is the density of the gas dissolved in the liquid phase;
- $\rho_{air}^{std}$ is the air specific mass at standard conditions;
- $Q_l$ is the volumetric flow rate of the liquid mixture.

The in-situ water fraction in the produced liquid is obtained by:

$$F_w = \frac{F_w^{std} B_a}{B_o + F_w^{std} B_a - F_w^{std} B_o}
\label{eq:F_w}
$$

Where:

- $B_a$ is the water formation volume factor;
- $F_w^{std}$ is the water fraction in the produced liquid phase at standard conditions, which is generally equal to the BSW (Basic Sediment and Water) when sediments are not considered.

It is worth noting that in the derivation of this expression, the possibility of gas dissolution in the water phase was not taken into account.

## Simplifications of the Governing Equations for Implementation

The equations presented in the previous sections are not exactly those implemented in `Marlim3`. A set of simplifications has been introduced with the primary goal of improving computational efficiency, without compromising the ability to resolve the physical problems `Marlim3` was designed for, and while maintaining acceptable numerical robustness — a particularly critical requirement for multiphase flow simulators intended for petroleum industry applications.

The starting point for defining which simplifications are appropriate is to identify the physical phenomena involved in the problems `Marlim3` is expected to solve. These are predominantly **long-duration transient processes**, governed by kinematic waves or low-propagation-speed dynamic waves. In practical terms, this means that the most relevant transient information is usually carried by the slower families responsible for redistributing gas fraction, liquid holdup, and phase inventory along the pipe. Phenomena associated with pressure wave propagation, abrupt pressure changes, and rapid temperature variations are of secondary importance in this context. This is precisely why a **semi-implicit scheme** was chosen: it is inherently better suited to wave propagation speeds on the order of the flow velocity. If the intent were to capture sonic wave propagation, an explicit scheme would be more appropriate due to its simplicity and efficiency for such fast phenomena. That said, there may be situations in which the analysis of high-speed dynamic wave transients becomes relevant; in those cases, it would be preferable to **switch the numerical approach** to a more suitable method for the duration of the fast transient — most likely a high-resolution explicit Riemann solver.

With this in mind, the simplifications should target physical contributions that have **limited impact on slow wave propagation phenomena** — namely, the role of fluid compressibility in the temporal evolution of the solution, rather than its spatial variation. The compressibility of the **gas phase** is non-negotiable and must be retained, since even in slow transients there are gas accumulation processes with strong dependence on gas compressibility. However, for a multiphase problem, the compressibility of the **liquid phase** contributes very little even in these slow phenomena, and it is therefore the first candidate for simplification.

### Mass conservation equations

Considering the three mass conservation equations for each phase (Equations \eqref{eq:prod_liquid_balance1}, \eqref{eq:comp_liquid_balance1}, and \eqref{eq:gas_balance1}), and making the compressibility of the densities explicit in the temporal derivative terms:

**Produced liquid**

$$
\frac{\partial(1-\alpha)(1-\beta)}{\partial t} + \frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} = \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} - \frac{\psi}{A\rho_{lp}}
\label{eq:prod_liquid_balance_rearran}
$$

**Complementary liquid**

$$
\frac{\partial(1-\alpha)\beta}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} = \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L}
\label{eq:comp_liquid_balance_rearran}
$$

**Gas phase**

$$
\frac{\partial \alpha}{\partial t} + \frac{\alpha}{\rho_g}\left(\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \left.\frac{\partial \rho_g}{\partial T}\right|_p \frac{\partial T}{\partial t}\right) + \frac{1}{A\rho_g}\frac{\partial \dot{M}_g}{\partial x} = \frac{\Gamma_g}{A\rho_g \Delta L} + \frac{\psi}{A\rho_g}
\label{eq:gas_balance_rearran}
$$

For Equations \eqref{eq:prod_liquid_balance_rearran} and \eqref{eq:comp_liquid_balance_rearran}, the liquid density derivative terms are moved to the right-hand side and treated as source terms within the convergence iteration of the numerical scheme, to be included whenever deemed necessary:

**Produced liquid**:

$$
\frac{\partial(1-\alpha)(1-\beta)}{\partial t} + \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} = \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} - \frac{\psi}{A\rho_{lp}} - \left(\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t}\right)
\label{eq:prod_liquid_balance_rearran2}
$$

**Complementary liquid**:

$$
\frac{\partial(1-\alpha)\beta}{\partial t} + \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} = \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} - \left(\frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right)
\label{eq:comp_liquid_balance_rearran2}
$$

Adding equations (Equations \eqref{eq:prod_liquid_balance_rearran2} and \eqref{eq:comp_liquid_balance_rearran2}) yields a useful **liquid holdup evolution equation** for the production line.

$$
\frac{\partial(1-\alpha)}{\partial t} + \frac{\psi}{A\rho_{lp}} = -\frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} - \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} - \left(\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right)
\label{eq:holdup_evolution}
$$

For Equation \eqref{eq:holdup_evolution} to be treated as a direct holdup evolution expression, the parenthesized liquid compressibility terms must either be **discarded** — yielding a fully explicit, single-pass update — or Equation \eqref{eq:holdup_evolution} must be solved **iteratively**, re-evaluating those terms at each iteration. The second option, while more accurate, carries a computational performance penalty. This equation is always the first to be integrated, yielding the holdup profile prior to advancing the remaining equations. This is particularly advantageous, as it allows one to identify, in advance, the locations along the domain where a transition from multiphase to single-phase flow may occur (which ultimately constitutes a singularity in the model), thereby enabling the necessary corrections and adjustments to be made before proceeding with the solution of the problem.

The evolution equation for $\beta$ can be derived directly from Equation \eqref{eq:comp_liquid_balance_rearran2}. In the numerical scheme it is solved immediately after Equation \eqref{eq:holdup_evolution}, so the updated holdup profile is already available. The resolution is also explicit:

$$
\frac{\partial \beta}{\partial t} + \frac{\beta}{(1-\alpha)}\frac{\partial(1-\alpha)}{\partial t} + \frac{1}{A(1-\alpha)\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} = \frac{\Gamma_{cp}}{A(1-\alpha)\rho_{lc}\Delta L} - \left(\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right)
\label{eq:beta_evolution_1}
$$

Rearranging:

$$
\frac{\partial \beta}{\partial t} = \frac{\Gamma_{cp}}{A(1-\alpha)\rho_{lc}\Delta L} - \frac{\beta}{(1-\alpha)}\frac{\partial(1-\alpha)}{\partial t} - \frac{1}{A(1-\alpha)\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} - \left(\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right)
\label{eq:beta_evolution_2}
$$

Adding the gas mass conservation equation Equation \eqref{eq:gas_balance_rearran} with the holdup evolution equation Equation \eqref{eq:holdup_evolution} yields:

$$
\frac{\alpha}{\rho_g}\left(\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \left.\frac{\partial \rho_g}{\partial T}\right|_p \frac{\partial T}{\partial t}\right) + \frac{1}{A\rho_g}\frac{\partial \dot{M}_g}{\partial x} + \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} + \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} = \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} + \frac{\psi}{A\rho_g} - \frac{\psi}{A\rho_{lp}} - \left(\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right)
\label{eq:summing_holdupEv_gasCons}
$$

As discussed, gas compressibility cannot be neglected — slow gas accumulation phenomena depend critically on the pressure derivative of gas density — however, the influence of the temperature derivative of $\rho_g$ on the temporal evolution is small. It is therefore moved to the "deferred" terms on the right-hand side of Equation \eqref{eq:summing_holdupEv_gasCons}, to be either discarded or updated iteratively, yielding:

$$
\frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \frac{1}{A\rho_g}\frac{\partial \dot{M}_g}{\partial x} + \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} + \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} + \frac{1}{A}\left(\frac{1}{\rho_{lp}} - \frac{1}{\rho_g}\right)\psi = \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} - \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial T}\right|_p \frac{\partial T}{\partial t}\right]
\label{eq:final_mass_conservation}
$$

The left-hand side now retains only the **pressure-compressibility** term for the gas phase — the dominant compressibility contribution — along with the spatial divergence of all three phase mass flow rates and the interphase mass-transfer term $\psi$. The bracketed terms on the right-hand side group all deferred compressibility corrections (liquid phases and gas thermal expansion), which may be dropped for a leaner explicit update or reintroduced iteratively when higher accuracy is required.

### Momentum equation

In the momentum equation, the dynamic (acceleration) terms are expected to be of minor relevance for the class of phenomena `Marlim3` is designed to simulate. Removing all acceleration terms from Equation \eqref{eq:mixture_momentum1} would render the model very similar to the well-known NPW scheme. Therefore, we will retain the $\partial \dot{M}_g / \partial t$ term in Equation \eqref{eq:mixture_momentum1}, yielding:

$$
\frac{\partial \dot{M}_g}{\partial t} + A_t \frac{\partial p}{\partial x} = f_m \frac{\rho_m j^2}{2} S_w + \rho_m g A_t \sin(\theta)
\label{eq:simplified_momentum}
$$

The mixture momentum equation, together with Equation \eqref{eq:final_mass_conservation} (which is essentially the mixture mass conservation equation), forms the basis for the **pressure–mass flow rate coupling** procedure. By retaining the $\partial \dot{M}_g / \partial t$ term, `Marlim3` harmonizes the pressure–velocity coupling between Equations \eqref{eq:final_mass_conservation} and \eqref{eq:simplified_momentum}, which is sufficient to stabilize the semi-implicit scheme without reintroducing the full set of acceleration terms.

From the perspective of the problem at hand, the equation pair Equations \eqref{eq:final_mass_conservation}–\eqref{eq:simplified_momentum} is directly tied to the resolution of the pressure profile. Since pressure is closely linked to the fastest wave families, both equations must be solved in a **fully implicit** manner to avoid CFL-based time-step restrictions. This means the pressure and mixture mass flow rate profiles are solved **simultaneously**, leading to a matrix system — a simple banded matrix that is straightforward to assemble and solve.

This approach yields a numerical framework that is simple to implement, robust, and — provided the deferred terms in Equations \eqref{eq:holdup_evolution}, \eqref{eq:beta_evolution_2} and \eqref{eq:final_mass_conservation} are discarded — free of iterative loops, with good computational performance.

### Interphase mass-transfer

The interphase mass-transfer term $\psi$ was not yet made explicit in Equations \eqref{eq:holdup_evolution} and \eqref{eq:final_mass_conservation}. Before doing so, some adjustments to Equation \eqref{eq:psi_blackoil} are required. The most immediate one concerns the temporal variation of pressure-dependent quantities. The mass-transfer equation is derived from a mass balance of the light component dissolved in the dead oil — i.e., the light component in the liquid state — which accounts for most of the pressure dependence of the liquid phase density. Consistently with the treatment applied to the produced liquid mass conservation equation, the pressure-dependent temporal derivative will be separated as a deferred term that may or may not be discarded. Expanding the temporal derivative of Equation \eqref{eq:psi_blackoil}:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\frac{\partial(1-\alpha)(1-\beta)}{\partial t} - A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t} - \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} \label{eq:mass_transfer_expanded}$$

Which, separating the pressure-sensitive temporal term into the deferred bracket, gives:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\frac{\partial(1-\alpha)(1-\beta)}{\partial t} - \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} - \left[A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t} \right]
\label{eq:psi_expanded}$$

The bracketed term may or may not be discarded depending on whether an iterative time-advancement procedure is adopted.

A brief clarification on the numerical treatment: $\psi$ is handled differently in Equations \eqref{eq:holdup_evolution} and \eqref{eq:final_mass_conservation}. In Equation \eqref{eq:holdup_evolution}, which governs holdup evolution, the mass source and mass flow rate terms are treated **explicitly**. In Equation \eqref{eq:final_mass_conservation}, used in the coupled pressure–mass flow rate system, the mass flow rate terms are treated **implicitly** — noting that the holdup temporal derivative is already available at that stage. While this may appear numerically inconsistent when no iterative procedure is used, this methodology has proven robust and effective for the time-step ranges typical of a semi-implicit scheme.

### Final holdup evolution equation

In preparing Equation \eqref{eq:psi_expanded} for substitution into Equation \eqref{eq:holdup_evolution}, the variable $\beta$ must be factored out of the temporal derivative; otherwise, the holdup and $\beta$ evolution equations would need to be solved in a coupled fashion, which is impractical. The algebraic steps are as follows.

Applying the chain rule to $\dfrac{\partial(1-\alpha)(1-\beta)}{\partial t}$:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\beta)\frac{\partial(1-\alpha)}{\partial t} + A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\partial \beta}{\partial t} - \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x}-\left[A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t}\right]$$

The term $(1-\alpha)\dfrac{\partial \beta}{\partial t}$ is then replaced using Equation \eqref{eq:beta_evolution_2}:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\beta)\frac{\partial(1-\alpha)}{\partial t} + A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\left[\frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} - \beta\frac{\partial(1-\alpha)}{\partial t} - \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x}\right] - \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} - \left[A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t} + A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right]$$

Which leads to:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\beta)\frac{\partial(1-\alpha)}{\partial t} + (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\frac{\Gamma_{cp}}{\rho_{lc}\Delta L} - A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\beta\frac{\partial(1-\alpha)}{\partial t} - (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}\frac{1}{\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} - \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} - \left[A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t} + A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right]$$

Grouping the terms containing temporal derivatives of the holdup:

$$\psi = -A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\beta)\frac{\partial(1-\alpha)}{\partial t} - (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{A\rho_{lp}B_o}\frac{1}{\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x}- \frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} + (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{A\rho_{lp}B_o}\frac{\Gamma_{cp}}{\rho_{lc}\Delta L}- \left[A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t} + A(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t}\right]
\label{eq:psi_grouped}$$

Note that after grouping, the two holdup temporal derivative terms — originally separated by the chain-rule expansion — recombine into a single $(1-\beta)\,\partial(1-\alpha)/\partial t$ contribution, while the $\beta\,\partial(1-\alpha)/\partial t$ terms cancel. The deferred bracket retains the black-oil property and liquid compressibility corrections, consistent with the treatment adopted throughout.

Substituting Equation \eqref{eq:psi_grouped} into Equation \eqref{eq:holdup_evolution} yields:

$$\frac{\partial(1-\alpha)}{\partial t} - (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{\rho_{lp}B_o}(1-\beta)\frac{\partial(1-\alpha)}{\partial t} - (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{A\rho_{lp}B_o}\frac{1}{\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x}- \frac{1}{A\rho_{lp}}\frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} + (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{A\rho_{lp}B_o}\frac{\Gamma_{cp}}{\rho_{lc}\Delta L} =- \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} - \frac{1}{A\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L}- \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} - \frac{A(1-F_w)}{ A\rho_{lp}}\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} - \frac{A(1-\alpha)(1-\beta)}{A\rho_{lp}}\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t}\right]
\label{eq:holdup_evolution_semifinal}$$

Rearranging Equation \eqref{eq:holdup_evolution_semifinal}:

$$\left[1 - (1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{\rho_{lp}B_o}(1-\beta)\right]\frac{\partial(1-\alpha)}{\partial t} =- \frac{1}{A\rho_{lp}}\frac{\partial \dot{M}_p}{\partial x} - \frac{1}{A\rho_{lc}}\left[1-(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{\rho_{lp}B_o}\right]\frac{\partial \dot{M}_c}{\partial x}+ \frac{1}{A\rho_{lp}}\frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}Q_l}{\partial x} + \left[1-(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{\rho_{lp}B_o}\right]\frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L}- \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} - \frac{A(1-F_w)}{ A\rho_{lp}}\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}(1-\alpha)\frac{\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} - \frac{A(1-\alpha)(1-\beta)}{A\rho_{lp}}\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{air}^{std}}{B_o}}{\partial t}\right] \label{eq:holdup_evolution_final}$$

This rearrangement makes the structure of the **explicit holdup update** particularly clear: the left-hand side contains a single effective coefficient multiplying $\partial(1-\alpha)/\partial t$, which accounts for the dissolved-gas feedback on holdup dynamics through the black-oil parameters $R_s$, $B_o$ and $F_w$. The right-hand side groups the flux divergences, mass sources, and deferred correction terms in a form directly amenable to explicit time integration. Therefore, Equation \eqref{eq:holdup_evolution_final} is the holdup evolution equation used in `Marlim3` transient model.

### Final mixture mass flow rate equation

As previously stated, for Equation \eqref{eq:final_mass_conservation} the mass-transfer term is treated differently, since the holdup and $\beta$ profiles are assumed to have already been obtained for the current time level. Applying the mass-transfer equation \eqref{eq:mass_transfer_expanded} into \eqref{eq:final_mass_conservation} yields:

$$\frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \frac{1}{A\rho_g}\frac{\partial \dot{M}g}{\partial x} + \frac{1}{A\rho{lp}}\frac{\partial \dot{M}p}{\partial x} + \frac{1}{A\rho{lc}}\frac{\partial \dot{M}_c}{\partial x}
+ \frac{1}{A}\left(\frac{1}{\rho_{lp}} - \frac{1}{\rho_g}\right)
\left\{
-A(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}
\frac{\partial(1-\alpha)(1-\beta)}{\partial t}
-
\frac{\partial\left((1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}Q_l\right)}{\partial x}
-
\left[
A(1-\alpha)(1-\beta)
\frac{\partial\left((1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\right)}{\partial t}
\right]
\right\}
= \frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} - \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial T}\right|_p \frac{\partial T}{\partial t}\right]$$

Rearranging:

$$\frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \frac{1}{A\rho_g}\frac{\partial \dot{M}g}{\partial x} + \frac{1}{A\rho{lp}}\frac{\partial \dot{M}p}{\partial x} + \frac{1}{A\rho{lc}}\frac{\partial \dot{M}_c}{\partial x}+ \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)\frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}Q_l}{\partial x} =\frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} - \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)A(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\frac{\partial(1-\alpha)(1-\beta)}{\partial t}- \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial T}\right|p \frac{\partial T}{\partial t} + \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho{lp}}\right)A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}}{\partial t}\right] \label{eq:final_mass_conservation_rearran}$$

Note that the temporal derivatives of the liquid volumetric fractions remain on the right-hand side of \eqref{eq:final_mass_conservation_rearran}, as they are treated as source terms in the numerical approach.

Furthermore, in \eqref{eq:final_mass_conservation_rearran} it is convenient to express all flow rate derivatives in terms of the **gas mass flow rate** $\dot{M}_g$ and the **liquid mixture mass flow rate** $\dot{M}_l$, since equation \eqref{eq:final_mass_conservation_rearran} will be used in the numerical scheme to obtain $\dot{M}_m$, from which $\dot{M}_g$ and $\dot{M}_l$ are recovered via equations \eqref{eq:M_l_from_M_m} and \eqref{eq:M_g_from_M_m}. Therefore:

$$\frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \frac{1}{A\rho_g}\frac{\partial \dot{M}_g}{\partial x} + \frac{1}{A\rho_{lp}}\frac{\partial \rho_{lp}(1-\beta)\frac{\dot{M}_l}{\rho_l}}{\partial x} + \frac{1}{A\rho_{lc}}\frac{\partial \rho_{lc}\beta\frac{\dot{M}_l}{\rho_l}}{\partial x}+ \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)\frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\frac{\dot{M}_l}{\rho_l}}{\partial x} =\frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} - \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)\left[A(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\frac{\partial(1-\alpha)(1-\beta)}{\partial t}\right]- \left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial T}\right|_p \frac{\partial T}{\partial t} + \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}}{\partial t}\right]$$

It should be noted that the flow rates of interest are not $\dot{M}_g$ and $\dot{M}_l$ individually, but rather the **mixture mass flow rate** $\dot{M}_m$. Using equations \eqref{eq:M_l_from_M_m}, \eqref{eq:M_g_from_M_m}, \eqref{eq:T1} and \eqref{eq:T2} the more appropriate form for the pressure–mixture mass flow rate coupling is:

$$\frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial p}\right|_T \frac{\partial p}{\partial t} + \frac{1}{A\rho_g}\frac{\partial (1-T_1)\dot{M}_m - T_2}{\partial x} + \frac{1}{A\rho_{lp}}\frac{\partial \rho_{lp}(1-\beta)\frac{T_1\dot{M}_m+T_2}{\rho_l}}{\partial x} + \frac{1}{A\rho_{lc}}\frac{\partial \rho_{lc}\beta\frac{T_1\dot{M}_m+T_2}{\rho_l}}{\partial x}+ \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)\frac{\partial(1-\beta)(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\frac{T_1\dot{M}_m+T_2}{\rho_l}}{\partial x} =\frac{\Gamma_{lp}}{A\rho_{lp}\Delta L} + \frac{\Gamma_{cp}}{A\rho_{lc}\Delta L} + \frac{\Gamma_g}{A\rho_g\Delta L} - \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)\left[A(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}\frac{\partial(1-\alpha)(1-\beta)}{\partial t}\right]-\left[\frac{(1-\alpha)(1-\beta)}{\rho_{lp}}\frac{\partial \rho_{lp}}{\partial t} + \frac{(1-\alpha)\beta}{\rho_{lc}}\frac{\partial \rho_{lc}}{\partial t} + \frac{\alpha}{\rho_g}\left.\frac{\partial \rho_g}{\partial T}\right|_p\frac{\partial T}{\partial t} + \frac{1}{A}\left(\frac{1}{\rho_g} - \frac{1}{\rho_{lp}}\right)A(1-\alpha)(1-\beta)\frac{\partial(1-F_w)\frac{R_s \gamma_g \rho_{ar}^{std}}{B_o}}{\partial t}\right]$$

With all flux divergence terms now expressed exclusively in terms of $\dot{M}_m$ through the drift-flux coefficients $T_1$ and $T_2$, this equation — together with the simplified momentum equation \eqref{eq:simplified_momentum} — forms the **closed system** to be solved implicitly for the pressure and mixture mass flow rate profiles at each time step.

## Property Tracking

In addition to the conservation equations derived above, a set of partial differential equations is used to track fluid properties — namely, the gas-oil ratio (GOR), free gas density, dead oil API gravity, water cut (BSW), and pseudo-component fractions (the latter in the compositional case). These equations are solved in a **marching fashion**, always immediately after the holdup, $\beta$, pressure, temperature, and mixture mass flow rate profiles have been obtained for the current time step.

Black-oil models are, in essence, phase-equilibrium models that treat petroleum as a **two-component system**: a light component (referred to as gas), which can exist either in the liquid phase — where it is called dissolved gas — or in the free gas phase; and a heavier component (referred to as dead oil), which always remains in the liquid phase. Consequently, the liquid hydrocarbon mixture always contains two components: dead oil plus dissolved gas.

The solution gas-oil ratio, formation volume factors, gas specific gravity, gas pseudo-critical pressure and temperature, oil viscosity, emulsion viscosity corrections, and many other properties are computed from the same black-oil correlations implemented in `Marlim2`. When PVT tables are provided, all these properties are read directly, which makes fluid property determination simpler and faster. In general, variables such as the solubility ratio and the formation volume factors are not directly tabulated in PVT tables, but converting the tabulated properties is a straightforward operation.

For black-oil models, inputs such as API gravity, water cut (BSW), GOR, gas specific gravity, and $\text{CO}_2$ mole fraction are required for property calculations, in addition to pressure and temperature. `Marlim3` mass sources carry fixed fluid characterization variables; however, along the production system these variables change as mass sources with different fluid properties mix throughout the well. In this work, emphasis is placed on the tracking of GOR, gas specific gravity, and $\text{CO}_2$ fraction, for which dedicated transport equations have been implemented.

It is important to note that in all equations presented below, **instantaneous thermodynamic equilibrium** between components is assumed. This is a simplification — in reality, the kinetics of phase change should be considered, and the assumption becomes increasingly inaccurate when a high $\text{CO}_2$ fraction is present. Unfortunately, due to its complexity, this aspect has not yet been incorporated into the model.

A key observation common to all property-tracking equations — including compositional tracking — is that they are **less critical** to the multiphase flow solution than the conservation equations for holdup, pressure, and mass flow rate, and are simultaneously strongly associated with **slow propagation phenomena**. They can therefore be treated entirely **explicitly** within a semi-implicit numerical scheme. In the numerical solution, the tracking equations are always resolved **last**, once the holdup, $\beta$, pressure, mass flow rate, and temperature profiles are already available for the current time level. This makes them the easiest equations to advance in time.

### Light-component mass conservation

The first property-tracking equation is the **mass conservation of the light component** in the black-oil model. It is formulated in terms of the volume of the light component at standard conditions, which is equivalent to tracking the mass of this component. This equation has direct value for black-oil property tracking, since the balance of the light and heavy fractions underpins the evaluation of all other fluid properties.

$$
\frac{\partial \text{Vol}_{\text{light}}}{\partial t}
+ \frac{\partial}{\partial x}\left[\frac{(1-\beta)(1-F_w) R_s Q_l}{B_o}\right]
+ \frac{\partial}{\partial x}\left[\frac{\dot{M}_g}{\rho_g^{\text{std}}}\right]
= \frac{\Gamma_{lp}}{\Delta L} \frac{R_s}{\rho_{lp}^{\text{std}}}(1-F_w)
+ \frac{\Gamma_g}{\Delta L} \frac{1}{\rho_{gf}^{\text{std}}}
\label{eq:light_component_conservation}
$$

where

$$
\text{Vol}_{\text{light}} = A\left[(1-\alpha)(1-\beta)(1-F_w)\frac{R_s}{B_o} + \alpha \frac{\rho_g}{\rho_g^{\text{std}}}\right]
\label{eq:vol_light}
$$

### Free gas specific gravity tracking

The transport equation for the **free gas specific gravity** $\gamma_{gl}$ is derived directly from Equation \eqref{eq:light_component_conservation}:

$$
\frac{\partial \gamma_{gl} \, \text{Vol}_{\text{light}}}{\partial t}
+ \frac{\partial}{\partial x}\left[\frac{\gamma_g (1-\beta)(1-F_w) R_s Q_l}{B_o} \right]
+ \frac{\partial \gamma_{gl} \dot{M}_g \rho_g^{\text{std}}}{\partial x}
= \gamma_g \frac{\Gamma_{lp}}{\Delta L} \frac{R_s}{\rho_{lp}^{\text{std}}}(1-F_w)
+ \gamma_{gf} \frac{\Gamma_g}{\Delta L} \frac{1}{\rho_{gf}^{\text{std}}}
\label{eq:gas_gravity_tracking}
$$

where $\gamma_{gl}$ is the free gas specific gravity.

### CO$_2$ mole fraction tracking

Analogously, the transport equation for the **$\text{CO}_2$ mole fraction** $y_{\text{CO}_2}$ is:

$$
\frac{\partial y_{\text{CO}_2} \, \text{Vol}_{\text{light}}}{\partial t}
+ \frac{\partial}{\partial x}\left[\frac{y_{\text{CO}_2}(1-\beta)(1-F_w)R_s  Q_l}{B_o}\right]
+ \frac{\partial y_{\text{CO}_2} \dot{M}_g \rho_g^{\text{std}}}{\partial x}
= y_{\text{CO}_2} \frac{\Gamma_{lp}}{\Delta L} \frac{R_s}{\rho_{lp}^{\text{std}}}(1-F_w)
+ {y_{\text{CO}_2}}\big|_f \frac{\Gamma_g}{\Delta L} \frac{1}{\rho_{gf}^{\text{std}}}
\label{eq:co2_tracking}
$$

### Instantaneous GOR

The GOR is generally a model input corresponding to the ratio of gas volume to oil volume at standard conditions, measured under a specific separation process. This implies that GOR is not solely an intrinsic fluid property — it also depends on the separation conditions, and can vary significantly for fluids with high dissolved-gas content.

In a transient simulation, an additional complication arises: the GOR obtained from a separation test, which characterizes the fluid produced by the reservoir, should not be held constant along the tubing. Transient phenomena such as production shut-in and fluid segregation can lead to fluid states along the production line with a GOR very different from the original reservoir fluid GOR. The concept of **instantaneous GOR** must therefore be adopted: the ratio of the light-component volume to the dead-oil volume at standard conditions at a given pipe location and time instant:

$$
\text{GOR} = \frac{\text{Vol}_{\text{light}} \, B_o}{(1-\alpha)(1-\beta)(1-F_w) A}
\label{eq:gor_instantaneous}
$$

The GOR, as well as the gas specific gravity and mole fractions, must be **continuously re-evaluated** during a transient simulation that uses a black-oil property model.

### API gravity tracking

The API gravity tracking equation is:

$$
\frac{\partial \left(\text{API} \cdot \text{Vol}_{\text{dead oil}}\right)}{\partial t}
+ \frac{\partial}{\partial x}\left[\text{API} \cdot \frac{(1-\beta)(1-F_w) Q_l}{B_o}\right]
= \text{API} \cdot \frac{\Gamma_{\text{dead oil}}}{\Delta L}
\label{eq:api_tracking}
$$

### BSW tracking

The **BSW (Basic Sediment and Water)** tracking equation is:

$$
\frac{\partial \left[\text{BSW} \left(\text{Vol}_{\text{dead oil}} + \text{Vol}_{\text{water}}\right)\right]}{\partial t}
+ \frac{\partial}{\partial x}\left[\text{BSW} \cdot \frac{(1-\beta) Q_l}{B_o}\right]
= \text{BSW} \left(\frac{\Gamma_{\text{dead oil}}}{\Delta L} + \frac{\Gamma_{\text{water}}}{\Delta L}\right)
\label{eq:bsw_tracking}
$$

where:

$$
\text{Vol}_{\text{dead oil}} = \frac{(1-\beta)(1-F_w) \alpha}{B_o}
$$

$$
\text{Vol}_{\text{water}} = \frac{(1-\beta) F_w \alpha}{B_a}
$$

## References

Oliveira, G.P.H., (1995), Estabilidade Operacional de Poços com Gas-Lift Contínuo, Doctoral Thesis, UNICAMP/FEM/DEP.