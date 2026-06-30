/*
 * structUNV.h
 *
 * Created on: September 9, 2024
 * Author: Eduardo
 *
 * Data structures used to represent nodes, edges, triangular faces, and
 * tetrahedral elements imported from a UNV mesh.
 */

#ifndef ESTRUTURAUNV_H_
#define ESTRUTURAUNV_H_

#include <string.h>
#include <string>

using namespace std;

/*!
 * Store the Cartesian coordinates and classification metadata of a mesh node.
 */
struct cartesiano {
    int ind;         // Global node index.
    double x[3];     // Cartesian coordinates: x, y, and z.
    int tipo;        // Node or geometric classification.
    int vertice;     // Associated vertex index.
    string condCont; // Boundary-condition label.

    //! Initialize the node with default values.
    cartesiano() {
        ind = -1;
        for (int i = 0; i < 3; i++)
            x[i] = 0.;
        tipo = -1;
        vertice = -1;
        condCont = "lixo";
    }

    //! Copy constructor.
    cartesiano(const cartesiano &vtemp) {
        ind = vtemp.ind;
        for (int i = 0; i < 3; i++)
            x[i] = vtemp.x[i];
        tipo = vtemp.tipo;
        vertice = vtemp.vertice;
        condCont = vtemp.condCont;
    }

    //! Copy-assignment operator.
    cartesiano &operator=(cartesiano &vtemp) {
        if (this != &vtemp) {
            ind = vtemp.ind;
            for (int i = 0; i < 3; i++)
                x[i] = vtemp.x[i];
            tipo = vtemp.tipo;
            vertice = vtemp.vertice;
            condCont = vtemp.condCont;
        }
        return *this;
    }
};

/*!
 * Store the connectivity of an edge defined by two mesh nodes.
 */
struct cordaNo {
    int ind;   // Global edge index.
    int no[2]; // Global indices of the two edge nodes.

    //! Initialize the edge with default values.
    cordaNo() {
        ind = -1;
        for (int i = 0; i < 2; i++)
            no[i] = 0;
    }

    //! Copy constructor.
    cordaNo(const cordaNo &vtemp) {
        ind = vtemp.ind;
        for (int i = 0; i < 2; i++)
            no[i] = vtemp.no[i];
    }

    //! Copy-assignment operator.
    cordaNo &operator=(cordaNo &vtemp) {
        if (this != &vtemp) {
            ind = vtemp.ind;
            for (int i = 0; i < 2; i++)
                no[i] = vtemp.no[i];
        }
        return *this;
    }
};

/*!
 * Store the connectivity and boundary metadata of a triangular face.
 */
struct faceNo {
    int ind;         // Global face index.
    int no[3];       // Global indices of the three face nodes.
    int tipo;        // Face or boundary classification.
    string condCont; // Boundary-condition label.
    string nomeFace; // Face name or region label.

    //! Initialize the face with default values.
    faceNo() {
        ind = -1;
        tipo = 0;
        condCont = "interno";
        nomeFace = "vazio";
        for (int i = 0; i < 3; i++) {
            no[i] = 0;
        }
    }

    //! Copy constructor.
    faceNo(const faceNo &vtemp) {
        ind = vtemp.ind;
        tipo = vtemp.tipo;
        condCont = vtemp.condCont;
        nomeFace = vtemp.nomeFace;
        for (int i = 0; i < 3; i++) {
            no[i] = vtemp.no[i];
        }
    }

    //! Copy-assignment operator.
    faceNo &operator=(const faceNo &vtemp) {
        if (this != &vtemp) {
            ind = vtemp.ind;
            tipo = vtemp.tipo;
            condCont = vtemp.condCont;
            nomeFace = vtemp.nomeFace;
            for (int i = 0; i < 3; i++) {
                no[i] = vtemp.no[i];
            }
        }
        return *this;
    }

    //! Move-assignment operator.
    faceNo &operator=(faceNo &&vtemp) noexcept {
        if (this != &vtemp) {
            ind = vtemp.ind;
            tipo = vtemp.tipo;
            condCont = std::move(vtemp.condCont);
            nomeFace = std::move(vtemp.nomeFace);
            for (int i = 0; i < 3; i++) {
                no[i] = vtemp.no[i];
            }
        }
        return *this;
    }
};

/*!
 * Store the connectivity and region metadata of a tetrahedral element.
 *
 * The element is defined by four nodes and four triangular faces.
 */
struct eleNo {
    int ind;             // Global element index.
    int no[4];           // Global indices of the four element nodes.
    int indFace[4];      // Global indices of the four element faces.
    string nomeFace[4];  // Names or region labels of the element faces.
    int indFaceNo[4][3]; // Node indices associated with each triangular face.
    string regiao;       // Material or geometric region name.

    //! Initialize the tetrahedral element with default values.
    eleNo() {
        ind = -1;
        regiao = "unica";
        for (int i = 0; i < 4; i++) {
            no[i] = 0;
            indFace[i] = -1;
            nomeFace[i] = "vazio";
            for (int j = 0; j < 3; j++)
                indFaceNo[i][j] = -1;
        }
    }

    //! Copy constructor.
    eleNo(const eleNo &vtemp) {
        ind = vtemp.ind;
        regiao = vtemp.regiao;
        for (int i = 0; i < 4; i++) {
            no[i] = vtemp.no[i];
            indFace[i] = vtemp.indFace[i];
            nomeFace[i] = vtemp.nomeFace[i];
            for (int j = 0; j < 3; j++)
                indFaceNo[i][j] = vtemp.indFaceNo[i][j];
        }
    }

    //! Copy-assignment operator.
    eleNo &operator=(const eleNo &vtemp) {
        if (this != &vtemp) {
            ind = vtemp.ind;
            regiao = vtemp.regiao;
            for (int i = 0; i < 4; i++) {
                no[i] = vtemp.no[i];
                indFace[i] = vtemp.indFace[i];
                nomeFace[i] = vtemp.nomeFace[i];
                for (int j = 0; j < 3; j++)
                    indFaceNo[i][j] = vtemp.indFaceNo[i][j];
            }
        }
        return *this;
    }

    //! Move-assignment operator.
    eleNo &operator=(eleNo &&vtemp) noexcept {
        if (this != &vtemp) {
            ind = vtemp.ind;
            regiao = std::move(vtemp.regiao);
            for (int i = 0; i < 4; i++) {
                no[i] = vtemp.no[i];
                indFace[i] = vtemp.indFace[i];
                nomeFace[i] = std::move(vtemp.nomeFace[i]);
                for (int j = 0; j < 3; j++) {
                    indFaceNo[i][j] = vtemp.indFaceNo[i][j];
                }
            }
        }
        return *this;
    }
};

/*!
 * Store an edge together with the complete data of its two nodes.
 *
 * The structure owns the dynamically allocated node array.
 */
struct cordaOBJ {
    int ind;           // Global edge index.
    int no[2];         // Global indices of the edge nodes.
    cartesiano *objNo; // Complete data for the two edge nodes.

    //! Initialize the edge and allocate its node objects.
    cordaOBJ() {
        ind = -1;
        for (int i = 0; i < 2; i++) {
            no[i] = 0.;
        }
        objNo = new cartesiano[2];
    }

    //! Deep-copy constructor.
    cordaOBJ(const cordaOBJ &vtemp) {
        ind = vtemp.ind;
        objNo = new cartesiano[2];
        for (int i = 0; i < 2; i++) {
            no[i] = vtemp.no[i];
            objNo[i] = vtemp.objNo[i];
        }
    }

    //! Deep-copy assignment operator.
    cordaOBJ &operator=(cordaOBJ &vtemp) {
        if (this != &vtemp) {
            if (objNo != 0) {
                delete[] objNo;
            }
            ind = vtemp.ind;
            objNo = new cartesiano[2];
            for (int i = 0; i < 2; i++) {
                no[i] = vtemp.no[i];
                objNo[i] = vtemp.objNo[i];
            }
        }
        return *this;
    }

    //! Release the owned node array.
    ~cordaOBJ() {
        if (objNo != 0) {
            delete[] objNo;
        }
    }
};

/*!
 * Store a triangular face together with the complete data of its three nodes.
 *
 * The structure owns the dynamically allocated node array.
 */
struct faceOBJ {
    int ind;           // Global face index.
    int tipo;          // Face or boundary classification.
    string condCont;   // Boundary-condition label.
    int no[3];         // Global indices of the face nodes.
    cartesiano *objNo; // Complete data for the three face nodes.

    //! Initialize the face and allocate its node objects.
    faceOBJ() {
        ind = -1;
        tipo = 0;
        condCont = "interno";
        for (int i = 0; i < 3; i++) {
            no[i] = 0.;
        }
        objNo = new cartesiano[3];
    }

    //! Deep-copy constructor.
    faceOBJ(const faceOBJ &vtemp) {
        ind = vtemp.ind;
        tipo = vtemp.tipo;
        condCont = vtemp.condCont;
        objNo = new cartesiano[3];
        for (int i = 0; i < 3; i++) {
            no[i] = vtemp.no[i];
            objNo[i] = vtemp.objNo[i];
        }
    }

    //! Deep-copy assignment operator.
    faceOBJ &operator=(const faceOBJ &vtemp) {
        if (this != &vtemp) {
            if (objNo != 0)
                delete[] objNo;
            ind = vtemp.ind;
            tipo = vtemp.tipo;
            condCont = vtemp.condCont;
            objNo = new cartesiano[3];
            for (int i = 0; i < 3; i++) {
                no[i] = vtemp.no[i];
                objNo[i] = vtemp.objNo[i];
            }
        }
        return *this;
    }

    //! Release the owned node array.
    ~faceOBJ() {
        if (objNo != 0)
            delete[] objNo;
    }
};

/*!
 * Store a tetrahedral element together with its node and face objects.
 *
 * The structure owns arrays containing four node objects and four face
 * objects. It also stores neighboring-element indices, global connectivity,
 * boundary-face mappings, region metadata, and an element-specific
 * aspect-ratio or geometric quality value.
 */
struct eleOBJ {
    int ind;               // Global element index.
    int noGlobal[4];       // Global indices of the four element nodes.
    int indFaceGlobal[4];  // Global indices of the four element faces.
    int indViz[4];         // Indices of neighboring elements.
    int indFaceNo[4][3];   // Node indices associated with each face.
    int faceCCfaceReal[4]; // Mapping between boundary-condition faces and actual faces.
    double RAspec;         // Element aspect-ratio or geometric quality value.
    string nomeFace[4];    // Names or region labels of the element faces.
    faceNo *objFace;       // Complete data for the four element faces.
    cartesiano *objNo;     // Complete data for the four element nodes.
    string regiao;         // Material or geometric region name.

    //! Initialize the element and allocate its node and face objects.
    eleOBJ() {
        ind = -1;
        regiao = "unica";
        RAspec = 1;
        for (int i = 0; i < 4; i++) {
            noGlobal[i] = 0.;
            indFaceGlobal[i] = -1;
            indViz[i] = -1;
            faceCCfaceReal[i] = -1;
            nomeFace[i] = "vazio";
            for (int j = 0; j < 3; j++)
                indFaceNo[i][j] = -1;
        }
        objNo = new cartesiano[4];
        objFace = new faceNo[4];
    }

    //! Deep-copy constructor.
    eleOBJ(const eleOBJ &vtemp) {
        ind = vtemp.ind;
        objNo = new cartesiano[4];
        objFace = new faceNo[4];
        regiao = vtemp.regiao;
        RAspec = vtemp.RAspec;
        for (int i = 0; i < 4; i++) {
            noGlobal[i] = vtemp.noGlobal[i];
            indFaceGlobal[i] = vtemp.indFaceGlobal[i];
            objNo[i] = vtemp.objNo[i];
            objFace[i] = vtemp.objFace[i];
            indViz[i] = vtemp.indViz[i];
            faceCCfaceReal[i] = vtemp.faceCCfaceReal[i];
            nomeFace[i] = vtemp.nomeFace[i];
            for (int j = 0; j < 3; j++)
                indFaceNo[i][j] = vtemp.indFaceNo[i][j];
        }
    }

    //! Deep-copy assignment operator.
    eleOBJ &operator=(const eleOBJ &vtemp) {
        if (this != &vtemp) {
            if (objNo != 0)
                delete[] objNo;
            if (objFace != 0)
                delete[] objFace;
            ind = vtemp.ind;
            objNo = new cartesiano[4];
            objFace = new faceNo[4];
            regiao = vtemp.regiao;
            RAspec = vtemp.RAspec;
            for (int i = 0; i < 4; i++) {
                noGlobal[i] = vtemp.noGlobal[i];
                indFaceGlobal[i] = vtemp.indFaceGlobal[i];
                objNo[i] = vtemp.objNo[i];
                objFace[i] = vtemp.objFace[i];
                indViz[i] = vtemp.indViz[i];
                faceCCfaceReal[i] = vtemp.faceCCfaceReal[i];
                nomeFace[i] = vtemp.nomeFace[i];
                for (int j = 0; j < 3; j++)
                    indFaceNo[i][j] = vtemp.indFaceNo[i][j];
            }
        }
        return *this;
    }

    //! Release the owned node and face arrays.
    ~eleOBJ() {
        if (objNo != 0)
            delete[] objNo;
        if (objFace != 0)
            delete[] objFace;
    }
};

#endif /* ESTRUTURAUNV_H_ */