#ifndef RW_GEOMETRY_INDEXEDTRIMESH_HPP_
#define RW_GEOMETRY_INDEXEDTRIMESH_HPP_

#include <rw/math/Vector3D.hpp>
#include "TriMesh.hpp"
#include "Triangle.hpp"
#include "IndexedTriangle.hpp"

namespace rw {
namespace geometry {

    template <class T>
    class IndexedTriMesh: public TriMesh<T> {
    public:
        typedef T value_type;

        virtual const std::vector<rw::math::Vector3D<T> >& getVertices() const = 0;

        virtual std::vector<rw::math::Vector3D<T> >& getVertices() = 0;

        /**
         * @brief
         */
        virtual const rw::math::Vector3D<T>& getVertex(size_t i) = 0;

        virtual const rw::math::Vector3D<T>& getVertex(size_t i) = 0;

    };

	/**
	 *
	 * @brief an Indexed Triangle mesh
	 *
	 */
	template <class T=double, TriType TRI=N0>
	class IndexedTriMesh: public TriMesh<T> {
	public:
		typedef std::vector<rw::math::Vector3D<T> > VertexArray;
		typedef std::vector<IndexedTriangle> TriangleArray;

	private:
		typedef T primType;

		std::vector<IndexedTriangle > *_triangles;
		const std::vector<rw::math::Vector3D<T> > *_vertices;
		std::vector<rw::math::Vector3D<T> > *_faceNormals;
		std::vector<rw::math::Vector3D<T> > *_normals;

	public:

		typedef IndexedTriangle value_type;

		/**
		 * @brief constructor
		 */
		IndexedTriMesh():
			_triangles( new TriangleArray() ),
			_vertices( new VertexArray() ),
			_faceNormals( new VertexArray() )
		{};

		/**
		 * @brief constructor
		 */
		IndexedTriMesh(const std::vector<rw::math::Vector3D<T> > *vertices):
			_triangles( new TriangleArray() ),
			_vertices(vertices)
		{

		};

		/**
		 * @brief constructor
		 */
		IndexedTriMesh(VertexArray *vertices,TriangleArray *triangles):
           _triangles(triangles),
           _vertices(vertices)
		{

		};

		/**
		 * @brief destructor
		 */
		virtual ~IndexedTriMesh(){};

		/**
		 * @brief add indexed triangle to the triangle mesh.
		 */
		void add(const IndexedTriangle& triangle){
			_triangles->push_back(triangle);
		}

		/**
		 * @brief
		 */
		const rw::math::Vector3D<primType>& getVertex(size_t i){
			return (*_vertices)[i];
		}

		/**
		 * @brief get vertex i of triangle at index triIdx.
		 * @param i [in] should be in interval [0;2]
		 * @param triIdx [in] index of triangle in the triangle mesh
		 */
		const rw::math::Vector3D<primType>& getVertex(size_t i, size_t triIdx){
			const IndexedTriangle& tri = (*_triangles)[triIdx];
			return (*_vertices)[tri.getVertexIdx(i)];
		}

		/**
		 * @brief
		 */
		const rw::math::Vector3D<primType>& getVertexNormal(size_t i){
			return (*_normals)[i];
		}

		const rw::math::Vector3D<primType>& getFaceNormal(size_t i){
			return (*_faceNormals)[i];
		}

		const std::vector<rw::math::Vector3D<primType> >& getVertices() const {
			return *_vertices;
		}

		const std::vector<rw::math::Vector3D<primType> >& getFaceNormals(){
			return *_faceNormals;
		}

		const std::vector<rw::math::Vector3D<primType> >& getVertexNormals(){
			return *_normals;
		}

		TriangleArray& getTriangles(){
			return *_triangles;
		}

		const TriangleArray& getTriangles() const {
			return *_triangles;
		}

        /**
         * @brief get vertex at index i
         */
        IndexedTriangle& operator[](int i){
            return (*_triangles)[i];
        }

        const IndexedTriangle& operator[](int i) const {
            return (*_triangles)[i];
        }


		T calcFaceArea(size_t triIdx){
			using namespace rw::math;
			const IndexedTriangle& tri = (*_triangles)[triIdx];
			const Vector3D<T> &v0( (*_vertices)[tri.getVertexIdx(0) ] );
			const Vector3D<T> &v1( (*_vertices)[tri.getVertexIdx(1) ] );
			const Vector3D<T> &v2( (*_vertices)[tri.getVertexIdx(2) ] );

			return ( cross( v0-v1 , v0-v2 ) ).norm2()/2;
		}

		rw::math::Vector3D<T> calcFaceCentroid(size_t triIdx){
			using namespace rw::math;
			const IndexedTriangle& tri = (*_triangles)[triIdx];
			const Vector3D<T> &v0( (*_vertices)[tri.getVertexIdx(0) ] );
			const Vector3D<T> &v1( (*_vertices)[tri.getVertexIdx(1) ] );
			const Vector3D<T> &v2( (*_vertices)[tri.getVertexIdx(2) ] );

			return (v0+v1+v2)/3;
		}

		rw::math::Vector3D<T> calcFaceNormal(size_t triIdx) const {
			using namespace rw::math;
			const IndexedTriangle& tri = (*_triangles)[triIdx];
			const Vector3D<T> &v0( (*_vertices)[tri.getVertexIdx(0) ] );
			const Vector3D<T> &v1( (*_vertices)[tri.getVertexIdx(1) ] );
			const Vector3D<T> &v2( (*_vertices)[tri.getVertexIdx(2) ] );

			Vector3D<T> n = cross(v1-v0,v2-v0);
	        return normalize(n);
		}

//		Triangle<T,TRI> getTriangle(const IndexedTriangle& idxTri) const {
//
//		}

		// Inherited from TriMesh
		TriangleN0<T> getTriangle(size_t idx) const {
			using namespace rw::math;
			const IndexedTriangle& tri = (*_triangles)[idx];
			const Vector3D<T> &v0( (*_vertices)[tri.getVertexIdx(0) ] );
			const Vector3D<T> &v1( (*_vertices)[tri.getVertexIdx(1) ] );
			const Vector3D<T> &v2( (*_vertices)[tri.getVertexIdx(2) ] );
			TriangleN0<T> tri1 = TriangleN0<T>(v0,v1,v2);
			return tri1;
		}

		size_t getSize() const {
			return _triangles->size();
		}


	};

} // geometry
} // geometry

#endif /*INDEXEDTRIMESH_HPP_*/
