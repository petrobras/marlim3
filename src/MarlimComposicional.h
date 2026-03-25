/*
 * MarlimComposicional.h
 *
 *  Created on: May 10, 2019
 *      Author: rodrigogdt
 */

#ifndef MARLIMCOMPOSICIONAL_H_
#define MARLIMCOMPOSICIONAL_H_

// OBS: Implementação em Fortran! Este arquivo não possui um "cpp" correspondente!

typedef struct ZRhoAndDerivs {
    double dLiquidZ;
    double dLiquidZWithPeneloux;
    double dDerivLiquidZWithT;
    double dDerivLiquidRhoWithT;
    double dDerivLiquidZWithP;
    double dDerivLiquidRhoWithP;
    double dVaporZWithPeneloux;
    double dDerivVaporZWithT;
    double dDerivVaporRhoWithT;
    double dDerivVaporZWithP;
    double dDerivVaporRhoWithP;
    double dDerivLiquidZPenelouxWithT;
    double dDerivLiquidZPenelouxWithP;
    double dDerivVaporZPenelouxWithT;
    double dDerivVaporZPenelouxWithP;
} ZRhoAndDerivs;

typedef struct InputForViscosity {
	double oPVTSIMViscCSPTuningParam[4];
} InputForViscosity;

#ifdef __cplusplus
	extern "C"
#endif
void MarlimCompositionalProps(double dFlashPressure, double dFlashTemperature, int iNComp, double* oZ, double* oMW, double* oTc,
		double* oPc, double* oW, double* oKij, double* oLij, double* oPeneloux, int iIGCoefsSource, double* oCpIGCoefs,
		double* oHIGCoefs, double* oParachors, int iLiqModel, int iVapModel, int iLiqDensityCalculationMethod, int iVapDensityCalculationMethod,
		int iLiqHAndCpThermodynamicModel, int iVapHAndCpThermodynamicModel,
        double dGivenInitialBeta, double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition,
        double* dCalculatedBeta, double* oLiqCompositionAtPAndT, double* oVapCompositionAtPAndT,
		double* dVaporMassFraction, double* dBubbleP, double* dLiqDensity, double* dVapDensity, double* dLiqMassEnthalpy,
		double* dVapMassEnthalpy, double* dLiqMassCp, double* dVapMassCp, double* dSurfaceTension, double* dVaporZ, double* dLiqJouleThomsonCoef,
		double* dVapJouleThomsonCoef, ZRhoAndDerivs* oZRhoAndDerivs, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void ReadMixtureComponentsFromExternalFile(const char *sExternalFileName, int iExternalFileNameLength, int iExternalFileSource,
		int* iNComp, double* oZ, double* oMW, double* oTc, double* oPc, double* oW, double* oParachors, double* oKij, double* oLij,
		double* oTIndep_Peneloux, int* iThermodynamicModel, double* oCpIGCoefs, InputForViscosity* oInputForViscosity, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void ReadWaxCalculationParametersFromExternalFile(const char *sWaxExternalFileName, int iWaxExternalFileNameLength, int iWaxExternalFileSource,
	                int iWaxComponentCount, int iWaxPressureCount, int iWaxTemperatureCount, int* iIER, double* oWaxPressurePoints,
					double* oWaxCloudPointTemperatures, double* oWaxStructurePressures, double* oWaxStructureTemperatures, double* oWaxStructureDensities,
					double* oWaxStructureLiqMW, double* oStructureCPWax, double* oWaxStructureThermCond, double* oStructureWaxConcs,
					double* oMolecularWeightsOfWaxComponents, double* oLiquidDensitiesOfWaxComponents,
					double* dOilWaxViscosityMultiplierD, double* dOilWaxViscosityMultiplierE, double* dOilWaxViscosityMultiplierF);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation(double dPressure, double dTemperature, int iWaxComponentCount, int iPressureCount,
        int iTemperatureCount, double* oPressurePoints, double* oCloudPointTemperatures, double* oStructurePressures,
		double* oStructureTemperatures, double* oStructureDensities, double* oStructureLiqMW, double* oStructureCPWax,
		double* oStructureThermCond, double* oStructureWaxConcs, double* oMolecularWeightsOfWaxComponents,
		double* oLiquidDensitiesOfWaxComponents, int* iIER, int* iWaxComponentCount_output, double* oMolecularWeightsOfWaxComponents_output,
		double* oLiquidDensitiesOfWaxComponents_output, double* dCloudPointT_output, double* dInterpolatedCPWax_output,
		double* dInterpolatedThermCond_output, double* dInterpolatedDens_output, double* dInterpolatedLiqMW_output,
        double* oInterpolatedWaxConcsTDeriv_output, double* oInterpolatedWaxConcs_output);

#ifdef __cplusplus
	extern "C"
#endif
void GetWaxParameterExternalFileDimensions(const char *sExternalFileName, int iExternalFileNameLength, int iExternalFileSource,
		int* iIER, int* iWaxComponentCount, int* iPressureCount, int* iTemperatureCount);

#ifdef __cplusplus
	extern "C"
#endif
void GetMixtureComponentNumberFromExternalFile(const char *sExternalFileName, int iExternalFileNameLength, int iExternalFileSource,
		int* iNComp, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateMixtureThermodynamicCondition(double dFlashPressure, double dFlashTemperature, int iNComp, double* oMW, double* oZ, double* oTc,
		double* oPc, double* oW, double* oPeneloux, double* oKij, double* oLij, int iLiqPhaseModel, int iVapPhaseModel,
		double dGivenInitialBeta, double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition,
		double* dCalculatedBubbleT, int* iIER_BubbleT, double* dCalculatedDewT, int* iIER_DewT, double* dCalculatedBeta,
		double* oCalculatedLiqComposition, double* oCalculatedVapComposition, int* iIER_Flash, double* dCalculatedBubbleP,
		int* iIER_BubbleP, int* iCalculatedThermodynamicCondition, double* dLiquidPhaseMW, double* dVaporPhaseMW, double* dVaporMassFraction, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateMixtureSurfaceTension(double dPressure, double dTemperature, int iNComp, double* oTc, double* oPc, double* oW, double* oPeneloux,
		double* oKij, double* oLij, double* oParachors, double dLiquidPhaseMW, double dVaporPhaseMW,
        double* oLiqPhaseComposition, double* oVapPhaseComposition, int iLiqDensityCalculationMethod, int iVapDensityCalculationMethod,
        double* dSurfaceTension, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculatePhaseCpAndEnthalpy(double dPressure, double dTemperature, int iPhaseID, double* oPhaseComposition, int iNComp, double* oTc, double* oPc, double* oW, double* oPeneloux,
		double* oKij, double* oLij, int iThermodynamicModel, int iIGCoefsSource, double* oCpIGCoefs, double* oHIGCoefs, double dPhaseMW,
		double* dMassEnthalpy, double* dMassCp, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculatePhaseDensity(double dPressure, double dTemperature, int iPhaseID, double* oPhaseComposition, int iNComp, double* oTc, double* oPc,
        double* oW, double* oPeneloux, double* oKij, double* oLij, int iDensityCalculationMethod, double dPhaseMW, double* dPhaseDensity,
		double* dPhaseCompressibilityFactor, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(double dPressure, double dTemperature, int iThermodynamicModel, int iPhaseID, int iNComp,
		double* oPhaseComposition, double* oTc, double* oPc, double* oW, double* oKij, double* oLij, double* oPeneloux, double dPhaseMW,
		double* dPhasicZ, double* dDerivPhasicZWithT, double* dDerivPhasicRhoWithT, double* dDerivPhasicZWithP, double* dDerivPhasicRhoWithP,
		int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_AdjustGlobalCompositionForTargetGOR(int iNComp, double* oOriginalGlobalComposition, double* oMW, double* oTc, double* oPc,
		double* oW, double* oPeneloux, double* oKij, double* oLij, int iLiqPhaseModel, int iVapPhaseModel, int iLiqDensityCalculationMethod,
		int iVapDensityCalculationMethod, double dStockTankPressure, double dStockTankTemperature, double dTargetGOR, double dGivenInitialBeta,
		double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition, double* oGORAdjustedGlobalComp, double* dAdjustedGlobalCompGOR,
        int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions(int iNComp, double* oZ, double* oMW, double* oTc, double* oPc, double* oW,
		double* oPeneloux, double* oKij, double* oLij,
        int iLiqPhaseModel, int iVapPhaseModel, int iLiqDensityCalculationMethod, int iVapDensityCalculationMethod, double dStockTankPressure,
        double dStockTankTemperature,
		double dGivenInitialBeta, double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition,
		int* iCalculatedStockTankThermodynamicCondition, double* dStockTankVaporMassFraction,
		double* dStockTankLiquidDensity,
        double* dStockTankVaporDensity, double* dStockTankCompositionalGOR, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateSolutionGasOilRatio(double dVaporMassFraction, double dStockTankVaporMassFraction, double dStockTankLiquidDensity,
		double dStockTankVaporDensity, double* dCompositionalSolutionGasOilRatio);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateOilFormationVolumeFactor(double dPressure, double dTemperature, int iNComp, double* oTc, double* oPc, double* oW, double* oKij,
		double* oLij, double* oPeneloux, int iLiqDensityCalculationMethod, double* oLiqPhaseComposition, double dLiquidPhaseMW, double dVaporMassFraction,
		double dStockTankLiquidDensity, double dStockTankVaporMassFraction, double* dCompositionalOilFormationVolumeFactor, int* iIER);

#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateBubblePressure(double dTemperature, int iNComp, double* oZ, double* oTc, double* oPc, double* oW, double* oKij, double* oLij,
		double* oPeneloux, int iLiqPhaseModel, int iVapPhaseModel, double dPressure, double* dCalculatedBubbleP, int* iIER);


#ifdef __cplusplus
	extern "C"
#endif
void MarlimCalculateViscosityPedersen(double dPressure, double dTemperature, int iNComp, double* oTc, double* oPc, double* oMW,
        double* oComposition, double* dEta, int* iIER);

/*
#ifdef __cplusplus
	extern "C"
#endif
void Marlim_CalculateSingleComponentZAndRho(double dTc, double dPc, double dW, double dC, double dMW, int iPhaseID, double dFlashPressure,
				double dFlashTemperature, int iModel, double* dZ, double* dDensity, int* iIER);
*/


#ifdef __cplusplus
	extern "C"
#endif
void TestFlashExtras(int iNComp, int iPhaseID, double dTemperature, double dPressure, double* oPhasicComposition,
        int iPhasicThermodynamicModel, double* oTc, double* oPc, double* oW, double* oKij, double* oLij, double* oPeneloux,
		double* oZ, int iLiqPhaseModel, int iVapPhaseModel);

#endif /* MARLIMCOMPOSICIONAL_H_ */
