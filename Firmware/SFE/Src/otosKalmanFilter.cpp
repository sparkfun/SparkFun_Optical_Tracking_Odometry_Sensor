/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "otosKalmanFilter.h"

otosKalmanFilter::otosKalmanFilter(float q00, float q11, float q22)
{
    // Initialize all matrices to zero
    memset(xData, 0, sizeof(xData));
    memset(pData, 0, sizeof(pData));
    memset(_fData, 0, sizeof(_fData));
    memset(_qData, 0, sizeof(_qData));

    // Set known state transition values
    _fData[kOtosKfIdx00] = 1.0f;
    _fData[kOtosKfIdx11] = 1.0f;
    _fData[kOtosKfIdx22] = 1.0f;

    // Set provided process noise covariance values
    _qData[kOtosKfIdx00] = q00;
    _qData[kOtosKfIdx11] = q11;
    _qData[kOtosKfIdx22] = q22;
}

void otosKalmanFilter::reset()
{
    // Reset state vector and covariance matrix to zero
    memset(xData, 0, sizeof(xData));
    memset(pData, 0, sizeof(pData));
}

void otosKalmanFilter::predict(float dt)
{
    // Update dt in the process model matrix
    _fData[kOtosKfIdx01] = dt;
    _fData[kOtosKfIdx12] = dt;

    ////////////////////////////////////////////////////////////////////////////
    // Prediction update of the state vector, following the equation:
    // X = F*X
    ////////////////////////////////////////////////////////////////////////////

    // Rather than using the matrix multiplication function, we can just use the
    // physics equations directly for simplicity, which is just:
    // pos += vel*dt
    // vel += acc*dt
    xData[0] += xData[1] * dt;
    xData[1] += xData[2] * dt;

    ////////////////////////////////////////////////////////////////////////////
    // Prediction update of the error covariance, following the equation:
    // P = F*P*F' + Q
    ////////////////////////////////////////////////////////////////////////////

    // Computing F*P*F' results in a lot of wasted computation, since F is
    // basically an identity matrix with a couple dt terms. We can optimizae
    // this by evaluating the equation directly per element. Below is F*P*F':
    //
    // [1 dt  0]   [P00 P01 P02]   [ 1  0 0]
    // [0  1 dt] * [P10 P11 P12] * [dt  1 0]
    // [0  0  1]   [P20 P21 P22]   [ 0 dt 1]
    //
    // Which expands to:
    //
    // [P00+(P01+P10)*dt+P11*dt^2 P01+(P02+P11)*dt+P12*dt^2 P02+P12*dt]
    // [P10+(P11+P20)*dt+P21*dt^2 P11+(P12+P21)*dt+P22*dt^2 P12+P22*dt]
    // [               P20+P21*dt                P21+P22*dt        P22]
    //
    // We can make an approximation here and ignore the dt^2 terms, since they
    // will be very small. This simplifies to:
    //
    // [P00+(P01+P10)*dt P01+(P02+P11)*dt P02+P12*dt]
    // [P10+(P11+P20)*dt P11+(P12+P21)*dt P12+P22*dt]
    // [      P20+P21*dt       P21+P22*dt        P22]
    //
    // Simulation also showed that the dt terms are small and can be ignored in
    // some situations, but omitting them can result in degraded estimation in
    // some edge cases that may be important.
    //
    // Now we can compute each element, also adding the Q matrix:
    pData[kOtosKfIdx00] += (pData[kOtosKfIdx01] + pData[kOtosKfIdx10]) * dt + _qData[kOtosKfIdx00];
    pData[kOtosKfIdx01] += (pData[kOtosKfIdx02] + pData[kOtosKfIdx11]) * dt;
    pData[kOtosKfIdx02] += pData[kOtosKfIdx12] * dt;
    pData[kOtosKfIdx10] += (pData[kOtosKfIdx11] + pData[kOtosKfIdx20]) * dt;
    pData[kOtosKfIdx11] += (pData[kOtosKfIdx12] + pData[kOtosKfIdx21]) * dt + _qData[kOtosKfIdx11];
    pData[kOtosKfIdx12] += pData[kOtosKfIdx22] * dt;
    pData[kOtosKfIdx20] += pData[kOtosKfIdx21] * dt;
    pData[kOtosKfIdx21] += pData[kOtosKfIdx22] * dt;
    pData[kOtosKfIdx22] += _qData[kOtosKfIdx22];
}

void otosKalmanFilter::updateVel(float vel, float velVar)
{
    update(vel, velVar, 1);
}

void otosKalmanFilter::updateAcc(float acc, float accVar)
{
    update(acc, accVar, 2);
}

void otosKalmanFilter::update(float measurement, float variance, uint8_t idx)
{
    ////////////////////////////////////////////////////////////////////////////
    // Innovation, following the equation:
    // y = z - H*X
    ////////////////////////////////////////////////////////////////////////////

    // No need to use matrices here, as the measurement z is a scalar. H is a
    // 1x3 matrix with a 1 in the idx-th position (eg. H = [0 1 0] for idx=1),
    // so H*X is just the idx-th element of X. So the equation simplifies to:
    float y = measurement - xData[idx];

    ////////////////////////////////////////////////////////////////////////////
    // Innovation covariance, following the equation:
    // S = H*P*H' + R
    ////////////////////////////////////////////////////////////////////////////

    // Because H is a 1x3 matrix with a 1 in the idx-th position, H*P*H' is just
    // the idx-th row and column of P. And we only need the inverse of S, so we
    // can simplify the equation to:
    float sInv = 1.0f / (pData[kOtosKfSize * idx + idx] + variance);

    ////////////////////////////////////////////////////////////////////////////
    // Kalman gain, following the equation:
    // K = P*H'*S^-1
    ////////////////////////////////////////////////////////////////////////////

    // Create K matrix
    float kData[kOtosKfSize];

    // Some more simplifications here, P*H' is just the idx-th column of P. And
    // S^-1 is a scalar that we just computed, so:
    kData[0] = pData[kOtosKfIdx00 + idx] * sInv;
    kData[1] = pData[kOtosKfIdx10 + idx] * sInv;
    kData[2] = pData[kOtosKfIdx20 + idx] * sInv;

    ////////////////////////////////////////////////////////////////////////////
    // State update, following the equation:
    // X = X + K*y
    ////////////////////////////////////////////////////////////////////////////

    // Just follow the equation directly per element, no need for matrices here:
    xData[0] += kData[0] * y;
    xData[1] += kData[1] * y;
    xData[2] += kData[2] * y;

    ////////////////////////////////////////////////////////////////////////////
    // Error covariance update, following the equation:
    // P = (I-K*H)*P
    ////////////////////////////////////////////////////////////////////////////

    // Computing (I-K*H)*P results in a lot of wasted computation, since (I-K*H)
    // is basically an identity matrix with a couple small terms. We can
    // optimize this by evaluating the equation directly per element. Below is
    // (I-K*H)*P:
    //
    // ([1 0 0] - [K0] [H0 H1 H2])   [P00 P01 P02]
    // ([0 1 0]   [K1]           ) * [P10 P11 P12]
    // ([0 0 1]   [K2]           )   [P20 P21 P22]
    //
    // We only expect H to be [0 1 0] (when idx=1) or [0 0 1] (when idx=2). In
    // the case that idx=1, the equation above expands to:
    //
    // [P00-P10*K0 P01-P11*K0 P02-P12*K0]
    // [P10-P10*K1 P11-P11*K1 P12-P12*K1]
    // [P20-P10*K2 P21-P11*K2 P22-P12*K2]
    //
    // And in the case that idx=2, the equation above expands to:
    //
    // [P00-P20*K0 P01-P21*K0 P02-P22*K0]
    // [P10-P20*K1 P11-P21*K1 P12-P22*K1]
    // [P20-P20*K2 P21-P21*K2 P22-P22*K2]
    //
    // There is a pattern here: each element is modified as follows:
    //
    // P_i,j -= P_idx,j * K_i
    //
    // So we can compute each element directly with a loop:
    for (int i = 0; i < kOtosKfSize; i++)
    {
        for (int j = 0; j < kOtosKfSize; j++)
        {
            pData[i * kOtosKfSize + j] -= pData[idx * kOtosKfSize + j] * kData[i];
        }
    }
}
