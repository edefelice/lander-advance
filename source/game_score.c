#include "game_score.h"
#include "fixedpoint32.h"
#include "physics_constants.h"


/*
    game_score.c

    This module calculates the final score and result of the game.

    Responsibilities:

    - Fuel score calculation
    - Distance score calculation
    - Landing quality score calculation
    - Battery score calculation
    - Landing pad score calculation
    - Final GameResult generation

    Score calculations use Q16.16 fixed-point arithmetic.
*/


// Calculate the score based on the remaining propellant.
// More remaining propellant results in a higher score.
// Score is calculated linearly, up to SCORE_FUEL.
static int CalculateFuelScore(const Lander *lander)
{
    fixed fuel_ratio;

    fuel_ratio = fixDiv(lander->propellant, PROP_MASS);

    return (int)fixMul(FIX_FROM_INT(SCORE_FUEL), fuel_ratio);
}


// Calculate the score based on the distance from the center of the map.
// Greater distance from the center results in a higher score.
// The distance is normalized using MAX_MAP_DISTANCE.
static int CalculateDistanceScore(const Lander *lander)
{
    fixed distance;
    fixed ratio;

    distance = fixAbs(lander->x);

    if (fixAbs(lander->y) > distance)
        distance = fixAbs(lander->y);

    ratio = fixDiv(distance, MAX_MAP_DISTANCE);

    if (ratio > FIX_FROM_INT(1))
        ratio = FIX_FROM_INT(1);

    return (int)fixMul(FIX_FROM_INT(SCORE_DISTANCE), ratio);
}


/*
    Calculate the touchdown quality score.

    The score rewards low velocities at touchdown:
    - Vertical velocity: 50%
    - Horizontal velocity: 30%
    - Angular velocity: 20%

    Each component is normalized between 0 and 1 and then cubed to strongly reward very soft landings.
*/
static int CalculateLandingScore(const Lander *lander)
{
    fixed vertical_ratio;
    fixed horizontal_ratio;
    fixed angular_ratio;

    fixed quality;

    vertical_ratio = fixDiv(fixAbs(lander->touchdown_vz), MAX_LANDING_VZ);

    horizontal_ratio = fixDiv(fixAbs(lander->touchdown_vx) + fixAbs(lander->touchdown_vy), MAX_LANDING_VX + MAX_LANDING_VY);    //is arcade

    angular_ratio = fixDiv(fixAbs(lander->touchdown_omega), MAX_LANDING_OMEGA);

    if (vertical_ratio > FIX_FROM_INT(1))
        vertical_ratio = FIX_FROM_INT(1);

    if (horizontal_ratio > FIX_FROM_INT(1))
        horizontal_ratio = FIX_FROM_INT(1);

    if (angular_ratio > FIX_FROM_INT(1))
        angular_ratio = FIX_FROM_INT(1);

    //Convertion 0 = perfect touchdown and 1 = worst touchdown

    vertical_ratio = FIX_FROM_INT(1) - vertical_ratio;

    horizontal_ratio = FIX_FROM_INT(1) - horizontal_ratio;

    angular_ratio = FIX_FROM_INT(1) - angular_ratio;

    //cubic curve

    vertical_ratio = fixMul(fixMul(vertical_ratio, vertical_ratio), vertical_ratio);

    horizontal_ratio = fixMul(fixMul(horizontal_ratio, horizontal_ratio), horizontal_ratio);

    angular_ratio = fixMul(fixMul(angular_ratio, angular_ratio), angular_ratio);

    //Landing quality calculation

    quality = fixMul(vertical_ratio, FIX_FROM_FRACTION(1, 2)) + fixMul(horizontal_ratio, FIX_FROM_FRACTION(3, 10)) + fixMul(angular_ratio, FIX_FROM_FRACTION(1, 5));

    return (int)fixMul(FIX_FROM_INT(SCORE_LANDING), quality);
}


// Calculate the score based on remaining battery power.
// More remaining power results in a higher score.
// Score is calculated linearly, up to SCORE_BATTERY.
static int CalculateBatteryScore(const Lander *lander)
{
    fixed battery_ratio;

    battery_ratio = fixDiv(FIX_FROM_INT(lander->available_power), FIX_FROM_INT(P_USES));

    return (int)fixMul(FIX_FROM_INT(SCORE_BATTERY), battery_ratio);
}


//----------------- PIERLU COMMENTA TU

//static int CalculatePadScore(const Lander *lander)
//{
    /*
    --------------------------------------------

PIER VEDI TU 

    Return value:
        0    = edge of pad
        1000 = exact center
    */

//    return SCORE_PAD;
//}



/*
    Create the final GameResult.

    A crashed lander receives a loss result and no score.
    A successful landing receives a score calculated from fuel, distance, touchdown quality, battery and pad position.
*/
GameResult GameScoreCreateResult(const Lander *lander)
{
    GameResult result;

    if (lander->state == LANDER_CRASHED)
    {
        result.outcome = GR_LOSE;
        result.reason = lander->crash_reason;
        result.score = 0;

        return result;
    }

    result.outcome = GR_WIN;
    result.reason = GR_REASON_NONE;

    result.score = 0;

    result.score += CalculateFuelScore(lander);
    result.score += CalculateDistanceScore(lander);
    result.score += CalculateLandingScore(lander);
    result.score += CalculateBatteryScore(lander);
    //result.score += CalculatePadScore(lander);

    if (result.score > SCORE_MAX)
        result.score = SCORE_MAX;

    return result;
}