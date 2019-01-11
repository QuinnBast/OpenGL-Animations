//
//  ObjModel.cpp
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <cassert>
#include <cctype>
#include <cstdlib>	// for atoi
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include "ObjSettings.h"

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	#include "../GetGlutWithShaders.h"
#else
	#include "../GetGlut.h"
#endif

#include "ObjStringParsing.h"
#include "DisplayList.h"
#include "Material.h"
#include "MtlLibrary.h"
#include "MtlLibraryManager.h"
#include "ObjModel.h"

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	#include "VertexDataFormat.h"
	#include "ObjVbo.h"
	#include "MeshWithShader.h"
	#include "ModelWithShader.h"
#endif

using namespace std;
using namespace ObjLibrary;
using namespace ObjLibrary::ObjStringParsing;
namespace
{
	//
	//  Are these guaranteed to be initialized before they
	//    are needed?  That would include either of:
	//    -> at compile time
	//    -> at run time, but before any function in this
	//       file is executed.
	//
	const char* DEFAULT_FILE_NAME = "unnamed.obj";
	const char* DEFAULT_FILE_PATH = "";

	const Vector2 FALLBACK_TEXTURE_COORDINATE(0.0, 0.0);

	// I hate non-deterministic initialization...
	#define FALLBACK_NORMAL_MACRO Vector3(0.0, 0.0, 1.0)
	const Vector3 FALLBACK_NORMAL = FALLBACK_NORMAL_MACRO;

	const bool DEBUGGING_SAVE          = false;
	const bool DEBUGGING_LOAD          = false;
	const bool DEBUGGING_EDITING       = false || DEBUGGING_LOAD;
	const bool DEBUGGING_VALIDATE      = false || DEBUGGING_LOAD;
	const bool DEBUGGING_VERTEX_BUFFER = false;
	const bool DEBUGGING_FACE_SHADERS  = false;
}



const unsigned int ObjModel :: NO_TEXTURE_COORDINATES = 0xFFFFFFFF;
const unsigned int ObjModel :: NO_NORMAL = 0xFFFFFFFF;



void ObjModel :: loadDisplayTextures ()
{
	MtlLibraryManager::loadDisplayTextures();
}

void ObjModel :: loadAllTextures ()
{
	MtlLibraryManager::loadAllTextures();
}



ObjModel :: ObjModel ()
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_meshes()
{
	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	assert(isEmpty());
	assert(invariant());
}

ObjModel :: ObjModel (const string& filename)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const string& filename, const string& logfile)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename, logfile);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const string& filename, ostream& r_logstream)
		: mv_material_libraries(),
		  mv_vertexes(),
		  mv_texture_coordinates(),
		  mv_normals(),
		  mv_meshes()
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	load(filename, r_logstream);
	// load calls validate()

	assert(invariant());
}

ObjModel :: ObjModel (const ObjModel& original)
		: mv_material_libraries(original.mv_material_libraries),
		  mv_vertexes(original.mv_vertexes),
		  mv_texture_coordinates(original.mv_texture_coordinates),
		  mv_normals(original.mv_normals),
		  mv_meshes(original.mv_meshes)
{
	m_file_name         = original.m_file_name;
	m_file_path         = original.m_file_path;
	m_file_load_success = original.m_file_load_success;
	m_valid             = original.m_valid;

	assert(invariant());
}

ObjModel& ObjModel :: operator= (const ObjModel& original)
{
	if(&original != this)
	{
		mv_material_libraries = original.mv_material_libraries;
		mv_vertexes = original.mv_vertexes;
		mv_texture_coordinates = original.mv_texture_coordinates;
		mv_normals = original.mv_normals;
		mv_meshes = original.mv_meshes;

		m_file_name         = original.m_file_name;
		m_file_path         = original.m_file_path;
		m_file_load_success = original.m_file_load_success;
		m_valid             = original.m_valid;
	}

	assert(invariant());
	return *this;
}

ObjModel :: ~ObjModel ()
{
}



const string& ObjModel :: getFileName () const
{
	return m_file_name;
}

const string& ObjModel :: getFilePath () const
{
	return m_file_path;
}

string ObjModel :: getFileNameWithPath () const
{
	return m_file_path + m_file_name;
}

bool ObjModel :: isLoadedSuccessfully () const
{
	return m_file_load_success;
}

bool ObjModel :: isEmpty () const
{
	if(!mv_material_libraries.empty()) return false;
	if(!mv_vertexes.empty()) return false;
	if(!mv_texture_coordinates.empty()) return false;
	if(!mv_normals.empty()) return false;
	if(!mv_meshes.empty()) return false;
	return true;
}

unsigned int ObjModel :: getMaterialLibraryCount () const
{
	return mv_material_libraries.size();
}

bool ObjModel :: isSingleMaterialLibrary () const
{
	return (mv_material_libraries.size() == 1);
}

const string& ObjModel :: getMaterialLibraryName (unsigned int library) const
{
	assert(library < getMaterialLibraryCount());

	assert(library < mv_material_libraries.size());
	return mv_material_libraries[library].m_file_name;
}

string ObjModel :: getMaterialLibraryPath (unsigned int library) const
{
	assert(library < getMaterialLibraryCount());

	assert(library < mv_material_libraries.size());
	if(mv_material_libraries[library].mp_mtl_library == NULL)
	{
		//  CAN THIS HAPPEN?  <|>
		return "";
	}

	assert(mv_material_libraries[library].mp_mtl_library != NULL);
	return mv_material_libraries[library].mp_mtl_library->getFileNameWithPath();
}

string ObjModel :: getMaterialLibraryNameWithPath (unsigned int library) const
{
	assert(library < getMaterialLibraryCount());

	assert(library < mv_material_libraries.size());
	if(mv_material_libraries[library].mp_mtl_library == NULL)
	{
		//  CAN THIS HAPPEN?  <|>
		return mv_material_libraries[library].m_file_name;
	}

	assert(mv_material_libraries[library].mp_mtl_library != NULL);
	return mv_material_libraries[library].mp_mtl_library->getFileNameWithPath();
}

const MtlLibrary* ObjModel :: getMaterialLibrary (unsigned int library) const
{
	assert(library < getMaterialLibraryCount());

	assert(library < mv_material_libraries.size());
	assert(mv_material_libraries[library].mp_mtl_library != NULL);  //  CAN THIS HAPPEN?  <|>
	return mv_material_libraries[library].mp_mtl_library;
}

const string& ObjModel :: getSingleMaterialLibraryName () const
{
	assert(isSingleMaterialLibrary());

	assert(mv_material_libraries.size() >= 1);
	return mv_material_libraries[0].m_file_name;
}

string ObjModel :: getSingleMaterialLibraryPath () const
{
	assert(isSingleMaterialLibrary());

	assert(mv_material_libraries.size() >= 1);
	if(mv_material_libraries[0].mp_mtl_library == NULL)
	{
		//  CAN THIS HAPPEN?  <|>
		return "";
	}

	assert(mv_material_libraries[0].mp_mtl_library != NULL);
	return mv_material_libraries[0].m_file_name;
}

string ObjModel :: getSingleMaterialLibraryNameWithPath () const
{
	assert(isSingleMaterialLibrary());

	assert(mv_material_libraries.size() >= 1);
	if(mv_material_libraries[0].mp_mtl_library == NULL)
	{
		//  CAN THIS HAPPEN?  <|>
		return mv_material_libraries[0].m_file_name;
	}

	assert(mv_material_libraries[0].mp_mtl_library != NULL);
	return mv_material_libraries[0].mp_mtl_library->getFileNameWithPath();
}

const MtlLibrary* ObjModel :: getSingleMaterialLibrary () const
{
	assert(isSingleMaterialLibrary());

	assert(mv_material_libraries.size() >= 1);
	assert(mv_material_libraries[0].mp_mtl_library != NULL);  //  CAN THIS HAPPEN?  <|>
	return mv_material_libraries[0].mp_mtl_library;
}

unsigned int ObjModel :: getVertexCount () const
{
	return mv_vertexes.size();
}

double ObjModel :: getVertexX (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].x;
}

double ObjModel :: getVertexY (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].y;
}

double ObjModel :: getVertexZ (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex].z;
}

const Vector3& ObjModel :: getVertexPosition (unsigned int vertex) const
{
	assert(vertex < getVertexCount());

	return mv_vertexes[vertex];
}


unsigned int ObjModel :: getTextureCoordinateCount () const
{
	return mv_texture_coordinates.size();
}

double ObjModel :: getTextureCoordinateU (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate].x;
}

double ObjModel :: getTextureCoordinateV (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate].y;
}

const Vector2& ObjModel :: getTextureCoordinate (unsigned int texture_coordinate) const
{
	assert(texture_coordinate < getTextureCoordinateCount());

	return mv_texture_coordinates[texture_coordinate];
}

unsigned int ObjModel :: getNormalCount () const
{
	return mv_normals.size();
}

double ObjModel :: getNormalX (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].x;
}

double ObjModel :: getNormalY (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].y;
}

double ObjModel :: getNormalZ (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal].z;
}

const Vector3& ObjModel :: getNormalVector (unsigned int normal) const
{
	assert(normal < getNormalCount());

	return mv_normals[normal];
}

unsigned int ObjModel :: getMeshCount () const
{
	return mv_meshes.size();
}

bool ObjModel :: isMeshMaterial (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	if(mv_meshes[mesh].m_material_name != "")
		return true;
	else
		return false;
}

const string& ObjModel :: getMeshMaterialName (unsigned int mesh) const
{
	assert(mesh < getMeshCount());
	assert(isMeshMaterial(mesh));

	assert(mv_meshes[mesh].m_material_name != "");
	return mv_meshes[mesh].m_material_name;
}

const Material* ObjModel :: getMeshMaterial (unsigned int mesh) const
{
	assert(mesh < getMeshCount());
	assert(isMeshMaterial(mesh));

	assert(mv_meshes[mesh].mp_material != NULL);  //  CAN THIS HAPPEN?  <|>
	return mv_meshes[mesh].mp_material;
}

unsigned int ObjModel :: getPointSetCount (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].mv_point_sets.size();
}

unsigned int ObjModel :: getPointSetVertexCount (unsigned int mesh, unsigned int point_set) const
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));

	return mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes.size();
}

unsigned int ObjModel :: getPointSetVertexIndex (unsigned int mesh, unsigned int point_set, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));
	assert(vertex < getPointSetVertexCount(mesh, point_set));

	return mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes[vertex];
}

unsigned int ObjModel :: getPolylineCount (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].mv_polylines.size();
}

unsigned int ObjModel :: getPolylineVertexCount (unsigned int mesh, unsigned int polyline) const
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	return mv_meshes[mesh].mv_polylines[polyline].mv_vertexes.size();
}

unsigned int ObjModel :: getPolylineVertexIndex (unsigned int mesh, unsigned int polyline, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, polyline));

	return mv_meshes[mesh].mv_polylines[polyline].mv_vertexes[vertex].m_vertex;
}

unsigned int ObjModel :: getPolylineVertexTextureCoordinates (unsigned int mesh, unsigned int polyline, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, polyline));

	return mv_meshes[mesh].mv_polylines[polyline].mv_vertexes[vertex].m_texture_coordinate;
}

bool ObjModel :: isPolylineTextureCoordinatesAny (unsigned int mesh, unsigned int polyline) const
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	const vector<PolylineVertex>& v_vertexes = mv_meshes[mesh].mv_polylines[polyline].mv_vertexes;
	for(unsigned int i = 0; i < v_vertexes.size(); i++)
		if(v_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
			return true;
	return false;
}

unsigned int ObjModel :: getFaceCount (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].mv_faces.size();
}

unsigned int ObjModel :: getFaceVertexCount (unsigned int mesh, unsigned int face) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes.size();
}

unsigned int ObjModel :: getFaceVertexIndex (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_vertex;
}

unsigned int ObjModel :: getFaceVertexTextureCoordinates (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_texture_coordinate;
}

unsigned int ObjModel :: getFaceVertexNormal (unsigned int mesh, unsigned int face, unsigned int vertex) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	return mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_normal;
}

bool ObjModel :: isFaceTextureCoordinatesAny (unsigned int mesh, unsigned int face) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	const vector<FaceVertex>& v_vertexes = mv_meshes[mesh].mv_faces[face].mv_vertexes;
	for(unsigned int i = 0; i < v_vertexes.size(); i++)
		if(v_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
			return true;
	return false;
}

bool ObjModel :: isFaceNormalAny (unsigned int mesh, unsigned int face) const
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	const vector<FaceVertex>& v_vertexes = mv_meshes[mesh].mv_faces[face].mv_vertexes;
	for(unsigned int i = 0; i < v_vertexes.size(); i++)
		if(v_vertexes[i].m_normal != NO_NORMAL)
			return true;
	return false;
}

bool ObjModel :: isMeshAllTriangles (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	return mv_meshes[mesh].m_all_triangles;
}

bool ObjModel :: isMeshTextureCoordinatesAny (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	const vector<Face>& v_faces = mv_meshes[mesh].mv_faces;
	for(unsigned int f = 0; f < v_faces.size(); f++)
	{
		const vector<FaceVertex>& v_vertexes = v_faces[f].mv_vertexes;
		for(unsigned int i = 0; i < v_vertexes.size(); i++)
			if(v_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
				return true;
	}
	return false;
}

bool ObjModel :: isMeshNormalAny (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	const vector<Face>& v_faces = mv_meshes[mesh].mv_faces;
	for(unsigned int f = 0; f < v_faces.size(); f++)
	{
		const vector<FaceVertex>& v_vertexes = v_faces[f].mv_vertexes;
		for(unsigned int i = 0; i < v_vertexes.size(); i++)
			if(v_vertexes[i].m_normal != NO_NORMAL)
				return true;
	}
	return false;
}

unsigned int ObjModel :: getMeshPointCountTotal (unsigned int mesh) const
{
	assert(mesh < getMeshCount());

	unsigned int total = 0;

	const vector<PointSet>& v_point_sets = mv_meshes[mesh].mv_point_sets;
	for(unsigned int i = 0; i < v_point_sets.size(); i++)
		total += v_point_sets[i].mv_vertexes.size();
	return total;
}

unsigned int ObjModel :: getPointSetCountTotal () const
{
	unsigned int total = 0;

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		total += mv_meshes[i].mv_point_sets.size();
	return total;
}

unsigned int ObjModel :: getPolylineCountTotal () const
{
	unsigned int total = 0;

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		total += mv_meshes[i].mv_polylines.size();
	return total;
}

unsigned int ObjModel :: getFaceCountTotal () const
{
	unsigned int total = 0;

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		total += mv_meshes[i].mv_faces.size();
	return total;
}

bool ObjModel :: isAllTriangles () const
{
	for(unsigned int m = 0; m < mv_meshes.size(); m++)
		if(!mv_meshes[m].m_all_triangles)
			return false;
	return true;
}

bool ObjModel :: isSingleMaterial () const
{
	if(mv_meshes.empty())
		return false;

	assert(mv_meshes.size() > 0);
	const string& material_name = mv_meshes[0].m_material_name;
	if(material_name == "")
		return false;

	for(unsigned int i = 1; i < mv_meshes.size(); i++)
		if(mv_meshes[i].m_material_name != material_name)
			return false;
	return true;
}

const string& ObjModel :: getSingleMaterialName () const
{
	assert(isSingleMaterial());

	assert(mv_meshes.size() > 0);
	assert(mv_meshes[0].m_material_name != "");  //  CAN THIS HAPPEN?  <|>
	return mv_meshes[0].m_material_name;
}

const Material* ObjModel :: getSingleMaterial () const
{
	assert(isSingleMaterial());

	assert(mv_meshes.size() > 0);
	assert(mv_meshes[0].mp_material != NULL);  //  CAN THIS HAPPEN?  <|>
	return mv_meshes[0].mp_material;
}

vector<string> ObjModel :: getAllMaterialNames () const
{
	unsigned int mesh_count = mv_meshes.size();
	vector<string> v_names(mesh_count, "");

	for(unsigned int m = 0; m < mesh_count; m++)
		if(isMeshMaterial(m))
			v_names[m] = getMeshMaterialName(m);

	return v_names;
}

bool ObjModel :: isValid () const
{
	return m_valid;
}



void ObjModel :: print () const
{
	print(cout);
}

void ObjModel :: print (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	print(logstream);
	logstream.close();
}

void ObjModel :: print (ostream& r_logstream) const
{
	if(m_valid)
		r_logstream << m_file_path << m_file_name << " (valid)" << endl;
	else
		r_logstream << m_file_path << m_file_name << " (invalid)" << endl;

	if(mv_material_libraries.size() > 0)
	{
		r_logstream << "    " << mv_material_libraries.size() << " material libraries" << endl;
		for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
			r_logstream << "        " << mv_material_libraries[m].m_file_name << endl;
	}

	r_logstream << "  Vertices: " << getVertexCount() << endl;
	for(unsigned int v = 0; v < getVertexCount(); v++)
		r_logstream << "    " << setw(6) << v << ": " << mv_vertexes[v] << endl;

	r_logstream << "  Texture Coordinate Pairs: " << getTextureCoordinateCount() << endl;
	for(unsigned int t = 0; t < getTextureCoordinateCount(); t++)
		r_logstream << "    " << setw(6) << t << ": (" << mv_texture_coordinates[t].x << ", " << mv_texture_coordinates[t].y << ")" << endl;

	r_logstream << "  Normals: " << getNormalCount() << endl;
	for(unsigned int n = 0; n < getNormalCount(); n++)
		r_logstream << "    " << setw(6) << n << ": " << mv_normals[n] << endl;

	r_logstream << "  Meshes: " << getMeshCount() << endl;
	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		r_logstream << "    Mesh " << m << ":" << endl;

		if(isMeshMaterial(m))
			r_logstream << "      Material: \"" << getMeshMaterialName(m) << "\"" << endl;
		else
			r_logstream << "      Material: None" << endl;

		r_logstream << "      Point Sets: " << getPointSetCount(m) << endl;
		for(unsigned int p = 0; p < getPointSetCount(m); p++)
		{
			r_logstream << "        " << setw(6) << p << ": " << getPointSetVertexCount(m, p) << endl;
			for(unsigned int i = 0; i < getPointSetVertexCount(m, p); i++)
				cout << "          " << setw(6) << i << ": " << getPointSetVertexIndex(m, p, i) << endl;
		}

		r_logstream << "      Polylines: " << getPolylineCount(m) << endl;
		for(unsigned int l = 0; l < getPolylineCount(m); l++)
		{
			r_logstream << "        " << setw(6) << l << ": " << getPolylineVertexCount(m, l) << endl;
			for(unsigned int i = 0; i < getPolylineVertexCount(m, l); i++)
			{
				r_logstream << "          " << setw(6) << i << ": ";
				r_logstream << getPolylineVertexIndex(m, l, i);
				if(getPolylineVertexTextureCoordinates(m, l, i) != NO_TEXTURE_COORDINATES)
					cout << "\t" << getPolylineVertexTextureCoordinates(m, l, i);
				r_logstream << endl;
			}
		}

		r_logstream << "      Faces: " << getFaceCount(m);
		if(isMeshAllTriangles(m))
			cout << " (triangles)";
		r_logstream << endl;
		for(unsigned int f = 0; f < getFaceCount(m); f++)
		{
			r_logstream << "        " << setw(6) << f << ": " << getFaceVertexCount(m, f) << endl;
			for(unsigned int i = 0; i < getFaceVertexCount(m, f); i++)
			{
				r_logstream << "          " << setw(6) << i << ": ";
				r_logstream << getFaceVertexIndex(m, f, i);
				if(getFaceVertexTextureCoordinates(m, f, i) != NO_TEXTURE_COORDINATES)
				{
					r_logstream << "\t" << getFaceVertexTextureCoordinates(m, f, i);
					if(getFaceVertexNormal(m, f, i) != NO_NORMAL)
						r_logstream << "\t" << getFaceVertexNormal(m, f, i);
				}
				else if(getFaceVertexNormal(m, f, i) != NO_NORMAL)
					r_logstream << "\t\t" << getFaceVertexNormal(m, f, i);
				r_logstream << endl;
			}
		}
	}

	r_logstream << endl;
}

void ObjModel :: printMtlLibraries () const
{
	printMtlLibraries(cout);
}

void ObjModel :: printMtlLibraries (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	printMtlLibraries(logstream);
	logstream.close();
}

void ObjModel :: printMtlLibraries (ostream& r_logstream) const
{
	r_logstream << "Libraries referenced by \"" << m_file_path << m_file_name << "\":" << endl;
	for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
		r_logstream << "\t\"" << mv_material_libraries[m].m_file_name << "\"" << endl;
}

void ObjModel :: printBadMaterials () const
{
	printBadMaterials(cout);
}

void ObjModel :: printBadMaterials (const string& logfile) const
{
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	printBadMaterials(logstream);
	logstream.close();
}

void ObjModel :: printBadMaterials (ostream& r_logstream) const
{
	for(unsigned int m = 0; m < getMeshCount(); m++)
		if(mv_meshes[m].m_material_name != "" && mv_meshes[m].mp_material == NULL)
		{
			r_logstream << "Invalid material referenced in \"" << m_file_path << m_file_name << "\": "
			     << "\"" << mv_meshes[m].m_material_name << "\"" << endl;
		}
}



#ifndef OBJ_LIBRARY_SHADER_DISPLAY

void ObjModel :: draw () const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, mv_meshes[m].mp_material);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterialNone () const
{
	assert(isValid());

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);
}

void ObjModel :: drawMaterial (const Material& material) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	material.activate();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);

	Material::deactivate();

	if(material.isSeperateSpecular())
	{
		material.activateSeperateSpecular();
		for(unsigned int m = 0; m < getMeshCount(); m++)
			drawMesh(m);
		Material::deactivate();
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterial (const char* a_name) const
{
	assert(isValid());
	assert(a_name != NULL);

	drawMaterial(string(a_name));
}

void ObjModel :: drawMaterial (const string& name) const
{
	assert(isValid());

	const Material* p_material = getMaterialByName(name);
	if(p_material != NULL)
		drawMaterial(*p_material);
	else
	{
		//  WRITE TO LOGING STREAM  <|>
		cerr << "Material \"" << name << "\" does not exist, displaying without material" << endl;
		drawMaterialNone();
	}
}

void ObjModel :: drawMaterial (const char* a_material_name, const char* a_library_name) const
{
	assert(isValid());
	assert(a_material_name != NULL);
	assert(a_library_name != NULL);

	drawMaterial(string(a_material_name), string(a_library_name));
}

void ObjModel :: drawMaterial (const string& material_name, const string& library_name) const
{
	assert(isValid());

	const MtlLibrary& mtl_library = MtlLibraryManager::get(library_name);

	unsigned int index = mtl_library.getMaterialIndex(material_name);
	if(index != MtlLibrary::NO_SUCH_MATERIAL)
		drawMaterial(*(mtl_library.getMaterial(index)));
	else
	{
		//  WRITE TO LOGING STREAM  <|>
		cerr << "Material \"" << material_name << "\" in library \"" << library_name
		     << "\" does not exist, displaying without material" << endl;
		drawMaterialNone();
	}
}

void ObjModel :: drawMaterials (const vector<Material*>& materials) const
{
	assert(isValid());
	assert(materials.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, materials[m]);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterials (const vector<const char*>& va_names) const
{
	assert(isValid());
	assert(va_names.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		if(va_names[m] == NULL)
			drawMesh(m);
		else
			drawMeshMaterial(m, getMaterialByName(va_names[m]));
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMaterials (const vector<string>& v_names) const
{
	assert(isValid());
	assert(v_names.size() >= getMeshCount());

	if(Material::isMaterialActive())
		Material::deactivate();
	assert(!Material::isMaterialActive());

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMeshMaterial(m, getMaterialByName(v_names[m]));

	assert(!Material::isMaterialActive());
}


void ObjModel :: drawMtlLibrary (const MtlLibrary& library) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		const Material* p_material = NULL;

		if(mv_meshes[m].m_material_name != "")
			p_material = library.getMaterial(mv_meshes[m].m_material_name);
		if(p_material == NULL)
			p_material = mv_meshes[m].mp_material;

		drawMeshMaterial(m, p_material);
	}

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawMtlLibrary (const char* a_name) const
{
	assert(isValid());
	assert(a_name != NULL);

	drawMtlLibrary(MtlLibraryManager::get(a_name));
}

void ObjModel :: drawMtlLibrary (const string& name) const
{
	assert(isValid());

	drawMtlLibrary(MtlLibraryManager::get(name));
}

void ObjModel :: drawVertices (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawVertices(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertices (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawVertices(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertices (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_POINTS);
		for(unsigned int v = 0; v < getVertexCount(); v++)
			glVertex3dv(mv_vertexes[v].getAsArray());
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawWireframe(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawWireframe(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawWireframe (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	for(unsigned int m = 0; m < getMeshCount(); m++)
	{
		drawPointSets(m);
		drawPolylines(m);

		for(unsigned int f = 0; f < getFaceCount(m); f++)
		{
			glBegin(GL_LINE_LOOP);
				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
					glVertex3dv(mv_vertexes[vertex].getAsArray());
				}
			glEnd();
		}
	}

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	drawSolid(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawSolid(red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawSolid (double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	for(unsigned int m = 0; m < getMeshCount(); m++)
		drawMesh(m);

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);

	drawVertexNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawVertexNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawVertexNormals (double length, double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_LINES);
		for(unsigned int m = 0; m < getMeshCount(); m++)
			for(unsigned int f = 0; f < getFaceCount(m); f++)
				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
					unsigned int normal = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_normal;

					if(normal != NO_NORMAL)
					{
						assert(vertex < getVertexCount());
						assert(normal < getNormalCount());

						Vector3 normal_end = mv_vertexes[vertex] + mv_normals[normal] * length;

						glVertex3dv(mv_vertexes[vertex].getAsArray());
						glVertex3dv(normal_end.getAsArray());
					}
				}
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, unsigned char red, unsigned char green, unsigned char blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);

	drawFaceNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, int red, int green, int blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0x00);
	assert(red <= 0xFF);
	assert(green >= 0x00);
	assert(green <= 0xFF);
	assert(blue >= 0x00);
	assert(blue <= 0xFF);

	drawFaceNormals(length, red / 255.0, green / 255.0, blue / 255.0);

	assert(!Material::isMaterialActive());
}

void ObjModel :: drawFaceNormals (double length, double red, double green, double blue) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(length > 0.0);
	assert(red >= 0.0);
	assert(red <= 1.0);
	assert(green >= 0.0);
	assert(green <= 1.0);
	assert(blue >= 0.0);
	assert(blue <= 1.0);

	Material material;

	material.setIlluminationMode(Material::ILLUMINATION_CONSTANT);
	material.setDiffuseColour(red, green, blue);
	material.activate();

	glBegin(GL_LINES);
		for(unsigned int m = 0; m < getMeshCount(); m++)
			for(unsigned int f = 0; f < getFaceCount(m); f++)
			{
				Vector3 center = Vector3::ZERO;
				Vector3 face_normal = Vector3::ZERO;

				for(unsigned int v = 0; v < getFaceVertexCount(m, f); v++)
				{
					unsigned int vertex = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
					unsigned int normal = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_normal;

					assert(vertex < getVertexCount());
					center += mv_vertexes[vertex];

					if(normal != NO_NORMAL)
					{
						assert(normal < getNormalCount());
						face_normal += mv_normals[normal];
					}
				}

				if(!face_normal.isZero())
				{
					center /= getFaceVertexCount(m, f);
					face_normal.setNorm(length);

					Vector3 normal_end = center + face_normal;

					glVertex3dv(center.getAsArray());
					glVertex3dv(normal_end.getAsArray());
				}
			}
	glEnd();

	material.deactivate();

	assert(!Material::isMaterialActive());
}

DisplayList ObjModel :: getDisplayList () const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	//
	//  We are using a loop to load textures instead of
	//
	//      loadDisplayTextures();
	//
	//    so that we only load textures used by this model.
	//    Otherwise, any texture required by any material in
	//    an MTL file referenced by an ObjModel would be
	//    loaded, even if that material - and thus that
	//    texture - was never used.  This way, we avoid
	//    cluttering up video memory with textures we never
	//    use.
	//

	for(unsigned int i = 0; i < mv_meshes.size(); i++)
		if(mv_meshes[i].mp_material != NULL)
			mv_meshes[i].mp_material->loadDisplayTextures();

	DisplayList list;
	list.begin();
		draw();
	list.end();

	assert(!Material::isMaterialActive());
	return list;
}
DisplayList ObjModel :: getDisplayListMaterial (const Material& material) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	// ensure material and all textures are loaded
	material.activate();
	Material::deactivate();
	assert(!Material::isMaterialActive());

	DisplayList list;
	list.begin();
		drawMaterial(material);
	list.end();

	assert(!Material::isMaterialActive());
	return list;
}

DisplayList ObjModel :: getDisplayListMaterial (const char* a_name) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(a_name != NULL);

	return getDisplayListMaterial(string(a_name), cerr);
}

DisplayList ObjModel :: getDisplayListMaterial (const string& name) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	return getDisplayListMaterial(name, cerr);
}

DisplayList ObjModel :: getDisplayListMaterial (const char* a_name, const string& logfile) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(a_name != NULL);
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	return getDisplayListMaterial(string(a_name), logstream);
	// logstream.close() will be called by ofstream destructor
}

DisplayList ObjModel :: getDisplayListMaterial (const string& name, const string& logfile) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	return getDisplayListMaterial(string(name), logstream);
	// logstream.close() will be called by ofstream destructor
}

DisplayList ObjModel :: getDisplayListMaterial (const char* a_name, ostream& r_logstream) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());
	assert(a_name != NULL);

	return getDisplayListMaterial(string(a_name), r_logstream);
}

DisplayList ObjModel :: getDisplayListMaterial (const string& name, ostream& r_logstream) const
{
	assert(isValid());
	assert(!Material::isMaterialActive());

	const Material* p_material = getMaterialByName(name);
	if(p_material != NULL)
		return getDisplayListMaterial(*p_material);
	else
	{
		r_logstream << "Material \"" << name << "\" does not exist, creating DisplayList without material" << endl;
		return getDisplayListMaterialNone();
	}
}

DisplayList ObjModel :: getDisplayListMaterialNone () const
{
	assert(isValid());

	DisplayList list;

	list.begin();
		drawMaterialNone();
	list.end();

	return list;
}

#endif  // OBJ_LIBRARY_SHADER_DISPLAY is not defined



#ifdef OBJ_LIBRARY_SHADER_DISPLAY

ModelWithShader ObjModel :: getModelWithShader () const
{
	assert(isValid());

	return getModelWithShader(true, true);
}

ModelWithShader ObjModel :: getModelWithShader (bool is_texture_coordinates,
                                                bool is_normals) const
{
	assert(isValid());

	static const Material MATERIAL_FALLBACK = Material::createSolid("shader_material_fallback",
	                                                                Vector3(1.0, 0.0, 0.0));

	ModelWithShader result;

	for(unsigned int m = 0; m < mv_meshes.size(); m++)
	{
		if(getPointSetCount(m) == 0 &&
		   getPolylineCount(m) == 0 &&
		   getFaceCount    (m) == 0)
		{
			// nothing in this mesh, so skip it
			continue;
		}

		unsigned int material_index;

		if(mv_meshes[m].mp_material == NULL)
			material_index = result.addMaterial(MATERIAL_FALLBACK.getForShader());
		else
			material_index = result.addMaterial(mv_meshes[m].mp_material->getForShader());

		// add point sets
		if(getPointSetCount(m) > 0)
		{
			assert(!mv_meshes[m].mv_point_sets.empty());
			result.addMesh(material_index, getPointSetMeshWithShader(m));
		}

		// add polylines
		for(unsigned int l = 0; l < mv_meshes[m].mv_polylines.size(); l++)
		{
			bool is_polyline_texture_coordinates = is_texture_coordinates;
			if(!isPolylineTextureCoordinatesAny(m, l))
				is_polyline_texture_coordinates = false;

			result.addMesh(material_index, getPolylineMeshWithShader(m, l, is_polyline_texture_coordinates));
		}

		// add faces
		if(getFaceCount(m) > 0)
		{
			assert(!mv_meshes[m].mv_faces.empty());

			bool is_mesh_texture_coordinates = is_texture_coordinates;
			if(!isMeshTextureCoordinatesAny(m))
				is_mesh_texture_coordinates = false;

			bool is_mesh_normals = is_normals;
			if(!isMeshNormalAny(m))
				is_mesh_normals = false;

			result.addMesh(material_index, getFaceMeshWithShader(m, is_mesh_texture_coordinates, is_mesh_normals));
		}
	}

	assert(result.isReady());
	return result;
}

#endif  // OBJ_LIBRARY_SHADER_DISPLAY is defined



void ObjModel :: save (const string& filename) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	save(filename, cerr);

	assert(invariant());
}

void ObjModel :: save (const string& filename, const string& logfile) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	save(filename, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel :: save (const string& filename, ostream& r_logstream) const
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ofstream output_file;

	if(DEBUGGING_SAVE)
		cout << "About to save " << filename << endl;

	output_file.open(filename.c_str());
	if(!output_file.is_open())
	{
		r_logstream << "Error: Cannot write to file \"" << filename << "\" - ABORTING" << endl;
		return;
	}

	//
	//  Format of file:
	//
	//  Header
	//    -> how many of each element
	//  Material libraries
	//  Vertices
	//  Texture coordinate pairs
	//  Normals
	//  Meshes with associated materials
	//    Point sets
	//    Polylines
	//    Faces
	//  Footer
	//
	//  Major sections are seperated by 3 blank lines.  Minor
	//    sections are seperated by 1 blank line.
	//

	output_file << "#" << endl;
	output_file << "# " << getFileNameWithPath() << endl;
	output_file << "#" << endl;
	if(mv_vertexes.size() > 0)
		output_file << "# " << getVertexCount() << " vertexes" << endl;
	if(mv_texture_coordinates.size() > 0)
		output_file << "# " << getTextureCoordinateCount() << " texture coordinate pairs" << endl;
	if(mv_normals.size() > 0)
		output_file << "# " << getNormalCount() << " vertex normals" << endl;
	if(mv_meshes.size() > 0)
	{
		output_file << "# " << getMeshCount() << " meshes" << endl;
		if(getPointSetCountTotal() > 0)
			output_file << "#  " << getPointSetCountTotal() << " point sets" << endl;
		if(getPolylineCountTotal() > 0)
			output_file << "#  " << getPolylineCountTotal() << " polylines" << endl;
		if(getFaceCountTotal() > 0)
			output_file << "#  " << getFaceCountTotal() << " faces" << endl;
	}
	output_file << "#" << endl;
	output_file << endl;
	output_file << endl;
	output_file << endl;

	if(DEBUGGING_SAVE)
		cout << "Wrote file header" << endl;

	if(mv_material_libraries.size() > 0)
	{
		output_file << "# " << mv_material_libraries.size() << " material libraries" << endl;
		output_file << "mtllib";
		for(unsigned int m = 0; m < mv_material_libraries.size(); m++)
			output_file << " " << mv_material_libraries[m].m_file_name;
		output_file << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING_SAVE)
			cout << "Wrote material libraries" << endl;
	}

	if(mv_vertexes.size() > 0)
	{
		output_file << "# " << getVertexCount() << " vertexes" << endl;
		for(unsigned int v = 0; v < mv_vertexes.size(); v++)
			output_file << "v " << mv_vertexes[v].x << " " << mv_vertexes[v].y << " " << mv_vertexes[v].z << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING_SAVE)
			cout << "Wrote vertexes" << endl;
	}

	if(mv_texture_coordinates.size() > 0)
	{
		output_file << "# " << getTextureCoordinateCount() << " texture coordinate pairs" << endl;
		for(unsigned int t = 0; t < mv_texture_coordinates.size(); t++)
			output_file << "vt " << mv_texture_coordinates[t].x << " " << mv_texture_coordinates[t].y << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING_SAVE)
			cout << "Wrote texture coordinates" << endl;
	}

	if(mv_normals.size() > 0)
	{
		output_file << "# " << getNormalCount() << " vertex normals" << endl;
		for(unsigned int n = 0; n < mv_normals.size(); n++)
			output_file << "vn " << mv_normals[n].x << " " << mv_normals[n].y << " " << mv_normals[n].z << endl;
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING_SAVE)
			cout << "Wrote normals" << endl;
	}

	if(mv_meshes.size() > 0)
	{
		output_file << "# " << getMeshCount() << " meshes" << endl;
		output_file << endl;

		for(unsigned int m = 0; m < mv_meshes.size(); m++)
		{
			if(isMeshMaterial(m))
				output_file << "usemtl " << mv_meshes[m].m_material_name << endl;

			if(mv_meshes[m].mv_point_sets.size() > 0)
			{
				output_file << "# " << getPointSetCount(m) << " faces" << endl;
				for(unsigned int p = 0; p < mv_meshes[m].mv_point_sets.size(); p++)
				{
					output_file << "p";
					for(unsigned int i = 0; i < mv_meshes[m].mv_point_sets[p].mv_vertexes.size(); i++)
						output_file << " " << (mv_meshes[m].mv_point_sets[p].mv_vertexes[i] + 1);
					output_file << endl;
				}
				output_file << endl;

				if(DEBUGGING_SAVE)
					cout << "Wrote point sets for mesh " << m << endl;
			}

			if(mv_meshes[m].mv_polylines.size() > 0)
			{
				output_file << "# " << getPolylineCount(m) << " faces" << endl;
				for(unsigned int l = 0; l < mv_meshes[m].mv_polylines.size(); l++)
				{
					output_file << "l";
					for(unsigned int i = 0; i < mv_meshes[m].mv_polylines[l].mv_vertexes.size(); i++)
					{
						output_file << " " << (mv_meshes[m].mv_polylines[l].mv_vertexes[i].m_vertex + 1);

						if(mv_meshes[m].mv_polylines[l].mv_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
							output_file << "/" << (mv_meshes[m].mv_polylines[l].mv_vertexes[i].m_texture_coordinate + 1);
					}
					output_file << endl;
				}
				output_file << endl;

				if(DEBUGGING_SAVE)
					cout << "Wrote polylines for mesh " << m << endl;
			}

			if(mv_meshes[m].mv_faces.size() > 0)
			{
				output_file << "# " << getFaceCount(m) << " faces" << endl;
				for(unsigned int f = 0; f < mv_meshes[m].mv_faces.size(); f++)
				{
					output_file << "f";
					for(unsigned int i = 0; i < mv_meshes[m].mv_faces[f].mv_vertexes.size(); i++)
					{
						output_file << " " << (mv_meshes[m].mv_faces[f].mv_vertexes[i].m_vertex + 1);

						if(mv_meshes[m].mv_faces[f].mv_vertexes[i].m_texture_coordinate != NO_TEXTURE_COORDINATES)
						{
							output_file << "/" << (mv_meshes[m].mv_faces[f].mv_vertexes[i].m_texture_coordinate + 1);

							if(mv_meshes[m].mv_faces[f].mv_vertexes[i].m_normal != NO_NORMAL)
								output_file << "/" << (mv_meshes[m].mv_faces[f].mv_vertexes[i].m_normal + 1);
						}
						else if(mv_meshes[m].mv_faces[f].mv_vertexes[i].m_normal != NO_NORMAL)
							output_file << "//" << (mv_meshes[m].mv_faces[f].mv_vertexes[i].m_normal + 1);

					}
					output_file << endl;
				}
				output_file << endl;

				if(DEBUGGING_SAVE)
					cout << "Wrote faces for mesh " << m << endl;
			}
		}
		output_file << endl;
		output_file << endl;
		output_file << endl;

		if(DEBUGGING_SAVE)
			cout << "Wrote all meshes" << endl;
	}

	output_file << "# End of " << getFileNameWithPath() << endl;
	output_file << endl;

	if(DEBUGGING_SAVE)
		cout << "Wrote footer" << endl;

	output_file.close();
}



void ObjModel :: makeEmpty ()
{
	mv_material_libraries.clear();
	mv_vertexes.clear();
	mv_texture_coordinates.clear();
	mv_normals.clear();
	mv_meshes.clear();

	m_file_name         = DEFAULT_FILE_NAME;
	m_file_path         = DEFAULT_FILE_PATH;
	m_file_load_success = true;
	m_valid             = true;

	assert(isEmpty());
	assert(invariant());
}

void ObjModel :: load (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	load(filename, cerr);

	assert(invariant());
}

void ObjModel :: load (const string& filename, const string& logfile)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	load(filename, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel :: load (const string& filename, ostream& r_logstream)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	ifstream input_file;
	unsigned int line_count;

	if(DEBUGGING_LOAD)
		cout << "About to remove any existing contents" << endl;

	makeEmpty();

	if(DEBUGGING_LOAD)
		cout << "About to load " << filename << endl;

	setFileNameWithPath(filename);

	input_file.open(filename.c_str(), ios::in);
	if(!input_file.is_open())
	{
		r_logstream << "Error: File \"" << filename << "\" does not exist" << endl;
		input_file.close();

		m_file_load_success = false;

		assert(invariant());
		return;
	}

	//
	//  Format is available at
	//
	//  http://www.martinreddy.net/gfx/3d/OBJ.spec
	//

	line_count = 0;
	while(true)	// drops out at EOF below
	{
		string line;
		size_t line_length;
		bool valid;

		getline(input_file, line);
		if(line == "" && input_file.eof())
			break;	// drop out if no more lines (but include last line)

		line_length = line.length();
		line_count++;

		if(line_length < 1 || line[0] == '#' || line[0] == '\r' || line[0] == '\n')
			continue;	// skip blank lines and comments

		valid = true;
		if(line_length < 1)
			valid = false;
		else
		{
			line = whitespaceToSpaces(line);

			if(startsWith(line, "mtllib "))
				valid = readMaterialLibrary(line.substr(7), r_logstream);
			else if(startsWith(line, "usemtl "))
				valid = readMaterial(line.substr(7), r_logstream);
			else if(startsWith(line, "v "))
				valid = readVertex(line.substr(2), r_logstream);
			else if(startsWith(line, "vt "))
				valid = readTextureCoordinates(line.substr(3), r_logstream);
			else if(startsWith(line, "vn "))
				valid = readNormal(line.substr(3), r_logstream);
			else if(startsWith(line, "p "))
				valid = readPointSet(line.substr(2), r_logstream);
			else if(startsWith(line, "l "))
				valid = readPolyline(line.substr(2), r_logstream);
			else if(startsWith(line, "f "))
				valid = readFace(line.substr(2), r_logstream);
			else if(line[0] == 'g' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING_LOAD)
					r_logstream << "In file \"" << filename << "\": ignoring groupings \"" << line.substr(1) << "\"" << endl;
			}
			else if(line[0] == 's' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING_LOAD)
					r_logstream << "In file \"" << filename << "\": ignoring smoothing group \"" << line.substr(1) << "\"" << endl;
			}
			else if(line[0] == 'o' && (line.length() == 1 || isspace(line[1])))
			{
				if(DEBUGGING_LOAD)
					r_logstream << "In file \"" << filename << "\": ignoring object name \"" << line.substr(1) << "\"" << endl;
			}
			else
				valid = false;
		}

		if(!valid)
			r_logstream << "Line " << setw(6) << line_count << " of file \"" << filename << "\" is invalid: \"" << line << "\"" << endl;
	}

	input_file.close();

	validate();
	printBadMaterials();

	assert(invariant());
}



void ObjModel :: setFileName (const string& file_name)
{
	assert(file_name != "");

	m_file_name = file_name;

	assert(invariant());
}

void ObjModel :: setFilePath (const string& file_path)
{
	assert(ObjStringParsing::isValidPath(file_path));

	m_file_path = file_path;

	assert(invariant());
}

void ObjModel :: setFileNameWithPath (const string& filename)
{
	assert(filename != "");
	assert(filename.find_last_of("/\\") == string::npos ||
	       filename.find_last_of("/\\") + 1 < filename.size());

	size_t last_slash = filename.find_last_of("/\\");
	if(last_slash != string::npos)
	{
		last_slash++;
		m_file_name = filename.substr(last_slash);
		m_file_path = filename.substr(0, last_slash);
		assert(ObjStringParsing::isValidPath(m_file_path));
	}
	else
	{
		m_file_name = filename;
		m_file_path = "";
		assert(ObjStringParsing::isValidPath(m_file_path));
	}

	assert(invariant());
}

void ObjModel :: setVertexCount (unsigned int count)
{
	if(count < getVertexCount())
	{
		m_valid = false;
		mv_vertexes.resize(count);
	}
	else if(count > getVertexCount())
		mv_vertexes.resize(count, Vector3::ZERO);

	assert(invariant());
}

void ObjModel :: setVertexX (unsigned int vertex, double x)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].x = x;

	assert(invariant());
}

void ObjModel :: setVertexY (unsigned int vertex, double y)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].y = y;

	assert(invariant());
}

void ObjModel :: setVertexZ (unsigned int vertex, double z)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].z = z;

	assert(invariant());
}

void ObjModel :: setVertexPosition (unsigned int vertex, double x, double y, double z)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex].set(x, y, z);

	assert(invariant());
}

void ObjModel :: setVertexPosition (unsigned int vertex, const Vector3& position)
{
	assert(vertex < getVertexCount());

	mv_vertexes[vertex] = position;

	assert(invariant());
}

void ObjModel :: setTextureCoordinateCount (unsigned int count)
{
	if(count < getTextureCoordinateCount())
	{
		m_valid = false;
		mv_texture_coordinates.resize(count);
	}
	else if(count > getTextureCoordinateCount())
		mv_texture_coordinates.resize(count, Vector2::ZERO);

	assert(invariant());
}

void ObjModel :: setTextureCoordinateU (unsigned int texture_coordinate, double u)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].x = u;

	assert(invariant());
}
void ObjModel :: setTextureCoordinateV (unsigned int texture_coordinate, double v)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].y = v;

	assert(invariant());
}

void ObjModel :: setTextureCoordinate (unsigned int texture_coordinate, double u, double v)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate].x = u;
	mv_texture_coordinates[texture_coordinate].y = v;

	assert(invariant());
}

void ObjModel :: setTextureCoordinate (unsigned int texture_coordinate, const Vector2& coordinates)
{
	assert(texture_coordinate < getTextureCoordinateCount());

	mv_texture_coordinates[texture_coordinate] = coordinates;

	assert(invariant());
}

void ObjModel :: setNormalCount (unsigned int count)
{
	if(count < getNormalCount())
	{
		m_valid = false;
		mv_normals.resize(count);
	}
	else if(count > getNormalCount())
		mv_normals.resize(count, Vector3::UNIT_Z_PLUS);

	assert(invariant());
}

void ObjModel :: setNormalX (unsigned int normal, double x)
{
	assert(normal < getNormalCount());
	assert(x != 0.0 || getNormalY(normal) != 0.0 || getNormalZ(normal) != 0.0);

	mv_normals[normal].x = x;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalY (unsigned int normal, double y)
{
	assert(normal < getNormalCount());
	assert(getNormalX(normal) != 0.0 || y != 0.0 || getNormalZ(normal) != 0.0);

	mv_normals[normal].y = y;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalZ (unsigned int normal, double z)
{
	assert(normal < getNormalCount());
	assert(getNormalX(normal) != 0.0 || getNormalY(normal) != 0.0 || z != 0.0);

	mv_normals[normal].z = z;
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalVector (unsigned int normal, double x, double y, double z)
{
	assert(normal < getNormalCount());
	assert(x != 0.0 || y != 0.0 || z != 0.0);

	mv_normals[normal].set(x, y, z);
	assert(!mv_normals[normal].isZero());
	mv_normals[normal].normalize();

	assert(invariant());
}

void ObjModel :: setNormalVector (unsigned int normal, const Vector3& vector)
{
	assert(normal < getNormalCount());
	assert(!vector.isZero());

	mv_normals[normal] = vector.getNormalized();

	assert(invariant());
}

void ObjModel :: setMeshMaterial (unsigned int mesh, const string& material)
{
	assert(mesh < getMeshCount());
	assert(material != "");

	if(DEBUGGING_LOAD)
		cout << "    Setting mesh " << mesh << " to use material " << material << endl;

	mv_meshes[mesh].m_material_name = material;

	mv_meshes[mesh].mp_material = NULL;
	for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
	{
		if(mv_material_libraries[i].mp_mtl_library == NULL)
			continue;

		unsigned int index = mv_material_libraries[i].mp_mtl_library->getMaterialIndex(material);

		if(index == MtlLibrary::NO_SUCH_MATERIAL)
			continue;

		mv_meshes[mesh].mp_material = mv_material_libraries[i].mp_mtl_library->getMaterial(index);
	}

	assert(invariant());
}

void ObjModel :: setMeshMaterialNone (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].m_material_name = "";
	mv_meshes[mesh].mp_material = NULL;

	assert(invariant());
}

void ObjModel :: setPointSetVertexIndex (unsigned int mesh, unsigned int point_set, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));
	assert(vertex < getPointSetVertexCount(mesh, point_set));

	mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes[vertex] = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setPolylineVertexIndex (unsigned int mesh, unsigned int polyline, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, polyline));

	mv_meshes[mesh].mv_polylines[polyline].mv_vertexes[vertex].m_vertex = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setPolylineVertexTextureCoordinates (unsigned int mesh, unsigned int polyline, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, polyline));

	mv_meshes[mesh].mv_polylines[polyline].mv_vertexes[vertex].m_texture_coordinate = index;
	if(index >= getTextureCoordinateCount() && index != NO_TEXTURE_COORDINATES)
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setFaceVertexIndex (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_vertex = index;
	if(index >= getVertexCount())
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setFaceVertexTextureCoordinates (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_texture_coordinate = index;
	if(index >= getTextureCoordinateCount() && index != NO_TEXTURE_COORDINATES)
		m_valid = false;

	assert(invariant());
}

void ObjModel :: setFaceVertexNormal (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int index)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	mv_meshes[mesh].mv_faces[face].mv_vertexes[vertex].m_normal = index;
	if(index >= getVertexCount() && index != NO_NORMAL)
		m_valid = false;

	assert(invariant());
}



void ObjModel :: addMaterialLibrary (const string& library)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());

	addMaterialLibrary(library, cerr);

	assert(invariant());
}

void ObjModel :: addMaterialLibrary (const string& library, const string& logfile)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());
	assert(logfile != "");
	assert(logfile.find_last_of("/\\") == string::npos ||
	       logfile.find_last_of("/\\") + 1 < logfile.size());

	ofstream logstream(logfile.c_str());
	addMaterialLibrary(library, logstream);
	logstream.close();

	assert(invariant());
}

void ObjModel :: addMaterialLibrary (const string& library, ostream& r_logstream)
{
	assert(library != "");
	assert(library.find_last_of("/\\") == string::npos ||
	       library.find_last_of("/\\") + 1 < library.size());

#ifdef OBJ_LIBRARY_PATH_PROPAGATION
	mv_material_libraries.push_back(MaterialLibrary(m_file_path, library, r_logstream));
#else
	mv_material_libraries.push_back(MaterialLibrary("", library, r_logstream));
#endif

	if(DEBUGGING_EDITING)
	{
		unsigned int index = mv_material_libraries.size() - 1;
		cout << "Added Material Libary \"" << mv_material_libraries[index].m_file_name << "\"" << endl;
		if(mv_material_libraries[index].mp_mtl_library == NULL)
			cout << "But couldn't load it" << endl;
	}

	assert(invariant());
}

unsigned int ObjModel :: addVertex (double x, double y, double z)
{
	return addVertex(Vector3(x, y, z));
}

unsigned int ObjModel :: addVertex (const Vector3& position)
{
	unsigned int id = mv_vertexes.size();
	mv_vertexes.push_back(position);

	if(DEBUGGING_EDITING)
		cout << "Added Vertex #" << (id + 1) << " " << position << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addTextureCoordinate (double u, double v)
{
	return addTextureCoordinate(Vector2(u, v));
}

unsigned int ObjModel :: addTextureCoordinate (const Vector2& texture_coordinates)
{
	unsigned int id = mv_texture_coordinates.size();
	mv_texture_coordinates.push_back(texture_coordinates);

	if(DEBUGGING_EDITING)
		cout << "Added Texture Coordinate #" << (id + 1) << " " << texture_coordinates << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addNormal (double x, double y, double z)
{
	assert(x != 0.0 || y != 0.0 || z != 0.0);

	return addNormal(Vector3(x, y, z));
}

unsigned int ObjModel :: addNormal (const Vector3& normal)
{
	assert(!normal.isZero());

	unsigned int id = mv_normals.size();
	mv_normals.push_back(normal.getNormalized());

	if(DEBUGGING_EDITING)
		cout << "Added Normal #" << (id + 1) << " " << normal << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addMesh ()
{
	unsigned int id = mv_meshes.size();
	mv_meshes.push_back(Mesh());

	if(DEBUGGING_EDITING)
		cout << "Added mesh #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPointSet (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int id = mv_meshes[mesh].mv_point_sets.size();
	mv_meshes[mesh].mv_point_sets.push_back(PointSet());
	m_valid = false;

	if(DEBUGGING_EDITING)
		cout << "    Added Point Set #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPointSetVertex (unsigned int mesh, unsigned int point_set, unsigned int vertex)
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));

	unsigned int id = mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes.size();
	mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes.push_back(vertex);

	if(vertex >= getVertexCount())
		m_valid = false;

	if(DEBUGGING_EDITING)
		cout << "        Added vertex #" << (id + 1) << " (" << (vertex + 1) << ")" << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPolyline (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int id = mv_meshes[mesh].mv_polylines.size();
	mv_meshes[mesh].mv_polylines.push_back(Polyline());
	m_valid = false;

	if(DEBUGGING_EDITING)
		cout << "    Added Polyline #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addPolylineVertex (unsigned int mesh, unsigned int polyline, unsigned int vertex, unsigned int texture_coordinates)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	unsigned int id = mv_meshes[mesh].mv_polylines[polyline].mv_vertexes.size();
	mv_meshes[mesh].mv_polylines[polyline].mv_vertexes.push_back(PolylineVertex(vertex, texture_coordinates));

	if(vertex >= getVertexCount())
		m_valid = false;
	if(texture_coordinates != NO_TEXTURE_COORDINATES && texture_coordinates >= getTextureCoordinateCount())
		m_valid = false;

	if(DEBUGGING_EDITING)
	{
		cout << "        Added vertex #" << (id + 1) << " (" << (vertex + 1) << "/";

		if(texture_coordinates == NO_TEXTURE_COORDINATES)
			cout << " )" << endl;
		else
			cout << (texture_coordinates + 1) << ")" << endl;
	}

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addFace (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int id = mv_meshes[mesh].mv_faces.size();
	mv_meshes[mesh].mv_faces.push_back(Face());
	m_valid = false;

	if(DEBUGGING_EDITING)
		cout << "    Added face #" << (id + 1) << endl;

	assert(invariant());
	return id;
}

unsigned int ObjModel :: addFaceVertex (unsigned int mesh, unsigned int face, unsigned int vertex, unsigned int texture_coordinates, unsigned int normal)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	unsigned int id = mv_meshes[mesh].mv_faces[face].mv_vertexes.size();
	mv_meshes[mesh].mv_faces[face].mv_vertexes.push_back(FaceVertex(vertex, texture_coordinates, normal));

	if(vertex >= getVertexCount())
		m_valid = false;
	if(texture_coordinates != NO_TEXTURE_COORDINATES && texture_coordinates >= getTextureCoordinateCount())
		m_valid = false;
	if(normal != NO_NORMAL && normal >= getNormalCount())
		m_valid = false;
	if(id > 3)
		mv_meshes[mesh].m_all_triangles = false;

	if(DEBUGGING_EDITING)
	{
		cout << "        Added vertex #" << (id + 1) << " (" << (vertex + 1) << "/";

		if(texture_coordinates == NO_TEXTURE_COORDINATES)
			cout << "/";
		else
			cout << (texture_coordinates + 1) << "/";

		if(normal == NO_NORMAL)
			cout << " )" << endl;
		else
			cout << (normal + 1) << ")" << endl;
	}

	assert(invariant());
	return id;
}

void ObjModel :: removeMaterialLibaryAll ()
{
	mv_material_libraries.clear();

	unsigned int mesh_count = mv_meshes.size();
	for(unsigned int m = 0; m < mesh_count; m++)
		setMeshMaterialNone(m);

	if(DEBUGGING_EDITING)
		cout << "    Removed all material libraries and set all meshes to have no material" << endl;

	assert(invariant());
}

void ObjModel :: removeMesh (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	unsigned int mesh_count = mv_meshes.size();
	for(unsigned int i = mesh + 1; i < mesh_count; i++)
	{
		assert(i >= 1);
		mv_meshes[i - 1] = mv_meshes[i];
	}

	mv_meshes.pop_back();

	if(DEBUGGING_EDITING)
		cout << "    Removed mesh #" << (mesh + 1) << endl;

	assert(invariant());
}

void ObjModel :: removeMeshAll ()
{
	mv_meshes.clear();

	if(DEBUGGING_EDITING)
		cout << "    Removed all meshes" << endl;

	assert(invariant());
}

void ObjModel :: removePointSet (unsigned int mesh,
                                 unsigned int point_set)
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));

	unsigned int point_set_count = mv_meshes[mesh].mv_point_sets.size();
	for(unsigned int i = point_set + 1; i < point_set_count; i++)
	{
		assert(i >= 1);
		mv_meshes[mesh].mv_point_sets[i - 1] = mv_meshes[mesh].mv_point_sets[i];
	}
	mv_meshes[mesh].mv_point_sets.pop_back();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<      ", point set #" << (point_set + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePointSetAll (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].mv_point_sets.clear();

	if(DEBUGGING_EDITING)
		cout << "    Removed mesh #" << (mesh + 1) << ", all point sets" << endl;

	assert(invariant());
}

void ObjModel :: removePointSetVertex (unsigned int mesh,
                                       unsigned int point_set,
                                       unsigned int vertex)
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, point_set));

	vector<unsigned int>& rv_vertexes = mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}
	rv_vertexes.pop_back();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<      ", point set #" << (point_set + 1)
		     <<         ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePointSetVertexAll (unsigned int mesh,
                                          unsigned int point_set)
{
	assert(mesh < getMeshCount());
	assert(point_set < getPointSetCount(mesh));

	mv_meshes[mesh].mv_point_sets[point_set].mv_vertexes.clear();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<      ", point set #" << (point_set + 1) << ", all vertexes" << endl;
	}

	assert(invariant());
}

void ObjModel :: removePolyline (unsigned int mesh,
                                 unsigned int polyline)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	unsigned int polyline_count = mv_meshes[mesh].mv_polylines.size();
	for(unsigned int i = polyline + 1; i < polyline_count; i++)
	{
		assert(i >= 1);
		mv_meshes[mesh].mv_polylines[i - 1] = mv_meshes[mesh].mv_polylines[i];
	}
	mv_meshes[mesh].mv_polylines.pop_back();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<       ", polyline #" << (polyline + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePolylineAll (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].mv_polylines.clear();

	if(DEBUGGING_EDITING)
		cout << "    Removed mesh #" << (mesh + 1) << ", all polylines" << endl;

	assert(invariant());
}

void ObjModel :: removePolylineVertex (unsigned int mesh,
                                       unsigned int polyline,
                                       unsigned int vertex)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));
	assert(vertex < getPolylineVertexCount(mesh, polyline));

	vector<PolylineVertex>& rv_vertexes = mv_meshes[mesh].mv_polylines[polyline].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}
	rv_vertexes.pop_back();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<       ", polyline #" << (polyline + 1)
		     <<         ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removePolylineVertexAll (unsigned int mesh,
                                          unsigned int polyline)
{
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	mv_meshes[mesh].mv_polylines[polyline].mv_vertexes.clear();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<       ", polyline #" << (polyline + 1) << ", all vertexes" << endl;
	}

	assert(invariant());
}

void ObjModel :: removeFace (unsigned int mesh,
                             unsigned int face)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	vector<Face>& rv_faces = mv_meshes[mesh].mv_faces;
	unsigned int face_count = rv_faces.size();
	for(unsigned int i = face + 1; i < face_count; i++)
	{
		assert(i >= 1);
		rv_faces[i - 1] = rv_faces[i];
	}
	rv_faces.pop_back();

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removeFaceAll (unsigned int mesh)
{
	assert(mesh < getMeshCount());

	mv_meshes[mesh].mv_faces.clear();
	mv_meshes[mesh].m_all_triangles = true;

	if(DEBUGGING_EDITING)
		cout << "    Removed mesh #" << (mesh + 1) << ", all faces" << endl;

	assert(invariant());
}

void ObjModel :: removeFaceVertex (unsigned int mesh,
                                   unsigned int face,
                                   unsigned int vertex)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));
	assert(vertex < getFaceVertexCount(mesh, face));

	vector<FaceVertex>& rv_vertexes = mv_meshes[mesh].mv_faces[face].mv_vertexes;
	unsigned int vertex_count = rv_vertexes.size();
	for(unsigned int i = vertex + 1; i < vertex_count; i++)
	{
		assert(i >= 1);
		rv_vertexes[i - 1] = rv_vertexes[i];
	}
	rv_vertexes.pop_back();
	m_valid = false;

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1)
		     <<         ", vertex #" << (vertex + 1) << endl;
	}

	assert(invariant());
}

void ObjModel :: removeFaceVertexAll (unsigned int mesh,
                                      unsigned int face)
{
	assert(mesh < getMeshCount());
	assert(face < getFaceCount(mesh));

	mv_meshes[mesh].mv_faces[face].mv_vertexes.clear();
	m_valid = false;

	if(DEBUGGING_EDITING)
	{
		cout << "    Removed mesh #" << (mesh + 1)
		     <<           ", face #" << (face + 1) << ", all vertexes" << endl;
	}

	assert(invariant());
}



void ObjModel :: validate ()
{
	bool old_valid = m_valid;

	m_valid = true;

	if(DEBUGGING_VALIDATE)
	{
		cout << "Vertices: " << getVertexCount() << endl;
		cout << "Texture Coordinates: " << getTextureCoordinateCount() << endl;
		cout << "Vertex Normals: " << getNormalCount() << endl;

		if(mv_material_libraries.size() > 0)
		{
			cout << "Material Libraries:" << endl;
			for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
				cout << "    " << mv_material_libraries[i].m_file_name << endl;
		}
	}

	for(unsigned int m = 0; m < mv_meshes.size(); m++)
	{
/*
		// bad materials cause too many crashes
		if(mv_meshes[m].m_material_name != "" && mv_meshes[m].mp_material == NULL)
		{
			if(DEBUGGING_VALIDATE)
				cout << "Invalid material: Mesh " << s << " has non-existant material \"" << mv_meshes[m].m_material_name << "\"" << endl;
			m_valid = false;

			assert(old_valid == false);
			assert(invariant());
			return;
		}
*/
		for(unsigned int p = 0; p < mv_meshes[m].mv_point_sets.size(); p++)
		{
			unsigned int point_set_vertex_count = mv_meshes[m].mv_point_sets[p].mv_vertexes.size();
			if(point_set_vertex_count < 1)
			{
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}

			for(unsigned int v = 0; v < point_set_vertex_count; v++)
			{
				unsigned int vertex = mv_meshes[m].mv_point_sets[p].mv_vertexes[v];

				if(vertex >= getVertexCount())
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid vertex: Point Set " << p << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}
			}
		}

		for(unsigned int l = 0; l < mv_meshes[m].mv_polylines.size(); l++)
		{
			unsigned int polyline_vertex_count = mv_meshes[m].mv_polylines[l].mv_vertexes.size();
			if(polyline_vertex_count < 2)
			{
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}

			for(unsigned int v = 0; v < polyline_vertex_count; v++)
			{
				unsigned int vertex = mv_meshes[m].mv_polylines[l].mv_vertexes[v].m_vertex;
				unsigned int texture_coordinates = mv_meshes[m].mv_polylines[l].mv_vertexes[v].m_texture_coordinate;

				if(vertex >= getVertexCount())
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid vertex: Polyline " << l << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}

				if(texture_coordinates >= getTextureCoordinateCount() && texture_coordinates != NO_TEXTURE_COORDINATES)
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid texture coordinates: Polyline " << l << ", vertex " << v << " has texture coordinates " << (texture_coordinates + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}
			}
		}

		mv_meshes[m].m_all_triangles = true;
		for(unsigned int f = 0; f < mv_meshes[m].mv_faces.size(); f++)
		{
			unsigned int face_vertex_count = mv_meshes[m].mv_faces[f].mv_vertexes.size();
			if(face_vertex_count < 3)
			{
				m_valid = false;

				assert(old_valid == false);
				assert(invariant());
				return;
			}
			else if(face_vertex_count > 3)
				mv_meshes[m].m_all_triangles = false;

			for(unsigned int v = 0; v < face_vertex_count; v++)
			{
				unsigned int vertex              = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_vertex;
				unsigned int texture_coordinates = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_texture_coordinate;
				unsigned int normal              = mv_meshes[m].mv_faces[f].mv_vertexes[v].m_normal;

				if(vertex >= getVertexCount())
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid vertex: Mesh " << m << ", face " << f << ", vertex " << v << " has vertex " << (vertex + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}

				if(texture_coordinates >= getTextureCoordinateCount() && texture_coordinates != NO_TEXTURE_COORDINATES)
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid texture coordinates: Mesh " << m << ", face " << f << ", vertex " << v << " has texture coordinates " << (texture_coordinates + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}

				if(normal >= getNormalCount() && normal != NO_NORMAL)
				{
					if(DEBUGGING_VALIDATE)
						cout << "Invalid normal: Mesh " << m << ", face " << f << ", vertex " << v << " has normal " << (normal + 1) << endl;
					m_valid = false;

					assert(old_valid == false);
					assert(invariant());
					return;
				}
			}
		}
	}

	assert(invariant());
}



#ifndef OBJ_LIBRARY_SHADER_DISPLAY

void ObjModel :: drawMeshMaterial (unsigned int mesh, const Material* p_material) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(!Material::isMaterialActive());

	if(p_material == NULL)
		drawMesh(mesh);
	else
	{
		p_material->activate();
		drawMesh(mesh);
		Material::deactivate();

		if(p_material->isSeperateSpecular())
		{
			p_material->activateSeperateSpecular();
			drawMesh(mesh);
			Material::deactivate();
		}
	}

	assert(!Material::isMaterialActive());
}

const Material* ObjModel :: getMaterialByName (const string& name) const
{
	assert(isValid());

	for(unsigned int i = 0; i < mv_material_libraries.size(); i++)
	{
		if(mv_material_libraries[i].mp_mtl_library == NULL)
			continue;

		unsigned int index = mv_material_libraries[i].mp_mtl_library->getMaterialIndex(name);
		if(index != MtlLibrary::NO_SUCH_MATERIAL)
			return mv_material_libraries[i].mp_mtl_library->getMaterial(index);
	}

	return NULL;
}

void ObjModel :: drawMesh (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());

	drawPointSets(mesh);
	drawPolylines(mesh);
	drawFaces(mesh);
}

void ObjModel :: drawPointSets (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());

	if(getPointSetCount(mesh) > 0)
	{
		glBegin(GL_POINTS);
			for(unsigned int p = 0; p < getPointSetCount(mesh); p++)
				for(unsigned int v = 0; v < getPointSetVertexCount(mesh, p); v++)
				{
					unsigned int vertex = mv_meshes[mesh].mv_point_sets[p].mv_vertexes[v];

					glVertex3dv(mv_vertexes[vertex].getAsArray());
				}
		glEnd();
	}
}

void ObjModel :: drawPolylines (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());

	for(unsigned int l = 0; l < getPolylineCount(mesh); l++)
	{
		glBegin(GL_LINE_STRIP);
			for(unsigned int v = 0; v < getPolylineVertexCount(mesh, l); v++)
			{
				unsigned int vertex              = mv_meshes[mesh].mv_polylines[l].mv_vertexes[v].m_vertex;
				unsigned int texture_coordinates = mv_meshes[mesh].mv_polylines[l].mv_vertexes[v].m_texture_coordinate;

				if(texture_coordinates != NO_TEXTURE_COORDINATES)
				{
					// flip texture coordinates to match Maya <|>
					glTexCoord2d(      mv_texture_coordinates[texture_coordinates].x,
					             1.0 - mv_texture_coordinates[texture_coordinates].y);
				}

				glVertex3dv(mv_vertexes[vertex].getAsArray());
			}
		glEnd();
	}
}

void ObjModel :: drawFaces (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());

	bool is_all_triangles = mv_meshes[mesh].m_all_triangles;

	// if everything is triangles, draw everything as one triangle group
	if(is_all_triangles)
		glBegin(GL_TRIANGLES);

	for(unsigned int f = 0; f < getFaceCount(mesh); f++)
	{
		// otherwise, draw as lots of trinagle fans
		if(!is_all_triangles)
			glBegin(GL_TRIANGLE_FAN);

		for(unsigned int v = 0; v < getFaceVertexCount(mesh, f); v++)
		{
			unsigned int vertex              = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_vertex;
			unsigned int texture_coordinates = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_texture_coordinate;
			unsigned int normal              = mv_meshes[mesh].mv_faces[f].mv_vertexes[v].m_normal;

			if(normal != NO_NORMAL)
				glNormal3dv(mv_normals[normal].getAsArray());

			if(texture_coordinates != NO_TEXTURE_COORDINATES)
			{
				// flip texture coordinates to match Maya <|>
				glTexCoord2d(      mv_texture_coordinates[texture_coordinates].x,
				             1.0 - mv_texture_coordinates[texture_coordinates].y);
			}

			glVertex3dv(mv_vertexes[vertex].getAsArray());
		}

		// end of current trinagle fan
		if(!is_all_triangles)
			glEnd();
	}

	// end of trinagles
	if(is_all_triangles)
		glEnd();
}

#endif  // OBJ_LIBRARY_SHADER_DISPLAY is not defined



#ifdef OBJ_LIBRARY_SHADER_DISPLAY

MeshWithShader ObjModel :: getPointSetMeshWithShader (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getPointSetCount(mesh) > 0);

	ObjVbo<unsigned int> vbo_indexes = createIndexVboSimple(getMeshPointCountTotal(mesh));
	ObjVbo<float>        vbo_data    = createPointSetVboVertexOnly(mesh);

	MeshWithShader point_set_mesh;
	point_set_mesh.init(GL_POINTS, VertexDataFormat::POSITION_ONLY, vbo_data, vbo_indexes);
	assert(point_set_mesh.isInitialized());
	return point_set_mesh;
}

MeshWithShader ObjModel :: getPolylineMeshWithShader (unsigned int mesh,
                                                      unsigned int polyline,
                                                      bool is_texture_coordinates) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	ObjVbo<unsigned int> vbo_indexes = createIndexVboSimple(getPolylineVertexCount(mesh, polyline));

	unsigned int  data_format;
	ObjVbo<float> vbo_data;

	if(is_texture_coordinates)
	{
		data_format = VertexDataFormat::POSITION_TEXTURE_COORDINATE;
		vbo_data    = createPolylineVboVertexTextureCoordinate(mesh, polyline);
	}
	else
	{
		data_format = VertexDataFormat::POSITION_ONLY;
		vbo_data    = createPolylineVboVertexOnly(mesh, polyline);
	}

	MeshWithShader polyline_mesh;
	polyline_mesh.init(GL_LINE_STRIP, data_format, vbo_data, vbo_indexes);
	assert(polyline_mesh.isInitialized());
	return polyline_mesh;
}

MeshWithShader ObjModel :: getFaceMeshWithShader (unsigned int mesh,
                                                  bool is_texture_coordinates,
                                                  bool is_normals) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);

	using namespace VertexDataFormat;

	VertexArrangement vv_arrangement;
	calculateVertexArrangement(mesh, is_texture_coordinates, is_normals, vv_arrangement);

	ObjVbo<unsigned int> vbo_indexes = createIndexVboForFaces(mesh,
	                                                          is_texture_coordinates,
	                                                          is_normals,
	                                                          vv_arrangement);

	unsigned int  data_format;
	ObjVbo<float> vbo_data;

	if(is_texture_coordinates)
	{
		if(is_normals)
		{
			data_format = VertexDataFormat::POSITION_TEXTURE_COORDINATE_NORMAL;
			vbo_data    = createFaceVboVertexTextureCoordinateNormal(mesh, vv_arrangement);
		}
		else
		{
			data_format = VertexDataFormat::POSITION_TEXTURE_COORDINATE;
			vbo_data    = createFaceVboVertexTextureCoordinate(mesh, vv_arrangement);
		}
	}
	else
	{
		if(is_normals)
		{
			data_format = VertexDataFormat::POSITION_NORMAL;
			vbo_data    = createFaceVboVertexNormal(mesh, vv_arrangement);
		}
		else
		{
			data_format = VertexDataFormat::POSITION_ONLY;
			vbo_data    = createFaceVboVertexOnly(mesh, vv_arrangement);
		}
	}

	MeshWithShader face_mesh;
	face_mesh.init(GL_TRIANGLES, data_format, vbo_data, vbo_indexes);
	assert(face_mesh.isInitialized());

	if(DEBUGGING_FACE_SHADERS)
	{
		cout << "Created face mesh:" << endl;
		cout << "\tis_texture_coordinates = " << is_texture_coordinates << endl;
		cout << "\tis_normals = " << is_normals << endl;
		cout << "\tdata_format = " << data_format << endl;
		cout << "\tVertex count = " << getVertexCount() << endl;
		cout << "\tFace count = " << getFaceCountTotal() << endl;
		cout << "\tElement count = " << vbo_data.getElementCount() / 3 << endl;
		cout << "\tIndex count = " << vbo_indexes.getElementCount() << endl;
	}

	return face_mesh;
}

ObjVbo<float> ObjModel :: createPointSetVboVertexOnly (unsigned int mesh) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getPointSetCount(mesh) > 0);

	using namespace VertexDataFormat;

	// calculate data

	unsigned int  point_count_total = getMeshPointCountTotal(mesh);
	PositionOnly* d_vertexes        = new PositionOnly[point_count_total];

	unsigned int next_point = 0;
	for(unsigned int p = 0; p < mv_meshes[mesh].mv_point_sets.size(); p++)
	{
		const vector<unsigned int>& v_vertex_ids = mv_meshes[mesh].mv_point_sets[p].mv_vertexes;
		for(unsigned int i = 0; i < v_vertex_ids.size(); i++)
		{
			assert(i < v_vertex_ids.size());
			assert(v_vertex_ids[i] < mv_vertexes.size());
			const Vector3& vertex = mv_vertexes[v_vertex_ids[i]];

			assert(next_point < point_count_total);
			d_vertexes[next_point].m_x = (float)(vertex.x);
			d_vertexes[next_point].m_y = (float)(vertex.y);
			d_vertexes[next_point].m_z = (float)(vertex.z);

			assert(next_point < point_count_total);
			next_point++;
			assert(next_point <= point_count_total);
		}
	}
	assert(next_point == point_count_total);

	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  point_count_total * getComponentCount(POSITION_ONLY),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createPolylineVboVertexOnly (unsigned int mesh,
                                                       unsigned int polyline) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	using namespace VertexDataFormat;

	const vector<PolylineVertex>& v_vertex_ids = mv_meshes[mesh].mv_polylines[polyline].mv_vertexes;
	unsigned int                  vertex_count = v_vertex_ids.size();
	PositionOnly*                 d_vertexes   = new PositionOnly[vertex_count];

	for(unsigned int i = 0; i < v_vertex_ids.size(); i++)
	{
		assert(i < v_vertex_ids.size());
		assert(v_vertex_ids[i].m_vertex < mv_vertexes.size());
		const Vector3& vertex = mv_vertexes[v_vertex_ids[i].m_vertex];

		d_vertexes[i].m_x = (float)(vertex.x);
		d_vertexes[i].m_y = (float)(vertex.y);
		d_vertexes[i].m_z = (float)(vertex.z);
	}

	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_ONLY),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createPolylineVboVertexTextureCoordinate (unsigned int mesh,
                                                                    unsigned int polyline) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(polyline < getPolylineCount(mesh));

	using namespace VertexDataFormat;

	const vector<PolylineVertex>& v_vertex_ids = mv_meshes[mesh].mv_polylines[polyline].mv_vertexes;
	unsigned int                  vertex_count = v_vertex_ids.size();
	PositionTextureCoordinate*    d_vertexes   = new PositionTextureCoordinate[vertex_count];

	for(unsigned int i = 0; i < v_vertex_ids.size(); i++)
	{
		assert(i < v_vertex_ids.size());
		assert(v_vertex_ids[i].m_vertex < mv_vertexes.size());
		const Vector3& vertex             = mv_vertexes[v_vertex_ids[i].m_vertex];

		d_vertexes[i].m_x = (float)(vertex.x);
		d_vertexes[i].m_y = (float)(vertex.y);
		d_vertexes[i].m_z = (float)(vertex.z);

		if(v_vertex_ids[i].m_texture_coordinate < mv_texture_coordinates.size())
		{
			const Vector2& texture_coordinate = mv_texture_coordinates[v_vertex_ids[i].m_texture_coordinate];

			// flip texture coordinates to match Maya <|>
			d_vertexes[i].m_s =        (float)(texture_coordinate.x);
			d_vertexes[i].m_t = 1.0f - (float)(texture_coordinate.y);
		}
		else
		{
			// no texture coordinates specified, use fallback
			d_vertexes[i].m_s = (float)(FALLBACK_TEXTURE_COORDINATE.x);
			d_vertexes[i].m_t = (float)(FALLBACK_TEXTURE_COORDINATE.y);
		}
	}

	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_TEXTURE_COORDINATE),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createFaceVboVertexOnly (unsigned int mesh,
                                                   const VertexArrangement& vv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);
	assert(vv_arrangement.size() == getVertexCount());

	using namespace VertexDataFormat;

	unsigned int vertex_count = getVertexArrangementTotalCount(vv_arrangement);
	PositionOnly* d_vertexes = new PositionOnly[vertex_count];

	// add the vertexes specified in vv_arrangement to the data array
	unsigned int next_vertex = 0;
	for(unsigned int i = 0; i < vv_arrangement.size(); i++)
		if(!vv_arrangement[i].empty())
		{
			assert(vv_arrangement[i].size() == 1); // cannot be more because all the same

			assert(i < mv_vertexes.size());
			const Vector3& vertex = mv_vertexes[i];

			d_vertexes[next_vertex].m_x = (float)(vertex.x);
			d_vertexes[next_vertex].m_y = (float)(vertex.y);
			d_vertexes[next_vertex].m_z = (float)(vertex.z);

			assert(next_vertex < vertex_count);
			next_vertex++;
			assert(next_vertex <= vertex_count);
		}
	assert(next_vertex == vertex_count);

	if(DEBUGGING_FACE_SHADERS)
	{
		// print the data array contnents
		cout << "Vertexes Added to VBO:" << endl;

		for(unsigned int i = 0; i < vertex_count; i++)
		{
			cout << "\t" << i << ":\t(" << d_vertexes[i].m_x
			                  << ",\t"  << d_vertexes[i].m_y 
			                  << ",\t"  << d_vertexes[i].m_z << ")" << endl;
		}
	}

	// create the VBO
	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_ONLY),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createFaceVboVertexTextureCoordinate (unsigned int mesh,
                                                                const VertexArrangement& vv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);
	assert(vv_arrangement.size() == getVertexCount());

	using namespace VertexDataFormat;

	unsigned int vertex_count = getVertexArrangementTotalCount(vv_arrangement);
	PositionTextureCoordinate* d_vertexes = new PositionTextureCoordinate[vertex_count];

	// add the vertexes specified in vv_arrangement to the data array
	unsigned int next_vertex = 0;
	for(unsigned int i = 0; i < vv_arrangement.size(); i++)
		for(unsigned int j = 0; j < vv_arrangement[i].size(); j++)
		{
			const TextureCoordinateAndNormal& element = vv_arrangement[i][j];

			assert(i < mv_vertexes.size());
			const Vector3& vertex = mv_vertexes[i];

			d_vertexes[next_vertex].m_x = (float)(vertex.x);
			d_vertexes[next_vertex].m_y = (float)(vertex.y);
			d_vertexes[next_vertex].m_z = (float)(vertex.z);

			if(element.m_texture_coordinate < mv_texture_coordinates.size())
			{
				const Vector2& texture_coordinate = mv_texture_coordinates[element.m_texture_coordinate];

				// flip texture coordinates to match Maya <|>
				d_vertexes[next_vertex].m_s =        (float)(texture_coordinate.x);
				d_vertexes[next_vertex].m_t = 1.0f - (float)(texture_coordinate.y);
			}
			else
			{
				// no texture coordinates specified, use fallback
				d_vertexes[next_vertex].m_s = (float)(FALLBACK_TEXTURE_COORDINATE.x);
				d_vertexes[next_vertex].m_t = (float)(FALLBACK_TEXTURE_COORDINATE.y);
			}

			assert(next_vertex < vertex_count);
			next_vertex++;
			assert(next_vertex <= vertex_count);
		}
	assert(next_vertex == vertex_count);

	// create the VBO
	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_TEXTURE_COORDINATE),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createFaceVboVertexNormal (unsigned int mesh,
                                                     const VertexArrangement& vv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);
	assert(vv_arrangement.size() == getVertexCount());

	using namespace VertexDataFormat;

	unsigned int vertex_count = getVertexArrangementTotalCount(vv_arrangement);
	PositionNormal* d_vertexes = new PositionNormal[vertex_count];

	// add the vertexes specified in vv_arrangement to the data array
	unsigned int next_vertex = 0;
	for(unsigned int i = 0; i < vv_arrangement.size(); i++)
		for(unsigned int j = 0; j < vv_arrangement[i].size(); j++)
		{
			const TextureCoordinateAndNormal& element = vv_arrangement[i][j];

			assert(i < mv_vertexes.size());
			const Vector3& vertex = mv_vertexes[i];

			d_vertexes[next_vertex].m_x = (float)(vertex.x);
			d_vertexes[next_vertex].m_y = (float)(vertex.y);
			d_vertexes[next_vertex].m_z = (float)(vertex.z);

			if(element.m_normal < mv_normals.size())
			{
				const Vector3& normal = mv_normals[element.m_normal];

				d_vertexes[next_vertex].m_nx = (float)(normal.x);
				d_vertexes[next_vertex].m_ny = (float)(normal.y);
				d_vertexes[next_vertex].m_nz = (float)(normal.z);
			}
			else
			{
				// no normal specified, use fallback
				d_vertexes[next_vertex].m_nx = (float)(FALLBACK_NORMAL.x);
				d_vertexes[next_vertex].m_ny = (float)(FALLBACK_NORMAL.y);
				d_vertexes[next_vertex].m_nz = (float)(FALLBACK_NORMAL.z);
			}

			assert(next_vertex < vertex_count);
			next_vertex++;
			assert(next_vertex <= vertex_count);
		}
	assert(next_vertex == vertex_count);

	// create the VBO
	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_NORMAL),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<float> ObjModel :: createFaceVboVertexTextureCoordinateNormal (unsigned int mesh,
                                                                      const VertexArrangement& vv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);
	assert(vv_arrangement.size() == getVertexCount());

	using namespace VertexDataFormat;

	unsigned int vertex_count = getVertexArrangementTotalCount(vv_arrangement);
	PositionTextureCoordinateNormal* d_vertexes = new PositionTextureCoordinateNormal[vertex_count];

	// add the vertexes specified in vv_arrangement to the data array
	unsigned int next_vertex = 0;
	for(unsigned int i = 0; i < vv_arrangement.size(); i++)
		for(unsigned int j = 0; j < vv_arrangement[i].size(); j++)
		{
			const TextureCoordinateAndNormal& element = vv_arrangement[i][j];

			assert(i < mv_vertexes.size());
			const Vector3& vertex = mv_vertexes[i];

			d_vertexes[next_vertex].m_x = (float)(vertex.x);
			d_vertexes[next_vertex].m_y = (float)(vertex.y);
			d_vertexes[next_vertex].m_z = (float)(vertex.z);

			if(element.m_texture_coordinate < mv_texture_coordinates.size())
			{
				const Vector2& texture_coordinate = mv_texture_coordinates[element.m_texture_coordinate];

				// flip texture coordinates to match Maya <|>
				d_vertexes[next_vertex].m_s =        (float)(texture_coordinate.x);
				d_vertexes[next_vertex].m_t = 1.0f - (float)(texture_coordinate.y);
			}
			else
			{
				// no texture coordinates specified, use fallback
				d_vertexes[next_vertex].m_s = (float)(FALLBACK_TEXTURE_COORDINATE.x);
				d_vertexes[next_vertex].m_t = (float)(FALLBACK_TEXTURE_COORDINATE.y);
			}

			if(element.m_normal < mv_normals.size())
			{
				const Vector3& normal = mv_normals[element.m_normal];

				d_vertexes[next_vertex].m_nx = (float)(normal.x);
				d_vertexes[next_vertex].m_ny = (float)(normal.y);
				d_vertexes[next_vertex].m_nz = (float)(normal.z);
			}
			else
			{
				// no normal specified, use fallback
				d_vertexes[next_vertex].m_nx = (float)(FALLBACK_NORMAL.x);
				d_vertexes[next_vertex].m_ny = (float)(FALLBACK_NORMAL.y);
				d_vertexes[next_vertex].m_nz = (float)(FALLBACK_NORMAL.z);
			}

			assert(next_vertex < vertex_count);
			next_vertex++;
			assert(next_vertex <= vertex_count);
		}
	assert(next_vertex == vertex_count);

	// create the VBO
	ObjVbo<float> vbo(GL_ARRAY_BUFFER,
	                  vertex_count * getComponentCount(POSITION_TEXTURE_COORDINATE_NORMAL),
	                  reinterpret_cast<float*>(d_vertexes),
	                  GL_STATIC_DRAW);

	delete[] d_vertexes;
	return vbo;
}

ObjVbo<unsigned int> ObjModel :: createIndexVboSimple (unsigned int count) const
{
	unsigned int* d_indexes = new unsigned int[count];

	for(unsigned int i = 0; i < count; i++)
		d_indexes[i] = i;

	ObjVbo<unsigned int> vbo(GL_ELEMENT_ARRAY_BUFFER, count, d_indexes, GL_STATIC_DRAW);
	delete[] d_indexes;

	return vbo;
}

ObjVbo<unsigned int> ObjModel :: createIndexVboForFaces (unsigned int mesh,
                                                         bool is_texture_coordinates,
                                                         bool is_normals,
                                                         const VertexArrangement& vv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);
	assert(vv_arrangement.size() == getVertexCount());

	assert(mesh < mv_meshes.size());
	const vector<Face>& v_faces = mv_meshes[mesh].mv_faces;

	// number all the vertex-with-datas, based on where they will be in the VBO
	vector<unsigned int> v_start;
	v_start.resize(vv_arrangement.size());

	unsigned int next_start = 0;
	for(unsigned int v = 0; v < vv_arrangement.size(); v++)
	{
		assert(v < v_start.size());
		v_start[v] = next_start;
		next_start += vv_arrangement[v].size();
	}

	if(DEBUGGING_FACE_SHADERS)
	{
		cout << "Vertex arrangement starting indexes:" << endl;

		for(unsigned int v = 0; v < vv_arrangement.size(); v++)
		{
			assert(v < v_start.size());
			cout << "\t" << v << ":\t" << v_start[v] << endl;
		}
	}

	// calculate the number of triangles needed (non-triangle faces will be triangulated)
	unsigned int vertex_count_total = 0;
	for(unsigned int f = 0; f < v_faces.size(); f++)
	{
		unsigned int face_vertex_count = v_faces[f].mv_vertexes.size();
		assert(face_vertex_count >= 3);

		unsigned int triangle_count = face_vertex_count - 2;
		assert(triangle_count >= 1);
		vertex_count_total += triangle_count * 3;  // 3 vertexes per triangle
	}

	// calculate the indexes
	unsigned int* d_indexes = new unsigned int[vertex_count_total];

	unsigned int next_index = 0;
	for(unsigned int f = 0; f < v_faces.size(); f++)
	{
		const vector<FaceVertex>& v_vertex_ids = v_faces[f].mv_vertexes;
		assert(v_vertex_ids.size() >= 3);

		// double loop to triangulate faces
		for(unsigned int t = 2; t < v_vertex_ids.size(); t++)  // per triangle
			for(unsigned int i = 0; i < 3; i++)  // 3 vertexes in each triangle
			{
				//
				//  Calculate face vertex ids for each triangle in the
				//    face.  For faces that are already triangles, these
				//    will just be 0, 1, 2.
				//
				//  t = 2:  0, 1, 2
				//  t = 3:  0, 2, 3
				//  t = 4:  0, 3, 4
				//  ...
				//

				unsigned int face_vertex_index = (i == 0) ? 0 : (t - 2 + i);

				assert(face_vertex_index < v_vertex_ids.size());
				const FaceVertex& face_vertex = v_vertex_ids[face_vertex_index];

				bool is_found = false;

				// find out where the appropriate vertex-with-data will be
				assert(face_vertex.m_vertex < vv_arrangement.size());
				const std::vector<TextureCoordinateAndNormal>& v_vertex_uses = vv_arrangement[face_vertex.m_vertex];
				for(unsigned int j = 0; j < v_vertex_uses.size(); j++)
				{
					if(is_texture_coordinates &&
					   v_vertex_uses[j].m_texture_coordinate != face_vertex.m_texture_coordinate)
					{
						continue;
					}
					if(is_normals && v_vertex_uses[j].m_normal != face_vertex.m_normal)
						continue;

					// if this is the correct vertex, record it in out buffer and stop looking
					is_found = true;

					assert(next_index < vertex_count_total);
					assert(face_vertex.m_vertex < v_start.size());
					d_indexes[next_index] = v_start[face_vertex.m_vertex] + j;
					next_index++;
					assert(next_index <= vertex_count_total);
					break;
				}
				assert(is_found);
			}
	}
	assert(next_index == vertex_count_total);

	if(DEBUGGING_FACE_SHADERS)
	{
		cout << "Vertex Indexes:" << endl;

		for(unsigned int i = 0; i < vertex_count_total; i += 3)
		{
			cout << "\t" << i << ":\t" << d_indexes[i]
			                  <<  "\t" << d_indexes[i + 1] 
			                  <<  "\t" << d_indexes[i + 2] << endl;
		}
	}

	// send the data to the graphics card
	ObjVbo<unsigned int> vbo(GL_ELEMENT_ARRAY_BUFFER, vertex_count_total, d_indexes, GL_STATIC_DRAW);
	delete[] d_indexes;

	return vbo;
}

void ObjModel :: calculateVertexArrangement (unsigned int mesh,
                                             bool is_texture_coordinates,
                                             bool is_normals,
                                             ObjModel::VertexArrangement& rvv_arrangement) const
{
	assert(isValid());
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) > 0);

	//
	//  Calculate which vertexes are used, and, if applicable,
	//    the texture coordinates and normals they are used
	//    with.
	//
	//  The overall idea is that we create a list of which
	//    vertexes are used and which texture coordinates and
	//    normals they are used with.  We end up with a list of
	//    all the combinations that are used.  Then we put the
	//    combinations in the VBO, sorted by which vertexes have
	//    the lowest array index.   When calculating the
	//    indexes, we search our array to find out where the
	//    combinations we want will be in the buffer.
	//
	//  Note that the vector here is the size of the total
	//    number of vertexes, not the number of uses in the
	//    mesh.
	//

	rvv_arrangement.resize(mv_vertexes.size());

	const vector<Face>& v_faces = mv_meshes[mesh].mv_faces;
	for(unsigned int f = 0; f < v_faces.size(); f++)
	{
		const vector<FaceVertex>& v_vertex_ids = v_faces[f].mv_vertexes;

		for(unsigned int i = 0; i < v_vertex_ids.size(); i++)
		{
			assert(i < v_vertex_ids.size());
			const FaceVertex& face_vertex = v_vertex_ids[i];

			bool is_duplicate = false;

			assert(face_vertex.m_vertex < rvv_arrangement.size());
			std::vector<TextureCoordinateAndNormal>& rv_vertex_uses = rvv_arrangement[face_vertex.m_vertex];

			if(is_texture_coordinates)
			{
				if(is_normals)
				{
					for(unsigned int j = 0; j < rv_vertex_uses.size(); j++)
						if(rv_vertex_uses[j].m_texture_coordinate == face_vertex.m_texture_coordinate &&
						   rv_vertex_uses[j].m_normal             == face_vertex.m_normal)
						{
							is_duplicate = true;
						}
				}
				else
				{
					for(unsigned int j = 0; j < rv_vertex_uses.size(); j++)
						if(rv_vertex_uses[j].m_texture_coordinate == face_vertex.m_texture_coordinate)
							is_duplicate = true;
				}
			}
			else
			{
				if(is_normals)
				{
					for(unsigned int j = 0; j < rv_vertex_uses.size(); j++)
						if(rv_vertex_uses[j].m_normal == face_vertex.m_normal)
							is_duplicate = true;
				}
				else
				{
					if(!rv_vertex_uses.empty())
						is_duplicate = true;
				}
			}

			if(!is_duplicate)
			{
				rv_vertex_uses.push_back(TextureCoordinateAndNormal(face_vertex.m_texture_coordinate,
					                                                face_vertex.m_normal));
			}
		}
	}

	if(DEBUGGING_FACE_SHADERS)
	{
		cout << "Calculated vertex arrangement:" << endl;

		for(unsigned int v = 0; v < rvv_arrangement.size(); v++)
		{
			cout << "\t" << v << ":";
			if(!rvv_arrangement[v].empty())
			{
				cout << "\t" << mv_vertexes[v] << endl;
				for(unsigned int i = 0; i < rvv_arrangement[v].size(); i++)
				{
					cout << "\t\t\t(" << rvv_arrangement[v][i].m_texture_coordinate
					     << ", "  << rvv_arrangement[v][i].m_normal << ")" << endl;
				}
			}
			else
				cout << endl;
		}
	}
}

unsigned int ObjModel :: getVertexArrangementTotalCount (const VertexArrangement& vv_arrangement)
{
	unsigned int count = 0;
	for(unsigned int i = 0; i < vv_arrangement.size(); i++)
		count += vv_arrangement[i].size();
	return count;
}

#endif  // OBJ_LIBRARY_SHADER_DISPLAY is defined



bool ObjModel :: readMaterialLibrary (const string& str, ostream& r_logstream)
{
	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		string library;

		size_t token_length = getTokenLength(str, token_index);

		if(token_length == 0)
			return false;

		library = str.substr(token_index, token_length);

		//
		//  Should we add on the current file path? <|>
		//
		//  Thoughts:
		//    - Harder when saving Obj models
		//      - would have to be adjusted to compensate
		//      - strip leading path portion
		//    - Simply prepending it would break currently-working files with duplicated paths
		//    - Try both positions?
		//      - have to only try later ones if first did not exist
		//      - loaded-correctly field in MtlLibrary class
		//
		//  Is this solved by OBJ_LIBRARY_PATH_PROPAGATION
		//

		assert(library != "");
		assert(library.find_last_of("/\\") == string::npos ||
		       library.find_last_of("/\\") + 1 < library.size());
		addMaterialLibrary(library, r_logstream);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readMaterial (const string& str, ostream& r_logstream)
{
	string material;
	unsigned int mesh_index;

	size_t length;
	size_t start_index;
	size_t end_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	length = str.length();
	for(end_index = start_index; end_index < length && !isspace(str[end_index]); end_index++)
		; // do nothing

	if(end_index < length)
		material = str.substr(start_index, end_index - start_index);
	else
		material = str.substr(start_index);

	mesh_index = addMesh();
	setMeshMaterial(mesh_index, material);
	return true;
}

bool ObjModel :: readVertex (const string& str, ostream& r_logstream)
{
	double x;
	double y;
	double z;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	x = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	y = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	z = atof(str.c_str() + index);

	addVertex(x, y, z);
	return true;
}

bool ObjModel :: readTextureCoordinates (const string& str, ostream& r_logstream)
{
	double u;
	double v;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	u = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	v = atof(str.c_str() + index);

	addTextureCoordinate(u, v);
	return true;
}

bool ObjModel :: readNormal (const string& str, ostream& r_logstream)
{
	double x;
	double y;
	double z;

	size_t index;

	if(isspace(str[0]))
		index = nextToken(str, 0);
	else
		index = 0;

	x = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	y = atof(str.c_str() + index);

	index = nextToken(str, index);
	if(index == string::npos)
		return false;

	z = atof(str.c_str() + index);

	if(x == 0.0 && y == 0.0 && z == 0.0)
	{
		r_logstream << "Found a bad normal: #" << getNormalCount() << ", using " << FALLBACK_NORMAL << endl;
		addNormal(FALLBACK_NORMAL_MACRO);
	}
	else
		addNormal(x, y, z);
	return true;
}

bool ObjModel :: readPointSet (const string& str, ostream& r_logstream)
{
	const unsigned int NO_POINT_SET = ~0u;

	unsigned int point_set_index = NO_POINT_SET;
	unsigned int mesh_index;

	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	if(mv_meshes.empty())
		mesh_index = addMesh();
	else
		mesh_index = mv_meshes.size() - 1;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		int vertex;

		vertex = atoi(str.c_str() + token_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(point_set_index != NO_POINT_SET)
				removeLastPointSet(mesh_index);
			return false;
		}

		if(point_set_index == NO_POINT_SET)
			point_set_index = addPointSet(mesh_index);

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		addPointSetVertex(mesh_index, point_set_index, vertex);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readPolyline (const string& str, ostream& r_logstream)
{
	//
	//  This function reads a polyline of vertexes in the
	//    model, not a line of the input file.
	//

	const unsigned int NO_LINE = ~0u;

	unsigned int polyline_index = NO_LINE;
	unsigned int mesh_index;

	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	if(mv_meshes.empty())
		mesh_index = addMesh();
	else
		mesh_index = mv_meshes.size() - 1;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		size_t number_index;

		int vertex;
		int texture_coordinates;

		number_index = token_index;

		vertex = atoi(str.c_str() + number_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(polyline_index != NO_LINE)
				removeLastPolyline(mesh_index);
			return false;
		}

		number_index = nextSlashInToken(str, number_index);
		if(number_index == string::npos)
		{
			texture_coordinates = NO_TEXTURE_COORDINATES;
		}
		else
		{
			number_index++;

			if(isspace(str[number_index]))
				texture_coordinates = NO_TEXTURE_COORDINATES;
			else
			{
				texture_coordinates = atoi(str.c_str() + number_index);
				if(texture_coordinates < 0)
					texture_coordinates += getTextureCoordinateCount() + 1;
				if(texture_coordinates <= 0)
					return false;
			}
		}

		if(polyline_index == NO_LINE)
			polyline_index = addPolyline(mesh_index);

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		if(texture_coordinates != NO_TEXTURE_COORDINATES)
		{
			assert(texture_coordinates >= 1);
			texture_coordinates--;
		}

		addPolylineVertex(mesh_index, polyline_index, vertex, texture_coordinates);
	}

	// if we get here, all tokens were valid
	return true;
}

bool ObjModel :: readFace (const string& str, ostream& r_logstream)
{
	const unsigned int NO_FACE = ~0u;

	unsigned int face_index = NO_FACE;
	unsigned int mesh_index;

	string::size_type start_index;

	if(isspace(str[0]))
		start_index = nextToken(str, 0);
	else
		start_index = 0;

	if(mv_meshes.empty())
		mesh_index = addMesh();
	else
		mesh_index = mv_meshes.size() - 1;

	for(string::size_type token_index = start_index; token_index != string::npos; token_index = nextToken(str, token_index))
	{
		size_t number_index;

		int vertex;
		int texture_coordinates;
		int normal;

		number_index = token_index;

		vertex = atoi(str.c_str() + number_index);
		if(vertex < 0)
			vertex += getVertexCount() + 1;
		if(vertex <= 0)
		{
			if(face_index != NO_FACE)
				removeLastFace(mesh_index);
			return false;
		}

		number_index = nextSlashInToken(str, number_index);
		if(number_index == string::npos)
		{
			texture_coordinates = NO_TEXTURE_COORDINATES;
			normal = NO_NORMAL;
		}
		else
		{
			number_index++;

			if(str[number_index] == '/')
				texture_coordinates = NO_TEXTURE_COORDINATES;
			else
			{
				texture_coordinates = atoi(str.c_str() + number_index);
				if(texture_coordinates < 0)
					texture_coordinates += getTextureCoordinateCount() + 1;
				if(texture_coordinates <= 0)
					return false;
			}

			number_index = nextSlashInToken(str, number_index);
			if(number_index == string::npos)
				normal = NO_NORMAL;
			else
			{
				number_index++;

				if(isspace(str[number_index]))
					normal = NO_NORMAL;
				else
				{
					normal = atoi(str.c_str() + number_index);
					if(normal < 0)
						normal += getNormalCount() + 1;
					if(normal <= 0)
						return false;
				}
			}
		}

		if(face_index == NO_FACE)
			face_index = addFace(mesh_index);

		// reduce indexes to start at 0 instead of 1
		assert(vertex >= 1);
		vertex--;

		if(texture_coordinates != NO_TEXTURE_COORDINATES)
		{
			assert(texture_coordinates >= 1);
			texture_coordinates--;
		}

		if(normal != NO_NORMAL)
		{
			assert(normal >= 1);
			normal--;
		}

		addFaceVertex(mesh_index, face_index, vertex, texture_coordinates, normal);
	}

	// if we get here, all tokens were valid
	return true;
}

void ObjModel :: removeLastPointSet (unsigned int mesh)
{
	assert(mesh < getMeshCount());
	assert(getPointSetCount(mesh) >= 1);

	mv_meshes[mesh].mv_point_sets.pop_back();
	m_valid = false;
}

void ObjModel :: removeLastPolyline (unsigned int mesh)
{
	assert(mesh < getMeshCount());
	assert(getPolylineCount(mesh) >= 1);

	mv_meshes[mesh].mv_polylines.pop_back();
	m_valid = false;
}

void ObjModel :: removeLastFace (unsigned int mesh)
{
	assert(mesh < getMeshCount());
	assert(getFaceCount(mesh) >= 1);

	mv_meshes[mesh].mv_faces.pop_back();
	m_valid = false;
}

bool ObjModel :: invariant () const
{
	if(m_file_name == "") return false;
	if(!ObjStringParsing::isValidPath(m_file_path)) return false;
	return true;
}





#ifdef OBJ_LIBRARY_SHADER_DISPLAY
ObjModel :: TextureCoordinateAndNormal :: TextureCoordinateAndNormal ()
		: m_texture_coordinate(0),
		  m_normal(0)
{
}

ObjModel :: TextureCoordinateAndNormal :: TextureCoordinateAndNormal (
		                        unsigned int texture_coordinate,
		                        unsigned int normal)
		: m_texture_coordinate(texture_coordinate),
		  m_normal(normal)
{
}

ObjModel :: TextureCoordinateAndNormal :: TextureCoordinateAndNormal (const TextureCoordinateAndNormal& original)
		: m_texture_coordinate(original.m_texture_coordinate),
		  m_normal(original.m_normal)
{
}

ObjModel :: TextureCoordinateAndNormal& ObjModel :: TextureCoordinateAndNormal :: operator= (const TextureCoordinateAndNormal& original)
{
	if(&original != this)
	{
		m_texture_coordinate = original.m_texture_coordinate;
		m_normal             = original.m_normal;
	}

	return *this;
}
#endif  // OBJ_LIBRARY_SHADER_DISPLAY is defined





ObjModel :: MaterialLibrary :: MaterialLibrary ()
{
	m_file_name    = "";
	mp_mtl_library = NULL;
}

ObjModel :: MaterialLibrary :: MaterialLibrary (const string& file_path, const string& file_name, ostream& r_logstream)
{
	assert(ObjStringParsing::isValidPath(file_path));

	m_file_name    = file_name;
	mp_mtl_library = &(MtlLibraryManager::get(file_path + file_name, r_logstream));
}

ObjModel :: MaterialLibrary :: MaterialLibrary (const ObjModel :: MaterialLibrary& original)
{
	m_file_name     = original.m_file_name;
	mp_mtl_library = original.mp_mtl_library;
}

ObjModel :: MaterialLibrary& ObjModel :: MaterialLibrary :: operator= (const ObjModel :: MaterialLibrary& original)
{
	if(&original != this)
	{
		m_file_name    = original.m_file_name;
		mp_mtl_library = original.mp_mtl_library;
	}

	return *this;
}





ObjModel :: PolylineVertex :: PolylineVertex ()
{
	m_vertex             = 0;
	m_texture_coordinate = NO_TEXTURE_COORDINATES;
}

ObjModel :: PolylineVertex :: PolylineVertex (unsigned int vertex, unsigned int texture_coordinates)
{
	m_vertex             = vertex;
	m_texture_coordinate = texture_coordinates;
}

ObjModel :: PolylineVertex :: PolylineVertex (const ObjModel :: PolylineVertex& original)
{
	m_vertex             = original.m_vertex;
	m_texture_coordinate = original.m_texture_coordinate;
}

ObjModel :: PolylineVertex& ObjModel :: PolylineVertex :: operator= (const ObjModel :: PolylineVertex& original)
{
	if(&original != this)
	{
		m_vertex             = original.m_vertex;
		m_texture_coordinate = original.m_texture_coordinate;
	}

	return *this;
}





ObjModel :: FaceVertex :: FaceVertex ()
{
	m_vertex             = 0;
	m_texture_coordinate = NO_TEXTURE_COORDINATES;
	m_normal             = NO_NORMAL;
}

ObjModel :: FaceVertex :: FaceVertex (unsigned int vertex, unsigned int texture_coordinates, unsigned int normal)
{
	m_vertex             = vertex;
	m_texture_coordinate = texture_coordinates;
	m_normal             = normal;
}

ObjModel :: FaceVertex :: FaceVertex (const ObjModel :: FaceVertex& original)
{
	m_vertex             = original.m_vertex;
	m_texture_coordinate = original.m_texture_coordinate;
	m_normal             = original.m_normal;
}

ObjModel :: FaceVertex& ObjModel :: FaceVertex :: operator= (const ObjModel :: FaceVertex& original)
{
	if(&original != this)
	{
		m_vertex             = original.m_vertex;
		m_texture_coordinate = original.m_texture_coordinate;
		m_normal             = original.m_normal;
	}

	return *this;
}




ObjModel :: Mesh :: Mesh () : mv_faces()
{
	m_material_name = "";
	mp_material     = NULL;
	m_all_triangles = true;
}

ObjModel :: Mesh :: Mesh (const string& material_name, Material* p_material) : mv_faces()
{
	m_material_name = material_name;
	mp_material     = p_material;
	m_all_triangles = true;
}

ObjModel :: Mesh :: Mesh (const ObjModel :: Mesh& original) : mv_faces(original.mv_faces)
{
	m_material_name = original.m_material_name;
	mp_material     = original.mp_material;
	m_all_triangles = original.m_all_triangles;
}

ObjModel :: Mesh& ObjModel :: Mesh :: operator= (const ObjModel :: Mesh& original)
{
	if(&original != NULL)
	{
		m_material_name = original.m_material_name;
		mp_material     = original.mp_material;
		mv_faces        = original.mv_faces;
		m_all_triangles = original.m_all_triangles;
	}

	return *this;
}
