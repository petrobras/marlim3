#ifndef SIMULATIONCONTEXT_H_
#define SIMULATIONCONTEXT_H_
#define _USE_MATH_DEFINES // Enables access to the M_PI constant

#include <math.h>
#include "Log.h"

/// Shared numerical settings and runtime state used by a simulation and its coupled solvers.
struct SimulationContext {
    /// Finite-volume startup mode used by face-pressure initialization.
    int finiteVolumeStartupMode;
    /// Relaxation factor for finite-volume velocity and mass-flow updates.
    double finiteVolumeRelaxationFactor;
    /// Relaxation factor used in the additional velocity-correction step.
    double velocityCorrectionRelaxationFactor;
    /// Relaxation factor applied to pressure corrections.
    double pressureCorrectionRelaxationFactor;
    double gravitationalAcceleration;
    /// Vertical Y parameter.
    double verticalY;
    /// Pressure-correction iteration index.
    int pressureCorrectionIteration;
    /// Time used by the finite-volume, porous and conduction solvers.
    double solverTime;
    /// High-resolution convection scheme selector.
    int highResolutionScheme;
    /// Holdup-advection scheme selector: 0 for upwind, 1 for CICSAM.
    int holdupAdvectionScheme;
    /// Selects deferred rather than implicit high-resolution corrections.
    int deferredHighResolutionCorrection;
    int regularGridCellCountX;
    int regularGridCellCountY;
    int pressureVelocityCouplingMethod;
    double gravityAngleY;
    double gravityAngleX;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    /// Index of the cell with the largest centroid Y coordinate.
    int maxYCellIndex;
    /// Factor on the reference-density contribution to body forces.
    double bodyForceDensityFactor;
    /// Weight of mesh-distortion corrections in face interpolation.
    double meshDistortionCorrectionFactor;
    /// Residual aggregation selector: 0 for maximum, 1 for mean.
    int residualAggregationMode;
    double targetCflNumber;
    int hasPhaseInterface;
    /// Finite-volume restart request flag.
    int finiteVolumeRestartRequested;
    int volumeOfFluidEnabled;
    /// Selects the body-force correction used in pressure interpolation.
    int bodyForceCorrectionMode;
    int liquidPhaseIndex;
    int gasPhaseIndex;
    double centerX;
    double centerY;
    int finiteVolumeMaterialCount;
    int thermalCouplingEnabled;
    /// Angular parameter for model coupling.
    double couplingAngle;
    /// Wall-temperature parameter.
    double wallTemperature;
    double ambientTemperature;
    double ambientVelocity;
    int ambientModel;
    /// Production simulation time; synchronized with networkSimulationTime in network runs.
    double simulationTime;
    int timeStepIndex;
    /// Small-value tolerance used by numerical and phase-boundary checks.
    double smallValueTolerance;
    /// Void-fraction threshold used by condensation and mass-transfer controls.
    double condensationVoidFractionThreshold;
    /// Superficial-velocity threshold for restarting fraction evolution.
    double fractionRestartVelocityThreshold;
    /// Number of property root solves that exhausted their iteration limit.
    int propertyIterationLimitCount;
    /// Simulation time advanced by the network time-stepping loop.
    double networkSimulationTime;
    double networkFinalTime;
    double maximumGasOilRatio;
    /// Selects an isolated system or network execution.
    int networkMode;
    /// Network fluid-mode selector from the input configuration.
    int networkFluidMode;
    /// Gas-lift ring-network mode flag.
    int gasLiftRingNetworkEnabled;
    /// Parallel-branch network configuration flag.
    int parallelNetworkMode;
    int networkIteration;
    /// Network restart request flag.
    int networkRestartRequested;
    double networkTimeStep;
    double totalInletArea;
    /// Estimated total standard liquid flow used for initial branch-flow distribution.
    double estimatedStandardLiquidFlowRate;
    /// Estimated total standard gas flow used for initial branch-flow distribution.
    double estimatedStandardGasFlowRate;
    double networkRelaxationFactor;
    /// Enables transient network simulation.
    int networkTransientEnabled;
    int transientNetworkIteration;
    int networkIndex;
    int branchIndex;
    int branchCheckEnabled;
    /// Selects the network gas-liquid balance formulation.
    int networkFluidBalanceMode;
    /// Relative change of the network norm since its preceding evaluation.
    double networkNormRelativeChange;
    /// Network norm retained from its preceding evaluation.
    double previousNetworkNorm;
    /// Reference area used to scale network flow rates.
    double referenceFlowArea;
    /// General thread-count parameter.
    int threadCount;
    /// Thread count used within the simulation solvers.
    int solverThreadCount;
    /// Thread count used across internal networks.
    int networkThreadCount;
    /// Thread count used by the three-dimensional Poisson solver.
    int poisson3DThreadCount;
    /// Property-evaluation mode; disabled while generating fluid minitables.
    int transientPropertyMode;
    /// Indicates that the configured simulation is transient.
    int transientSimulationEnabled;
    /// Index of the active parametric-analysis case.
    int parametricCaseIndex;
    /// Number of branches represented by the context.
    int branchCount;
    /// Drift-flux model selector.
    int driftFluxModel;
    /// Selects the temporary Black-Oil formulation used by compositional network initialization.
    int temporaryBlackOilMode;
    /// Initializes the numerical controls and simulation state.
    SimulationContext() {
        finiteVolumeStartupMode = 0;
        finiteVolumeRelaxationFactor = 0.7;
        velocityCorrectionRelaxationFactor = 0.9;
        pressureCorrectionRelaxationFactor = 0.1;
        gravitationalAcceleration = 9.82;
        verticalY = 0;
        pressureCorrectionIteration = 0;
        solverTime = 0;
        highResolutionScheme = 0;
        holdupAdvectionScheme = 1;
        deferredHighResolutionCorrection = 1;
        regularGridCellCountX = 0;
        regularGridCellCountY = 0;
        pressureVelocityCouplingMethod = 0;
        gravityAngleY = 0.;
        gravityAngleX = 0.;
        xmin = 0;
        xmax = 0;
        ymin = 0;
        ymax = 0;
        maxYCellIndex = 0;
        bodyForceDensityFactor = 1.;
        meshDistortionCorrectionFactor = 0.;
        residualAggregationMode = 0;
        targetCflNumber = 0.5;
        hasPhaseInterface = 0;
        finiteVolumeRestartRequested = 0;
        volumeOfFluidEnabled = 1;
        bodyForceCorrectionMode = 1;
        liquidPhaseIndex = 0;
        gasPhaseIndex = 0;
        centerX = 0;
        centerY = 0;
        finiteVolumeMaterialCount = 0;
        thermalCouplingEnabled = 0;
        couplingAngle = 0;
        wallTemperature = 0;
        ambientTemperature = 0;
        ambientVelocity = 0;
        ambientModel = 0;
        simulationTime = 0;
        timeStepIndex = 0;
        smallValueTolerance = 1e-9;
        condensationVoidFractionThreshold = 1e-9;
        fractionRestartVelocityThreshold = 1e-5;
        propertyIterationLimitCount = 0;
        networkSimulationTime = 0;
        networkFinalTime = 0;
        maximumGasOilRatio = 9400000.;
        networkMode = 0;
        networkFluidMode = 1;
        gasLiftRingNetworkEnabled = 0;
        parallelNetworkMode = 0;
        networkIteration = 0;
        networkRestartRequested = 1;
        networkTimeStep = 0;
        totalInletArea = 0;
        estimatedStandardLiquidFlowRate = 0;
        estimatedStandardGasFlowRate = 0;
        networkRelaxationFactor = 0.5;
        networkTransientEnabled = 0;
        transientNetworkIteration = 0;
        networkIndex = -1;
        branchIndex = -1;
        branchCheckEnabled = 0;
        networkFluidBalanceMode = 0;
        networkNormRelativeChange = 1000;
        previousNetworkNorm = 10000;
        double MPI = 3.14159265359;
        referenceFlowArea = 0.25 * MPI * (4 * 4) * 2.54 * 2.54 / 10000;
        threadCount = 1;
        solverThreadCount = 1;
        networkThreadCount = 1;
        poisson3DThreadCount = 1;
        transientPropertyMode = 0;
        transientSimulationEnabled = 0;
        parametricCaseIndex = 0;
        branchCount = 1;
        driftFluxModel = 1;
        temporaryBlackOilMode = 0;
    }
    /// Assigns shared settings and runtime state from other.
    SimulationContext &operator=(SimulationContext &other) {
        if (this != &other) {
            finiteVolumeStartupMode = other.finiteVolumeStartupMode;
            finiteVolumeRelaxationFactor = other.finiteVolumeRelaxationFactor;
            velocityCorrectionRelaxationFactor = other.velocityCorrectionRelaxationFactor;
            pressureCorrectionRelaxationFactor = other.pressureCorrectionRelaxationFactor;
            gravitationalAcceleration = other.gravitationalAcceleration;
            verticalY = other.verticalY;
            pressureCorrectionIteration = other.pressureCorrectionIteration;
            solverTime = other.solverTime;
            highResolutionScheme = other.highResolutionScheme;
            holdupAdvectionScheme = other.holdupAdvectionScheme;
            deferredHighResolutionCorrection = other.deferredHighResolutionCorrection;
            regularGridCellCountX = other.regularGridCellCountX;
            regularGridCellCountY = other.regularGridCellCountY;
            pressureVelocityCouplingMethod = other.pressureVelocityCouplingMethod;
            gravityAngleY = other.gravityAngleY;
            gravityAngleX = other.gravityAngleX;
            xmin = other.xmin;
            xmax = other.xmax;
            ymin = other.ymin;
            ymax = other.ymax;
            maxYCellIndex = other.maxYCellIndex;
            bodyForceDensityFactor = other.bodyForceDensityFactor;
            meshDistortionCorrectionFactor = other.meshDistortionCorrectionFactor;
            residualAggregationMode = other.residualAggregationMode;
            targetCflNumber = other.targetCflNumber;
            hasPhaseInterface = other.hasPhaseInterface;
            finiteVolumeRestartRequested = other.finiteVolumeRestartRequested;
            volumeOfFluidEnabled = other.volumeOfFluidEnabled;
            bodyForceCorrectionMode = other.bodyForceCorrectionMode;
            liquidPhaseIndex = other.liquidPhaseIndex;
            gasPhaseIndex = other.gasPhaseIndex;
            centerX = other.centerX;
            centerY = other.centerY;
            finiteVolumeMaterialCount = other.finiteVolumeMaterialCount;
            thermalCouplingEnabled = other.thermalCouplingEnabled;
            couplingAngle = other.couplingAngle;
            wallTemperature = other.wallTemperature;
            ambientTemperature = other.ambientTemperature;
            ambientVelocity = other.ambientVelocity;
            ambientModel = other.ambientModel;
            simulationTime = other.simulationTime;
            timeStepIndex = other.timeStepIndex;
            smallValueTolerance = other.smallValueTolerance;
            condensationVoidFractionThreshold = other.condensationVoidFractionThreshold;
            fractionRestartVelocityThreshold = other.fractionRestartVelocityThreshold;
            propertyIterationLimitCount = other.propertyIterationLimitCount;
            networkSimulationTime = other.networkSimulationTime;
            networkFinalTime = other.networkFinalTime;
            maximumGasOilRatio = other.maximumGasOilRatio;
            networkMode = other.networkMode;
            networkFluidMode = other.networkFluidMode;
            gasLiftRingNetworkEnabled = other.gasLiftRingNetworkEnabled;
            parallelNetworkMode = other.parallelNetworkMode;
            networkIteration = other.networkIteration;
            networkRestartRequested = other.networkRestartRequested;
            networkTimeStep = other.networkTimeStep;
            totalInletArea = other.totalInletArea;
            estimatedStandardLiquidFlowRate = other.estimatedStandardLiquidFlowRate;
            estimatedStandardGasFlowRate = other.estimatedStandardGasFlowRate;
            networkRelaxationFactor = other.networkRelaxationFactor;
            networkTransientEnabled = other.networkTransientEnabled;
            transientNetworkIteration = other.networkIteration;
            networkIndex = other.networkIndex;
            branchIndex = other.branchIndex;
            branchCheckEnabled = other.branchCheckEnabled;
            networkFluidBalanceMode = other.networkFluidBalanceMode;
            networkNormRelativeChange = other.networkNormRelativeChange;
            previousNetworkNorm = other.previousNetworkNorm;
            referenceFlowArea = other.referenceFlowArea;
            threadCount = other.threadCount;
            solverThreadCount = other.solverThreadCount;
            networkThreadCount = other.networkThreadCount;
            poisson3DThreadCount = other.poisson3DThreadCount;
            transientPropertyMode = other.transientPropertyMode;
            transientSimulationEnabled = other.transientSimulationEnabled;
            parametricCaseIndex = other.parametricCaseIndex;
            branchCount = other.branchCount;
            driftFluxModel = other.driftFluxModel;
            temporaryBlackOilMode = other.temporaryBlackOilMode;
        }
        return *this;
    }
};

#endif /* SIMULATIONCONTEXT_H_ */
