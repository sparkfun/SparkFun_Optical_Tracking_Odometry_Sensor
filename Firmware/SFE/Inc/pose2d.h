/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// Stuct to define a 2D pose, include x, y, and heading
typedef struct
{
    float x;
    float y;
    float h;
} pose2d;

// Helper function to transform a pose by another pose. This has the effect of
// expressing one pose in the coordinate frame of another pose
pose2d transformPose(pose2d pose, pose2d transform);

// Helper function to invert a pose. This has the effect of expressing the
// inverse of a pose in the coordinate frame of the original pose
pose2d invertPose(pose2d pose);
