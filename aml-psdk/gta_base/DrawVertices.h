#ifndef __AML_PSDK_GTADRAWVERTICES_H
#define __AML_PSDK_GTADRAWVERTICES_H

#include <aml-psdk/renderware/RwRGBA.h>

typedef u16 RwImVertexIndex;

struct RwIm2DVertex
{
    RwIm2DVertex(){}
    RwReal      x;              /**< Screen X */
    RwReal      y;              /**< Screen Y */
    RwReal      z;              /**< Screen Z */
    RwReal      rhw;            /**< Reciprocal of homogeneous W */

    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        RwRGBA      color;
        RwUInt32    emissiveColor;  /**< Vertex color */
    };

    RwReal      u;              /**< Texture coordinate U */
    RwReal      v;              /**< Texture coordinate V */
};

struct RwIm3DVertex
{
    RwIm3DVertex(){}
    RwReal      x;              /**< World X */
    RwReal      y;              /**< World Y */
    RwReal      z;              /**< World Z */
    RwV3d normal;

    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        RwRGBA      color;
        RwUInt32    emissiveColor;  /**< Vertex color */
    };

    RwReal      u;              /**< Texture coordinate U */
    RwReal      v;              /**< Texture coordinate V */
};

#endif // __AML_PSDK_GTADRAWVERTICES_H
