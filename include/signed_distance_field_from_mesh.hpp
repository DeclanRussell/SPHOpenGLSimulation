// Copyright © 2011 Mathieu Sanchez
//
// This file is part of sdflib.
// sdflib is a compiled software libray that is free to have and use under the terms of the Non-commerical, Non-open, Common Good Public License 1.0 as published by CGPl.org.
// sdflib is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// You should have received a copy of the Non-commerical, Non-open, Common Good Public License 1.0 along with sdflib. If not please visit CGPL.org.

#ifndef SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED
#define SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED

#include <string>
#include <memory>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class signed_distance_field_from_mesh "include/sdf/signed_distance_field_from_mesh.hpp"
	/// @brief This should be the main class users should use. 
	///			Allows users to query the exact signed distance to the mesh from any point in space.
	///			The sign is negative if it is inside the mesh, and positive outside.
	///			The absolute value of the SDF is the distance to the mesh boundary
	///			SDFs are very sensitive to mesh defects
	///			 - Duplicated vertices dont affect results
	///			 - Flipped faces damage the field. The voronoi region of the flipped face (normal) will swap the signs
	///			 - Self intersection are not handled and often produce discontinuities around
	///			 - Holes create a discontinuity where the field could jump from a negative to a positive value without going through 0
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 10/01/11 Untemplated version for the MSc. class. Interface simplified to a single SDF query
	//----------------------------------------------------------------------------------------------------------------------
	class signed_distance_field_from_mesh
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor: none of the data will be prepared (good for lazy init)
		/// @warning This constructor does NOTHING, one of the initialize methods has to be called before sampling
		//----------------------------------------------------------------------------------------------------------------------
		signed_distance_field_from_mesh();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor - from an obj file
		/// @param[in] i_filename Path (relative or absolute) to the obj file
		/// @warning This constructor calls the initialize method, there is no need to call it afterwards
		//----------------------------------------------------------------------------------------------------------------------
		signed_distance_field_from_mesh(const std::string& i_filename);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor through vertices (vertices through 3 floats per vertex) and indices (uint)
		/// @param[in] i_indices Index array, starting from 0 (first vertex is indexed to 0)
		/// @param[in] i_nindices Number of indices (number of indices = number of triangles * 3)
		/// @param[in] i_vertices i_vertices An array of floats, where each position is made of 3 floats. It points to the first float containing the position
		/// @param[in] i_nvertices Number of vertices in the array
		/// @param[in] i_vertexstride The gap in bytes between two vertex positions. 
		///								For instance, if the input is made of ngl::Vector, use sizeof(ngl::Vector).
		///								If it is a larger vertex buffer (containing interlaced positions, normals and uvs) use sizeof(VertexStruct)
		/// @warning This constructor calls the initialize method, there is no need to call it afterwards
		//----------------------------------------------------------------------------------------------------------------------
		signed_distance_field_from_mesh(
			const unsigned int* i_indices, const unsigned int i_nindices,
			const float* i_vertices, const unsigned int i_nvertices, const unsigned int i_vertexstride = sizeof(float)*3);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Destructor
		//----------------------------------------------------------------------------------------------------------------------
		~signed_distance_field_from_mesh();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the distance field - It must be called before sampling. Parameters for sign and distance computations are guessed
		/// @param[in] i_filename The path to an obj file.
		/// @return If the mesh was valid and all the preprocessing work was succesful, return true, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool load_from_file(const std::string& i_filename);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the distance field - It must be called before sampling. Parameters for sign and distance computations are guessed
		/// @param[in] i_indices Index array, starting from 0 (first vertex is indexed to 0)
		/// @param[in] i_nindices Number of indices (number of indices = number of triangles * 3)
		/// @param[in] i_vertices i_vertices An array of floats, where each position is made of 3 floats. It points to the first float containing the position
		/// @param[in] i_nvertices Number of vertices in the array
		/// @param[in] i_vertexstride The gap in bytes between two vertex positions. 
		///								For instance, if the input is made of ngl::Vector, use sizeof(ngl::Vector).
		///								If it is a larger vertex buffer (containing interlaced positions, normals and uvs) use sizeof(VertexStruct)
		/// @return If the mesh was valid and all the preprocessing work was succesful, return true, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool load_from_data(
			const unsigned int* i_indices, const unsigned int i_nindices,
			const float* i_vertices, const unsigned int i_nvertices, const unsigned int i_vertexstride = sizeof(float)*3);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the distance field - It must be called before sampling. Parameters for sign and distance computations are guessed
		/// @return True if the structure was correctly initialized, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool is_valid() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from position (x,y,z) to the mesh
		/// @param[in] x point.x
		/// @param[in] y point.y
		/// @param[in] z point.z
		/// @param[out] o_closest (optional) the closest point on the surface from (x,y,z)
		/// @return Signed distance to the mesh
		/// @usage ngl::Vector pos(1.f,1.3f,-2.f),closestPoint;
		///			const float value = sdf(pos.m_x,pos.m_y,pos.m_z,closestPoint.openGL());
		/// @warning If the structure has not been initialized, in debug it will stop (assert), in release it will return the maximum float value
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(float x, float y, float z, float* o_closest = 0) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from position (x,y,z) to the mesh with its analytical gradient if the point is within a range of the object
		/// @param[in] x point.x
		/// @param[in] y point.y
		/// @param[in] z point.z
		/// @param[in] i_maxrad the maximum radius to be considered
		/// @param[out] o_sdf The signed distance of the point (x,y,z) to the mesh. signed distance value will only be changed if the distance to the mesh is less than i_maxrad
		/// @param[out] o_closest (optional)  the closest point on the surface from (x,y,z)
		/// @return True if it is closer to a boundary than i_maxrad, false otherwise
		/// @warning If the structure has not been initialized, in debug it will stop (assert), in release it will return false 
		//----------------------------------------------------------------------------------------------------------------------
		bool operator()(
			float x, float y, float z, float i_maxrad, 
			float* o_sdf, float* o_closest = 0) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the min bound of the mesh
		/// @return An array of 3 floats containing the lowest values in X, Y and Z of the mesh's bounding box
		/// @warning If the structure has not been initialized, in debug it will stop (assert), in release it will return the positive maximum float value (to create an invalid BBox)
		//----------------------------------------------------------------------------------------------------------------------
		const float* minimum_bound() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the max bound of the mesh
		/// @return An array of 3 floats containing the highest values in X, Y and Z of the mesh's bounding box
		/// @warning If the structure has not been initialized, in debug it will stop (assert), in release it will return the negative maximum float value (to create an invalid BBox)
		//----------------------------------------------------------------------------------------------------------------------
		const float* maximum_bound() const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The following operators are disallowed (private, not implemented)
		/// Copies of this object are either non-sense, or incredibly wasteful
		//----------------------------------------------------------------------------------------------------------------------
		signed_distance_field_from_mesh(const signed_distance_field_from_mesh& i_copy);
		signed_distance_field_from_mesh& operator=(const signed_distance_field_from_mesh& i_copy);
	private :
		struct pimpl;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Private implementation :x
		//----------------------------------------------------------------------------------------------------------------------
		pimpl* m_data;
	};
}

#endif //SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED
