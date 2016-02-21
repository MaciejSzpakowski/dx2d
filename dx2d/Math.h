#pragma once

namespace Viva {
    namespace Math
    {
        struct Vector2
        {
            float x, y;
        };

        struct Vector3
        {
            float x, y, z;
        };

        struct Vector4
        {
            float x, y, z, w;
        };

        struct Matrix3x3
        {
            float f[3][3];
        };

        struct Matrix4x4
        {
            float f[4][4];
        };
    }
}