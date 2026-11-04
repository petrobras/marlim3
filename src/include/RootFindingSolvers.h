#ifndef ROOTFINDINGSOLVERS_H_
#define ROOTFINDINGSOLVERS_H_

#include <math.h>

#include <concepts>
#include <type_traits>

namespace rootfinding {

/// Callable accepting a double position and returning a value convertible to double.
template <typename Function>
concept ObjectiveFunction =
    std::invocable<Function, double> &&
    std::convertible_to<std::invoke_result_t<Function, double>, double>;

/// Transforms a residual into a value convertible to double.
template <typename Function>
concept ResidualMonitor = ObjectiveFunction<Function>;

/// Applies a positive sign when signSource >= 0, otherwise a negative sign.
/// \param magnitude   Value whose absolute value is used.
/// \param signSource  Sign selector; +0.0 and -0.0 select the positive sign.
/// \return fabs(magnitude) with the selected sign.
[[nodiscard]] inline double SIGN(double magnitude, double signSource) {
    return (signSource >= 0 ? 1.0 : -1.0) * fabs(magnitude);
}

/// \param value  Value to inspect.
/// \return -1 when value <= 0, otherwise 1, including NaN.
[[nodiscard]] int sign(double value);

/// Reports iteration exhaustion through NumError, which logs the message and
/// terminates the application.
/// \param message  Text handed to NumError verbatim.
void reportIterationLimit(const char *message);

/// Bisects the interval using residual signs, for at most 100 iterations.
/// Stops when the half-width or residual magnitude is below 0.001.
/// \tparam Objective  Residual function; see the ObjectiveFunction concept.
/// \param bracketLow   First interval endpoint.
/// \param bracketHigh  Second interval endpoint.
/// \param objective    Residual callable, evaluated at bracketLow and each midpoint.
/// \return Stopping midpoint, or the last midpoint when the iteration limit is reached.
template <ObjectiveFunction Objective>
[[nodiscard]] double bisect(double bracketLow, double bracketHigh, Objective &&objective) {
    double lowValue = objective(bracketLow);
    double halfWidth = bracketHigh - bracketLow;
    double midpoint;
    int maximumIterations = 100;
    double intervalTolerance = 0.001;
    double valueTolerance = 0.001;
    double multFC = 0.5;

    for (int iteration = 1; iteration <= maximumIterations; iteration++) {
        halfWidth = bracketHigh - bracketLow;
        halfWidth *= 0.5;
        midpoint = bracketLow + halfWidth;
        double midpointValue = objective(midpoint);
        if (fabs(halfWidth) < intervalTolerance || fabs(midpointValue) < valueTolerance)
            return midpoint;
        ((lowValue > 0 && midpointValue < 0) || (lowValue < 0 && midpointValue > 0)) ? (bracketHigh = midpoint) : (bracketLow = midpoint, lowValue = midpointValue);
    }
    return midpoint;
}

/// Finds a root using Brent's method. Endpoint residuals with the same nonzero
/// sign select bisect. Iteration exhaustion calls reportIterationLimit.
/// \tparam Objective          Residual function; see the ObjectiveFunction concept.
/// \param bracketLow          Interval endpoint.
/// \param bracketHigh         Interval endpoint.
/// \param objective           Residual callable; may update caller state.
/// \param absoluteTolerance   Absolute component of the convergence tolerance.
/// \param relativeTolerance   Relative component of the convergence tolerance.
/// \param maximumIterations   Iteration budget for Brent's loop.
/// \return Root estimate, or 1e10 if an initial endpoint residual exceeds 1e9 in magnitude.
template <ObjectiveFunction Objective>
[[nodiscard]] double zbrent(double bracketLow, double bracketHigh, Objective &&objective, double absoluteTolerance, double relativeTolerance, int maximumIterations) {
    double relativePrecision = relativeTolerance;
    double previousEstimate = bracketLow;
    double currentEstimate = bracketHigh;
    double oppositeSignPoint = bracketHigh;
    double previousValue = objective(previousEstimate);
    double currentValue = objective(currentEstimate);
    if (fabs(previousValue) > 1e9 || fabs(currentValue) > 1e9)
        return 1e10;
    double previousStep = 0.;
    double step, oppositeSignValue, stepNumerator, stepDenominator, valueRatioOpposite, valueRatioPrevious, workingTolerance, halfBracketWidth;

    if ((previousValue > 0.0 && currentValue > 0.0) || (previousValue < 0.0 && currentValue < 0.0)) {
        double fallbackRoot;
        fallbackRoot = bisect(bracketLow, bracketHigh, objective);
        return fallbackRoot;
    } else {
        oppositeSignValue = currentValue;
        for (int iteration = 0; iteration < maximumIterations; iteration++) {
            if ((currentValue > 0.0 && oppositeSignValue > 0.0) || (currentValue < 0.0 && oppositeSignValue < 0.0)) {
                oppositeSignPoint = previousEstimate;
                oppositeSignValue = previousValue;
                previousStep = step = currentEstimate - previousEstimate;
            }
            if (fabs(oppositeSignValue) < fabs(currentValue)) {
                previousEstimate = currentEstimate;
                currentEstimate = oppositeSignPoint;
                oppositeSignPoint = previousEstimate;
                previousValue = currentValue;
                currentValue = oppositeSignValue;
                oppositeSignValue = previousValue;
            }
            workingTolerance = 2.0 * relativePrecision * fabs(currentEstimate) + 0.5 * absoluteTolerance;
            halfBracketWidth = 0.5 * (oppositeSignPoint - currentEstimate);
            if (fabs(halfBracketWidth) <= workingTolerance || currentValue == 0.0)
                return currentEstimate;
            if (fabs(previousStep) >= workingTolerance && fabs(previousValue) > fabs(currentValue)) {
                valueRatioPrevious = currentValue / previousValue;
                if (previousEstimate == oppositeSignPoint) {
                    stepNumerator = 2.0 * halfBracketWidth * valueRatioPrevious;
                    stepDenominator = 1.0 - valueRatioPrevious;
                } else {
                    stepDenominator = previousValue / oppositeSignValue;
                    valueRatioOpposite = currentValue / oppositeSignValue;
                    stepNumerator = valueRatioPrevious * (2.0 * halfBracketWidth * stepDenominator * (stepDenominator - valueRatioOpposite) - (currentEstimate - previousEstimate) * (valueRatioOpposite - 1.0));
                    stepDenominator = (stepDenominator - 1.0) * (valueRatioOpposite - 1.0) * (valueRatioPrevious - 1.0);
                }
                if (stepNumerator > 0.0)
                    stepDenominator = -stepDenominator;
                stepNumerator = fabs(stepNumerator);
                double interpolationLimit = 3.0 * halfBracketWidth * stepDenominator - fabs(workingTolerance * stepDenominator);
                double previousStepLimit = fabs(previousStep * stepDenominator);
                if (2.0 * stepNumerator < (interpolationLimit < previousStepLimit ? interpolationLimit : previousStepLimit)) {
                    previousStep = step;
                    step = stepNumerator / stepDenominator;
                } else {
                    step = halfBracketWidth;
                    previousStep = step;
                }
            } else {
                step = halfBracketWidth;
                previousStep = step;
            }
            previousEstimate = currentEstimate;
            previousValue = currentValue;
            if (fabs(step) > workingTolerance)
                currentEstimate += step;
            else
                currentEstimate += SIGN(workingTolerance, halfBracketWidth);
            currentValue = objective(currentEstimate);
        }
        reportIterationLimit("Metodo Van Winjngaarden-Dekker-Brent para calcular zero de funcaoo atingiu maximo de iteracoes");
        return 0.0;
    }
}

/// Finds a root using Ridders' method with bracket adjustment.
/// Endpoint and best-point residuals are evaluated directly; midpoint and trial
/// residuals pass through monitor. Callbacks may update caller state.
/// \tparam Objective          Residual function; see the ObjectiveFunction concept.
/// \tparam Monitor            Residual post-processing; see ResidualMonitor.
/// \param bracketLow          Endpoint where the residual is expected negative.
/// \param bracketHigh         Endpoint where the residual is expected positive.
/// \param objective           Residual callable.
/// \param monitor             Transforms midpoint and trial residuals.
/// \param reverseMarch        Not used by this solver.
/// \param minimumIterations   Three early returns require the zero-based iteration
///                            index to exceed this value; other exits are unaffected.
/// \return Best point found; 1e10 for an initial residual above 1e9 in magnitude or
///         iteration exhaustion, -1e10 when bracketing fails.
template <ObjectiveFunction Objective, ResidualMonitor Monitor>
[[nodiscard]] double zriddr(double bracketLow, double bracketHigh, Objective &&objective, Monitor &&monitor,
              int reverseMarch, int minimumIterations) {
    double rootAccuracy = 1e-5;
    int maximumIterations = 100;
    double bestValue;
    double bestPoint;
    double lowValue;
    double highValue;
    lowValue = objective(bracketLow);
    highValue = objective(bracketHigh);
    if (fabs(lowValue) > 1e9 || fabs(highValue) > 1e9)
        return 1e10;
    if (lowValue >= 0.) {
        if (lowValue > 0.9e10) {
            bracketLow *= 0.9999;
            lowValue = objective(bracketLow);
        } else {
            int attempts = 0;
            while (attempts < 100 && lowValue > 0.) {
                if (bracketHigh < bracketLow)
                    bracketLow *= 1.0001;
                else
                    bracketLow *= 0.999;
                lowValue = objective(bracketLow);
                attempts++;
            }
        }
    } else if (highValue <= 0.) {
        if (highValue < -0.9e10) {
            bracketHigh *= 1.00001;
            highValue = objective(bracketHigh);
        } else {
            int attempts = 0;
            while (attempts < 100 && highValue < 0.) {
                if (bracketLow < bracketHigh)
                    bracketHigh *= 1.0001;
                else
                    bracketHigh *= 0.999;
                highValue = objective(bracketHigh);
                attempts++;
            }
        }
    }
    if (fabs(highValue) < fabs(lowValue)) {
        bestValue = highValue;
        bestPoint = bracketHigh;
    } else {
        bestValue = lowValue;
        bestPoint = bracketLow;
    }
    if ((lowValue > 0.0 && highValue < 0.0) || (lowValue < 0.0 && highValue > 0.0)) {
        double intervalLow = bracketLow;
        double intervalHigh = bracketHigh;
        double previousAnswer = -1.e20;
        for (int iteration = 0; iteration < maximumIterations; iteration++) {
            double midpoint = 0.5 * (intervalLow + intervalHigh);
            double midpointValue = monitor(objective(midpoint));
            if (fabs(midpointValue) < fabs(bestValue)) {
                bestValue = midpointValue;
                bestPoint = midpoint;
            }
            double discriminant = sqrt(midpointValue * midpointValue - lowValue * highValue);
            if (discriminant == 0.0) {
                bestValue = objective(bestPoint);
                if(iteration>minimumIterations)return bestPoint;
                continue;
            }
            double nextPoint = midpoint + (midpoint - intervalLow) * ((lowValue >= highValue ? 1.0 : -1.0) * midpointValue / discriminant);
            if (fabs(nextPoint - previousAnswer) <= rootAccuracy) {
                bestValue = objective(bestPoint);
                if(iteration>minimumIterations)return bestPoint;
            }
            previousAnswer = nextPoint;
            double nextValue = monitor(objective(previousAnswer));
            if (fabs(nextValue) < fabs(bestValue)) {
                bestValue = nextValue;
                bestPoint = previousAnswer;
            }
            if (fabs(nextValue) <= rootAccuracy) {
                bestValue = objective(bestPoint);
                if(iteration>minimumIterations)return bestPoint;
            }
            if (SIGN(midpointValue, nextValue) != midpointValue) {
                intervalLow = midpoint;
                lowValue = midpointValue;
                intervalHigh = previousAnswer;
                highValue = nextValue;
            } else if (SIGN(lowValue, nextValue) != lowValue) {
                intervalHigh = previousAnswer;
                highValue = nextValue;
            } else if (SIGN(highValue, nextValue) != highValue) {
                intervalLow = previousAnswer;
                lowValue = nextValue;
            } else
                return -1.e10;
            if (fabs(intervalHigh - intervalLow) <= rootAccuracy) {
                bestValue = objective(bestPoint);
                return bestPoint;
            }
        }
        return 1.e10;
    } else {
        if (fabs(lowValue) <= rootAccuracy) {
            return bracketLow;
        }
        if (fabs(highValue) <= rootAccuracy) {
            return bracketHigh;
        }
        return -1e10;
    }
}

}  // namespace rootfinding

#endif  // ROOTFINDINGSOLVERS_H_
