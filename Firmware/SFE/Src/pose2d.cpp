/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "pose2d.h"
#include "otosConstants.h"
#include "arm_math.h"

pose2d transformPose(pose2d t12, pose2d t23)
{
    // We're going to compute this transformation as if the provided poses are
    // homogeneous transformation matrices. For the 2D case, the transformation
    // matrix is a 3x3 matrix T that looks like this:
    //
    //             [R11 R12 x]   [cos(h) -sin(h) x]
    // T = [R d] = [R21 R22 y] = [sin(h)  cos(h) y]
    //     [0 1]   [0   0   1]   [0       0      1]
    //
    // Where h is the heading and (x, y) is the translation from one frame to
    // another. If we know the transformation from frame 1 to frame 2 (T12), and
    // from frame 2 to frame 3 (T23), we can compute the transformation from
    // frame 1 to frame 3 (T13) by multiplying the transformation matrices:
    //
    // T13 = T12 * T23
    //
    // Which expands to:
    //
    // [cos(h13) -sin(h13) x13]   [cos(h12) -sin(h12) x12]   [cos(h23) -sin(h23) x23]
    // [sin(h13)  cos(h13) y13] = [sin(h12)  cos(h12) y12] * [sin(h23)  cos(h23) y23]
    // [0         0        1  ]   [0         0        1  ]   [0         0        1  ]
    //
    // For the x13 and y13 components, this expands to:
    //
    // x13 = x23 * cos(h12) - y23 * sin(h12) + x12
    // y13 = x23 * sin(h12) + y23 * cos(h12) + y12
    //
    // The upper left 2x2 corner is the resulting rotation matrix, where each
    // component depends only on h13. The expansion for each element is one of:
    //
    // cos(h13) = cos(h12) * cos(h23) - sin(h12) * sin(h23)
    // sin(h13) = sin(h12) * cos(h23) + cos(h12) * sin(h23)
    //
    // Instead of computing the inverse trigonometric functions, we can notice
    // that these are just the angle addition identities, so we can simplify by
    // just adding the angles:
    //
    // h1 = h2 + h3

    // Create struct to hold output
    pose2d t13;

    // Pre-compute sin and cos
    float cosh12 = arm_cos_f32(t12.h);
    float sinh12 = arm_sin_f32(t12.h);

    // Compute transformation following equations above
    t13.x = t23.x * cosh12 - t23.y * sinh12 + t12.x;
    t13.y = t23.x * sinh12 + t23.y * cosh12 + t12.y;
    t13.h = t23.h + t12.h;

    // Wrap heading to +/- pi
    if (t13.h >= kPi)
        t13.h -= k2Pi;
    if (t13.h < -kPi)
        t13.h += k2Pi;

    // Done!
    return t13;
}

pose2d invertPose(pose2d pose)
{
    // Similar to transformPose(), we're going to compute this transformation as
    // if the provided pose is a homogeneous transformation matrix T. The
    // inverse of T is given by:
    //
    //                               [ cos(h) sin(h) -x*cos(h)-y*sin(h)]
    // T^-1 = [R' d'] = [RT -RT*d] = [-sin(h) cos(h)  x*sin(h)-y*cos(h)]
    //        [0  1 ]   [0   1   ]   [0       0       1                ]
    //
    // Where R' and d' are the inverse rotation matrix and position vector, and
    // RT is the transpose of the rotation matrix. Solving for the inverse
    // position vector, we get:
    //
    // x' = -x * cos(h) - y * sin(h)
    // y' =  x * sin(h) - y * cos(h)
    //
    // The expansion for the inverse rotation matrix elements are one of:
    //
    // cos(h') =  cos(h)
    // sin(h') = -sin(h)
    //
    // Instead of computing the inverse trigonometric functions, we can notice
    // that h' and h must be opposite angles, so we can simplify:
    //
    // h' = -h

    // Create struct to hold output
    pose2d poseInv;

    // Pre-compute sin and cos
    float cosh = arm_cos_f32(pose.h);
    float sinh = arm_sin_f32(pose.h);

    // Compute inverse following equations above
    poseInv.x = -pose.x * cosh - pose.y * sinh;
    poseInv.y = pose.x * sinh - pose.y * cosh;
    poseInv.h = -pose.h;

    // Done!
    return poseInv;
}
