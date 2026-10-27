In `Marlim3` multiphase flow model, temperature is advected predominantly by the slowest wave family, at a speed close to the mean flow velocity. Pressure, by contrast, adjusts through the much faster acoustic families. As a result, in any transient where an initial imbalance exists — for instance a cold fluid upstream of a hot fluid separated by a closed valve — the pressure profile reaches a quasi-steady distribution long before the thermal front has propagated appreciably. The thermal field remains genuinely hyperbolic and continues to evolve at the slower flow timescale. This separation justifies treating the energy equation with an explicit convective scheme driven by the already-known flow velocity, while pressure is handled implicitly.

Assuming a one-dimensional model, the energy conservation equations for each phase (liquid and gas) of the stream are:

$$\begin{aligned}
\underbrace{\frac{\partial}{\partial t}\!\left[\rho_g\!\left(e_g+\frac{u_g^{2}}{2}\right)\alpha A\right]}_{\text{Energy variation in the control volume}}
\;+\;
\underbrace{\frac{\partial}{\partial x}\!\left[\rho_g u_g\!\left(e_g+\frac{u_g^{2}}{2}\right)\alpha A\right]}_{\text{Energy transport across the control-volume boundary}}
&=
-\underbrace{\frac{\partial}{\partial x}\!\left(p\,u_g\,\alpha A\right)}_{\text{Boundary work}}
\\[6pt]
&\quad
+\underbrace{\left(\rho_g u_g \alpha\right)A g}_{\text{Potential energy variation}}
+\underbrace{Q_g}_{\text{Heat flux}}
+\underbrace{\frac{h_{Fg}\,\Gamma_g}{\Delta l}}_{\text{Mass source}}
+\underbrace{h_t\Psi_g}_{\text{Phase change}}
\end{aligned} \label{eq:gas_energy_initial}$$

$$
\begin{aligned}
\underbrace{\frac{\partial}{\partial t}\!\left[\rho_l\!\left(e_l+\frac{u_l^{2}}{2}\right)(1-\alpha) A\right]}_{\text{Energy variation in the control volume}}
\;+\;
\underbrace{\frac{\partial}{\partial x}\!\left[\rho_l u_l\!\left(e_l+\frac{u_l^{2}}{2}\right)(1-\alpha) A\right]}_{\text{Energy transport across the control-volume boundary}}
&=
-\underbrace{\frac{\partial}{\partial x}\!\left(p\,u_l\,(1-\alpha) A\right)}_{\text{Boundary work}}
\\[6pt]
&\quad
+\underbrace{\rho_l u_l(1-\alpha)A g}_{\text{Potential energy variation}}
+\underbrace{Q_l}_{\text{Heat flux}}
+\underbrace{\frac{h_{Flp}\Gamma_{lp}+h_{Flc}\Gamma_{lc}}{\Delta l}}_{\text{Mass source}}
-\underbrace{h_t\Psi_g}_{\text{Phase change}}
\end{aligned} \label{eq:liq_energy_initial}
$$

Equation \eqref{eq:liq_energy_initial} governs energy conservation of the liquid phase, which, in the context of homogeneous flow, may consist of a mixture of two liquids: a production liquid, referred to by the subscript $p$, and a complementary liquid, referred to by the subscript $c$. The volumetric fraction of the complementary liquid within the liquid phase is denoted by $\beta$. The properties $\rho_l$ and $e_l$ in \eqref{eq:liq_energy_initial} are computed as a weighted average of the corresponding properties of these two fluids.

Both \eqref{eq:gas_energy_initial} and \eqref{eq:liq_energy_initial} include a term accounting for energy transfer due to phase change (vaporization and condensation). This term is, by definition, equal in magnitude in both equations.

Equations \eqref{eq:gas_energy_initial} and \eqref{eq:liq_energy_initial} are written in conservative form. Since the numerical model operates on the primitive variables — pressure and temperature — rather than on enthalpies and internal energies directly, it is more convenient to recast the energy equations in non-conservative form. Applying the chain rule:

$$\rho_g\alpha A\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+\rho_gu_g\alpha A\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial x}+\left(e_g+\frac{u_g^2}{2}\right)\frac{\partial\rho_g\alpha A}{\partial t}+\left(e_g+\frac{u_g^2}{2}\right)\frac{\partial\rho_gu_g\alpha A}{\partial x}+u_g\rho_g\alpha A\frac{\partial\frac{p}{\rho_g}}{\partial x}+\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}=-\left(\rho_gu_g\alpha_g\right)Ag+Q_g+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+h_t\psi_g$$

$$\rho_l\left(1-\alpha\right)A\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+\rho_lu_l\left(1-\alpha\right)A\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial x}+\left(e_l+\frac{u_l^2}{2}\right)\frac{\partial\rho_l\left(1-\alpha\right)A}{\partial t}+\left(e_l+\frac{u_l^2}{2}\right)\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}+u_l\rho_l\left(1-\alpha\right)A\frac{\partial\frac{p}{\rho_l}}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\rho_lu_l\left(1-\alpha\right)Ag+Q_l+h_{Fonte}\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-h_t\psi_g$$

Regrouping some terms:

$$\rho_g\alpha A\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+\rho_gu_g\alpha A\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}+\left(e_g+\frac{u_g^2}{2}\right)\frac{\partial\rho_g\alpha A}{\partial t}+\left(e_g+\frac{u_g^2}{2}\right)\frac{\partial\rho_gu_g\alpha A}{\partial x}+\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}=-\left(\rho_gu_g\alpha_g\right)Ag+Q_g+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+h_t\psi_g \label{eq:energ_intermediario_gas}$$

$$\rho_l\left(1-\alpha\right)A\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+\rho_lu_l\left(1-\alpha\right)A\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}+\left(e_l+\frac{u_l^2}{2}\right)\frac{\partial\rho_l\left(1-\alpha\right)A}{\partial t}+\left(e_l+\frac{u_l^2}{2}\right)\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\rho_lu_l\left(1-\alpha\right)Ag+Q_l+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-h_t\psi_g \label{eq:energ_intermediario_liquido}$$

At this point, it is worth noting that enthalpy arises in the equations only upon transition to the non-conservative form. In the conservative formulation, even within the spatial derivative, it is not straightforward to work with enthalpy directly; instead, internal energy and pressure-volume work must be treated separately.

To proceed, the following mass conservation relations are employed:

$$A\frac{\partial\rho_{lp}\left(1-\alpha\right)\left(1-\beta\right)}{\partial t}+\frac{\partial{\dot{M}}_p}{\partial x}=\frac{\mathrm{\Gamma}_{lp}}{\mathrm{\Delta L}}-\psi \label{eq:rel_cons_mass1}$$

$$A\frac{\partial\rho_{lc}\left(1-\alpha\right)\beta}{\partial t}+\frac{\partial{\dot{M}}_c}{\partial x}=\frac{\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}} \label{eq:rel_cons_mass2}$$

$$A\frac{\partial\rho_g\alpha}{\partial t}+\frac{\partial{\dot{M}}_g}{\partial x}=\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi$$

Adding \eqref{eq:rel_cons_mass1} and \eqref{eq:rel_cons_mass2}:

$$A\frac{\partial\rho_l\left(1-\alpha\right)}{\partial t}+\frac{\partial{\dot{M}}_l}{\partial x}=\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi$$

Where:

$$
\rho_l = \rho_{lp}(1-\beta) + \rho_{lc}\beta
$$

$$
\dot{M}_p = \rho_{lp}(1-\beta)(1-\alpha)\,u_l\,A
$$

$$
\dot{M}_c = \rho_{lc}\beta(1-\alpha)\,u_l\,A
$$

$$
\dot{M}_l = \dot{M}_c + \dot{M}_p
$$

$$
\dot{M}_g = \rho_g\,\alpha\,u_g\,A \label{eq:last_dotM}
$$

Applying \eqref{eq:rel_cons_mass1} to \eqref{eq:last_dotM} in \eqref{eq:energ_intermediario_gas} and \eqref{eq:energ_intermediario_liquido}:

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}=-\left(e_g+\frac{u_g^2}{2}\right)\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-\left(\rho_gu_g\alpha_g\right)Ag+Q_g+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+h_t\psi_g$$

$$\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\left(e_l+\frac{u_l^2}{2}\right)\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)-\rho_lu_l\left(1-\alpha\right)Ag+Q_l+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-h_t\psi_g$$

At this stage, it becomes convenient to sum the two energy equations, since in `Marlim3` the pressure and temperature of each phase are assumed to be equal.

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]+\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\left(e_g+\frac{u_g^2}{2}\right)\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-\left(e_l+\frac{u_l^2}{2}\right)\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)-\left(\rho_gu_g\alpha_g\right)Ag-\rho_lu_l\left(1-\alpha\right)Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}} \label{eq:energy_sum}$$

It is worth noting that the term $h_t \psi_g$ vanishes upon summation of the two equations.

In \eqref{eq:energy_sum}, the term $\frac{p}{\rho_g}\frac{\partial\rho_g u_g \alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_l u_l\left(1-\alpha\right)A}{\partial x}$ can be simplified by means of equations \eqref{eq:rel_cons_mass1} through \eqref{eq:last_dotM}, neglecting the time variation of liquid densities:

$$\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-Ap\frac{\partial\alpha}{\partial t}-\frac{A}{\rho_g}p\alpha\frac{\partial\rho_g}{\partial t}+\frac{1}{\rho_g}p\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-Ap\frac{\partial\left(1-\alpha\right)}{\partial t}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lp}-\rho_{lc}\right)\frac{\partial\beta}{\partial t}+\frac{1}{\rho_l}p\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right) \label{eq:pressure_work_expand}$$

Reorganizing:

$$\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\frac{A}{\rho_g}p\alpha\frac{\partial\rho_g}{\partial t}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lp}-\rho_{lc}\right)\frac{\partial\beta}{\partial t}+\frac{1}{\rho_g}\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)+\frac{1}{\rho_l}\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right) \label{eq:pressure_work_reorg}$$

Or:

$$\frac{p}{\rho_g}\frac{\partial\rho_gu_g\alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_lu_l\left(1-\alpha\right)A}{\partial x}=-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lp}-\rho_{lc}\right)\frac{\partial\beta}{\partial t}+\frac{p}{\rho_g}\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)+\frac{p}{\rho_l}\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right) \label{eq:pressure_work_final}$$

Applying \eqref{eq:pressure_work_final} in \eqref{eq:energy_sum}:

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)+\frac{p}{\rho_g}\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)+\frac{p}{\rho_l}\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)=-\left(e_g+\frac{u_g^2}{2}\right)\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-\left(e_l+\frac{u_l^2}{2}\right)\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)-\left(\rho_gu_g\alpha_g\right)Ag-\rho_lu_l\left(1-\alpha\right)Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_108}$$

Reorganizing:

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)=-\left(e_g+\frac{p}{\rho_g}+\frac{u_g^2}{2}\right)\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-\left(e_l+\frac{p}{\rho_l}+\frac{u_l^2}{2}\right)\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)-\left(\rho_gu_g\alpha_g\right)Ag-\rho_lu_l\left(1-\alpha\right)Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_109}$$

From the relation $e_f+\frac{p}{\rho_f}=h_f$:

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)=-\left(h_g+\frac{u_g^2}{2}\right)\left(\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\psi\right)-\left(h_l+\frac{u_l^2}{2}\right)\left(\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\psi\right)-\left(\rho_gu_g\alpha_g\right)Ag-\rho_lu_l\left(1-\alpha\right)Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_110}$$

$$\rho_g\alpha A\left[\frac{\partial\left(e_g+\frac{u_g^2}{2}\right)}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[\frac{\partial\left(e_l+\frac{u_l^2}{2}\right)}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)=-\left(h_g+\frac{u_g^2}{2}\right)\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}-\left(h_l+\frac{u_l^2}{2}\right)\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\left(h_g-h_l+\frac{u_g^2}{2}-\frac{u_l^2}{2}\right)\psi-\left(\rho_gu_g\alpha_g\right)Ag-\rho_lu_l\left(1-\alpha\right)Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_111}$$


In \eqref{eq:energy_111}, a term directly related to the interphase mass transfer rate appears, $\left(h_g - h_l + \frac{u_g^2}{2} - \frac{u_l^2}{2}\right)\psi$. This term differs from the original term $h_t\psi_g$ present in the energy conservation equation for each individual phase. It emerges exclusively in the non-conservative system as a direct consequence of the manipulation of $\frac{p}{\rho_g}\frac{\partial\rho_g u_g \alpha A}{\partial x}+\frac{p}{\rho_l}\frac{\partial\rho_l u_l\left(1-\alpha\right)A}{\partial x}$, and therefore carries physical meaning only within the non-conservative energy transport relations.

As previously stated, `Marlim3` operates directly with pressure and temperature as primitive variables. It is therefore necessary to invoke the thermodynamic relations between enthalpy, internal energy, pressure, and temperature in order to arrive at the final form of the energy transport equation used in `Marlim3`.

From Van Wylen & Sonntag (1989):

$$dh=c_pdT+\left(\frac{1}{\rho}-T\left.\frac{\partial\frac{1}{\rho}}{\partial T}\right|_p\right) dp \label{eq:dh_generic}$$

$$de=c_vdT-\frac{1}{\rho^2}\left(T\left.\frac{\partial p}{\partial T}\right|_\rho-p\right)d\rho \label{eq:de_generic}$$

For the gas phase, Relation \eqref{eq:de_generic} does not explicitly show the differential of pressure for internal energy, one must therefore substitute the differential of density by the appropriate relation of the differentials of temperature and pressure.

Considering the real gas relation:

$$p=Rz\left(p,T\right)\rho_gT \label{eq:real_gas}$$

From \eqref{eq:real_gas}

$$\left.\frac{\partial p}{\partial T}\right|_{\rho_g}=R\rho_gz+R\rho_gT\left.\frac{\partial z}{\partial T}\right|_{\rho_g} \label{eq:dpdT_rho}$$


Therefore:

$$\left.\frac{\partial z}{\partial T}\right|_{\rho_g}=\left.\frac{\partial z}{\partial T}\right|_p+\left.\frac{\partial z}{\partial p}\right|_T\left.\frac{\partial p}{\partial T}\right|_{\rho_g} \label{eq:dzdt_rho}$$

Then:

$$\left.\frac{\partial p}{\partial T}\right|_{\rho_g}=R\rho_gz+R\rho_gT\left(\left.\frac{\partial z}{\partial T}\right|_p+\left.\frac{\partial z}{\partial p}\right|_T\left.\frac{\partial p}{\partial T}\right|_{\rho_g}\right)\Rightarrow 
\left(1-R\rho_gT\left.\frac{\partial z}{\partial p}\right|_T\right)\left.\frac{\partial p}{\partial T}\right|_{\rho_g}=R\rho_gz+R\rho_gT\left.\frac{\partial z}{\partial T}\right|_p\Rightarrow$$ 

$$\left.\frac{\partial p}{\partial T}\right|_{\rho_g}=\frac{R\rho_gz+R\rho_gT\left.\frac{\partial z}{\partial T}\right|_p}{\left(1-R\rho_gT\left.\frac{\partial z}{\partial p}\right|_T\right)} \label{eq:dpdT_final}$$

Applying \eqref{eq:dpdT_final} to \eqref{eq:de_generic}:

$$de_g=c_{vg}dT+\frac{1}{\rho_g^2}\left(p-R\rho_gT\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{1-R\rho_gT\left.\frac{\partial z}{\partial p}\right|_T}\right)d\rho \label{eq:de_g_rho}$$

Where:

$$\left.d\rho=\frac{\partial\rho}{\partial T}\right|_pdT+\left.\frac{\partial\rho}{\partial p}\right|_Tdp \label{eq:drho_total}$$

Applying \eqref{eq:drho_total} in \eqref{eq:de_g_rho}:

$$de_g=\left[c_{vg}+\frac{1}{\rho_g^2}\left.\frac{\partial\rho_g}{\partial T}\right|_p\left(p-R\rho_gT\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{1-R\rho_gT\left.\frac{\partial z}{\partial p}\right|_T}\right)\right]dT+\frac{1}{\rho_g^2}\left.\frac{\partial\rho_g}{\partial p}\right|_T\left(p-R\rho_gT\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{1-R\rho_gT\left.\frac{\partial z}{\partial p}\right|_T}\right)dp\ \ \label{eq:de_g_Tp}$$

Where:

$$\left.\frac{\partial\rho_g}{\partial T}\right|_p=-\rho_g\left(\frac{1}{T}+\frac{1}{z}\left.\frac{\partial z}{\partial T}\right|_p\right) \label{eq:drho_g_dT}$$

$$\left.\frac{\partial\rho_g}{\partial p}\right|_T=\rho_g\left(\frac{1}{p}-\frac{1}{z}\left.\frac{\partial z}{\partial p}\right|_T\right) \label{eq:drho_g_dp}$$

Applying \eqref{eq:drho_g_dT} and \eqref{eq:drho_g_dp} in \eqref{eq:de_g_Tp}:

$$de_g=\left[c_{vg}-R\left(z+\left.\frac{\partial z}{\partial T}\right|_pT\right)\left(1-\frac{z+\left.\frac{\partial z}{\partial T}\right|_pT}{z-\left.\frac{\partial z}{\partial p}\right|_Tp}\right)\right]dT+\frac{1}{z\rho_g}\left(z-p\left.\frac{\partial z}{\partial p}\right|_T\right)\left(1-\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{z-p\left.\frac{\partial z}{\partial p}\right|_T}\right)dp \label{eq:de_g_simplified}$$

From this moment on, for convenience, the following variable will be defined:

$$de_g=c_{vg}^\prime dT+\frac{1}{z\rho_g}\left(z-p\left.\frac{\partial z}{\partial p}\right|_T\right)\left(1-\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{z-p\left.\frac{\partial z}{\partial p}\right|_T}\right)dp\Rightarrow 
c_{vg}^\prime=c_{vg}-R\left(z+\left.\frac{\partial z}{\partial T}\right|_pT\right)\left(1-\frac{z+\left.\frac{\partial z}{\partial T}\right|_pT}{z-\left.\frac{\partial z}{\partial p}\right|_Tp}\right) \label{eq:cvg_prime_def}$$

For the liquid phase, only the temperature-dependent contribution to the internal energy variation is retained:

$$de_l=c_{vl}dT \label{eq:de_l}$$

Substituting \eqref{eq:cvg_prime_def} and \eqref{eq:de_l} into \eqref{eq:energy_111}, and neglecting kinetic energy variations in the source terms:

$$\rho_g\alpha A\left[c_{vg}^\prime\frac{\partial T}{\partial t}+\frac{\partial\frac{u_g^2}{2}}{\partial t}+\frac{1}{z\rho_g}\left(z-p\left.\frac{\partial z}{\partial p}\right|_T\right)\left(1-\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{z-p\left.\frac{\partial z}{\partial p}\right|_T}\right)\frac{\partial p}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[c_{vl}\frac{\partial T}{\partial t}++\frac{\partial\frac{u_l^2}{2}}{\partial t}+u_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]-\frac{Ap}{\rho_g}\alpha\left(\frac{\partial\rho_g}{\partial T}\frac{\partial T}{\partial t}+\frac{\partial\rho_g}{\partial p}\frac{\partial p}{\partial t}\right)=-h_g\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}-h_l\frac{\mathrm{\Gamma}_{lp}+\mathrm{\Gamma}_{cp}}{\mathrm{\Delta L}}-\left(h_g-h_l\right)\psi-\left[\rho_gu_g\alpha_g+\rho_lu_l\left(1-\alpha\right)\right]Ag+Q_w+\frac{h_{Fg}\mathrm{\Gamma}_g}{\mathrm{\Delta l}}+\frac{\left(h_{Flp}\mathrm{\Gamma}_{lp}+h_{Flc}\mathrm{\Gamma}_{lc}\right)}{\mathrm{\Delta l}}-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_127}$$

Applying \eqref{eq:drho_g_dT} and \eqref{eq:drho_g_dp} in \eqref{eq:energy_127}:

$$\rho_g\alpha A\left[c_{vg}^\prime\frac{\partial T}{\partial t}+\frac{1}{z\rho_g}\left(z-p\left.\frac{\partial z}{\partial p}\right|_T\right)\left(\frac{z+T\left.\frac{\partial z}{\partial T}\right|_p}{z-p\left.\frac{\partial z}{\partial p}\right|_T}\right)\frac{\partial p}{\partial t}+\frac{\partial\frac{u_g^2}{2}}{\partial t}+u_g\frac{\partial\left(h_g+\frac{u_g^2}{2}\right)}{\partial x}\right]+\rho_l\left(1-\alpha\right)A\left[c_{vl}\frac{\partial T}{\partial t}+{\frac{\partial\frac{u_l^2}{2}}{\partial t}+u}_l\frac{\partial\left(h_l+\frac{u_l^2}{2}\right)}{\partial x}\right]=\left(h_{Fg}-h_g\right)\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\left(h_{Flp}-h_l\right)\frac{\mathrm{\Gamma}_{lp}}{\mathrm{\Delta l}}+\left(h_{Flc}-h_l\right)\frac{\mathrm{\Gamma}_{lc}}{\mathrm{\Delta l}}-\left(h_g-h_l\right)\psi-\left[\rho_gu_g\alpha_g+\rho_lu_l\left(1-\alpha\right)\right]Ag+Q_w-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_128}$$

Where:

$$c_{vg}^\prime=c_{vg}+R\left(z+\left.\frac{\partial z}{\partial T}\right|_pT\right)\left(\frac{z+\left.\frac{\partial z}{\partial T}\right|_pT}{z-\left.\frac{\partial z}{\partial p}\right|_Tp}\right) \label{eq:cvg_prime_final}$$

Let's consider the enthalpy variations. For the gas:

$$dh_g=c_{pg}dT+\left(\frac{1}{\rho_g}+\frac{T}{\rho_g^2}\left.\frac{\partial\rho_g}{\partial T}\right|_p\right)dp \label{eq:dh_g_general}$$

Applying \eqref{eq:drho_g_dT} in \eqref{eq:dh_g_general}:

$$dh_g=c_{pg}dT-\frac{T}{z\rho_g}\left.\frac{\partial z}{\partial T}\right|_pdp \label{eq:dh_g_Jg}$$

Defining:

$$J_g=\frac{T}{z\rho_g}\left.\frac{\partial z}{\partial T}\right|_p \label{eq:Jg_def}$$

$J_g$ is the Joule-Thompson coefficient of the gas multiplied by its specific heat at constant pressure.

For the liquid:

$$dh_l=c_{pl}dT+\left(\frac{1}{\rho_l}+\frac{T}{\rho_l^2}\left.\frac{\partial\rho_l}{\partial T}\right|_p\right)dp \label{eq:dh_l_general}$$

For the liquid phase, the term $\left.\frac{\partial\rho_l}{\partial T}\right|_p$ may be relevant to the energy equation, particularly at high solubility ratio conditions. However, since this term is not currently computed, the following simplification is adopted:

$$dh_l=c_{pl}dT+\left(\frac{1}{\rho_l}\right)dp \label{eq:dh_l_simplified}$$

The properties of the liquid phase components must be weighted by their respective mass fractions:

$$x_O=\frac{\rho_O\left(1-F_W\right)\left(1-\beta\right)}{\rho_O\left(1-F_W\right)\left(1-\beta\right)+\rho_WF_W\left(1-\beta\right)+\rho_{lc}\beta} \label{eq:x_O}$$

$$x_W=\frac{\rho_WF_W\left(1-\beta\right)}{\rho_O\left(1-F_W\right)\left(1-\beta\right)+\rho_WF_W\left(1-\beta\right)+\rho_{lc}\beta} \label{eq:x_W}$$

$$x_{lc}=\frac{\rho_{lc}\beta}{\rho_O\left(1-F_W\right)\left(1-\beta\right)+\rho_WF_W\left(1-\beta\right)+\rho_{lc}\beta} \label{eq:x_lc}$$

With this:

$$J_l=-x_O\left(\frac{1}{\rho_o}+\frac{T}{\rho_o^2}\left.\frac{\partial\rho_o}{\partial T}\right|_p\right)-x_W\left(\frac{1}{\rho_w}+\frac{T}{\rho_w^2}\left.\frac{\partial\rho_w}{\partial T}\right|_p\right)-x_{\mathrm{lc}}\left(\frac{1}{\rho_c}+\frac{T}{\rho_c^2}\left.\frac{\partial\rho_c}{\partial T}\right|_p\right) \label{eq:Jl_def}$$

$$dh_g=c_{pg}dT-J_gdp \label{eq:dh_g}$$

$$dh_l=c_{pl}dT-J_ldp \label{eq:dh_l}$$

Applying \eqref{eq:Jl_def} and \eqref{eq:dh_l} into \eqref{eq:energy_128}:

$$\rho_g\alpha A\left[c_{vg}^\prime\frac{\partial T}{\partial t}+\frac{1}{z\rho_g}\left(z+T\left.\frac{\partial z}{\partial T}\right|_p\right)\frac{\partial p}{\partial t}+\frac{\partial\frac{u_g^2}{2}}{\partial t}+u_g\left(c_{pg}\frac{\partial T}{\partial x}-J_g\frac{\partial p}{\partial x}+u_g\frac{\partial u_g}{\partial x}\right)\right]+\rho_l\left(1-\alpha\right)A\left[c_{vl}\frac{\partial T}{\partial t}+{\frac{\partial\frac{u_l^2}{2}}{\partial t}+u}_l\left(c_{pl}\frac{\partial T}{\partial x}-J_l\frac{\partial p}{\partial x}+u_l\frac{\partial u_l}{\partial x}\right)\right]=\left(h_{Fg}-h_g\right)\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\left(h_{Flp}-h_l\right)\frac{\mathrm{\Gamma}_{lp}}{\mathrm{\Delta l}}+\left(h_{Flc}-h_l\right)\frac{\mathrm{\Gamma}_{lc}}{\mathrm{\Delta l}}-\left(h_g-h_l\right)\psi-\left[\rho_gu_g\alpha_g+\rho_lu_l\left(1-\alpha\right)\right]Ag+Q_w-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_140}$$

Reorganizing:

$$\left[\rho_g\alpha A c_{vg}^\prime+\rho_l\left(1-\alpha\right)Ac_{vl}\right]\frac{\partial T}{\partial t}+\rho_g\alpha A\frac{1}{z\rho_g}\left(z+T\left.\frac{\partial z}{\partial T}\right|_p\right)\frac{\partial p}{\partial t}+\left[\rho_g\alpha A u_gc_{pg}+\rho_l\left(1-\alpha\right)Au_lc_{pl}\right]\frac{\partial T}{\partial x}-\left[\rho_g\alpha A u_gJ_g+\rho_l\left(1-\alpha\right)Au_lJ_l\right]\frac{\partial p}{\partial x}+\left(\rho_g\alpha A\right)\left(\frac{\partial\frac{u_g^2}{2}}{\partial t}+u_g^2\frac{\partial u_g}{\partial x}\right)+\left[\rho_l\left(1-\alpha\right)A\right]\left(\frac{\partial\frac{u_l^2}{2}}{\partial t}+u_l^2\frac{\partial u_l}{\partial x}\right)=\left(h_{Fg}-h_g\right)\frac{\mathrm{\Gamma}_g}{\mathrm{\Delta L}}+\left(h_{Flp}-h_l\right)\frac{\mathrm{\Gamma}_{lp}}{\mathrm{\Delta l}}+\left(h_{Flc}-h_l\right)\frac{\mathrm{\Gamma}_{lc}}{\mathrm{\Delta l}}-\left(h_g-h_l\right)\psi-\left[\rho_gu_g\alpha_g+\rho_lu_l\left(1-\alpha\right)\right]Ag+Q_w-\frac{A}{\rho_l}p\left(1-\alpha\right)\left(\rho_{lc}-\rho_{lp}\right)\frac{\partial\beta}{\partial t} \label{eq:energy_final}$$

\eqref{eq:energy_final} is the current form of the equation being used in `Marlim3` for calculating temperature in transient processes.

## References

Van Wylen, G., Sonntag, R., (1989), Fundamentals of Classical Thermodynamics.