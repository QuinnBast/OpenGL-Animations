//
//  MtlLibrary.h
//
//  This class represents a library of MTL materials.
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

#ifndef OBJ_LIBRARY_MTL_LIBRARY_H
#define OBJ_LIBRARY_MTL_LIBRARY_H

#include <string>
#include <vector>
#include <iostream>



namespace ObjLibrary
{

class Material;



//
//  MtlLibrary
//
//  This class represents a library of MTL materials.  It
//    corresponds to an .mtl file.
//
//  Class Invariant:
//    <1> m_file_name != ""
//    <2> m_file_name_lowercase == toLowercase(m_file_name)
//    <3> m_file_path_lowercase == toLowercase(m_file_path)
//    <4> ObjStringParsing::isValidPath(m_file_path)
//    <5> mvp_materials[i] != NULL
//                           WHERE 0 <= i < mvp_materials.size()
//
class MtlLibrary
{
public:
//
//  NO_SUCH_MATERIAL
//
//  A constant indicating that there is no Material in this
//    MtlLibrary with the specified name.
//
	static const unsigned int NO_SUCH_MATERIAL;

#ifndef OBJ_LIBRARY_SHADER_DISPLAY
//
//  Class Function: isMaterialActive
//
//  Purpose: A convenience function to determine if there is
//           currently a Material that has been activated but
//           not deactivated.
//  Parameter(s): N/A
//  Precondition(s):  N/A
//  Returns: Whether there is a currently active Material.
//  Side Effect: N/A
//  Note: This function just calls the version in Material of
//        the same name.
//
	static bool isMaterialActive ();

//
//  Class Function: deactivate
//
//  Purpose: A convenience function to deactivate the currently
//           active Material, if any.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The current active material, if any, is
//               deactivated.
//  Note: This function just calls the version in Material of
//        the same name.
//
	static void deactivate ();
#endif  // OBJ_LIBRARY_SHADER_DISPLAY is not defined

public:
//
//  Default Constructor
//
//  Purpose: To create a new MtlLibrary containing no Materials.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new, empty MtlLibrary is created.
//
	MtlLibrary ();

//
//  Constructor
//
//  Purpose: To create a new MtlLibrary from a file.  A logfile
//           may be specified as a filename or an output stream.
//  Parameter(s):
//    <1> filename: The name of the file
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> filename != ""
//    <2> filename.find_last_of("/\\") == string::npos ||
//        filename.find_last_of("/\\") + 1 < filename.size()
//    <3> logfile != ""
//    <4> logfile.find_last_of("/\\") == string::npos ||
//        logfile.find_last_of("/\\") + 1 < logfile.size()
//  Returns: N/A
//  Side Effect: A new MtlLibrary is created containing the
//               materials specified by file filename.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.
//
	MtlLibrary (const std::string& filename);
	MtlLibrary (const std::string& filename,
	            const std::string& logfile);
	MtlLibrary (const std::string& filename,
	            std::ostream& r_logstream);

//
//  Copy Constructor
//
//  Purpose: To create a new MtlLibrary containing the same
//           Materials as another MtlLibrary.
//  Parameter(s):
//    <1> original: The MtlLibrary to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new MtlLibrary is created.  The Materials in
//               original are copied to the new Library.
//
	MtlLibrary (const MtlLibrary& original);

//
//  Assignment Operator
//
//  Purpose: To replace the Materials in this MtlLibrary with
//           the Materials in another MtlLibrary.
//  Parameter(s):
//    <1> original: The MtlLibrary to copy
//  Precondition(s): N/A
//  Returns: A reference to this MtlLibrary.
//  Side Effect: All Materials in this MtlLibrary are removed.
//               The Materials in original are copied to this
//               Library.
//
	MtlLibrary& operator= (const MtlLibrary& original);

//
//  Destructor
//
//  Purpose: To safely destroy a MtlLibrary without memory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.
//
	~MtlLibrary ();

//
//  getFileName
//
//  Purpose: To determine the file name of this MtlLibrary.
//           This does not include the relative file path.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The name of this MtlLibrary in the case it was
//           specified in.
//  Side Effect: N/A
//
	const std::string& getFileName () const;

//
//  getFileNameLowercase
//
//  Purpose: To determine the file name of this MtlLibrary.
//           This does not include the relative file path.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The name of this MtlLibrary, in lowercase.
//  Side Effect: N/A
//
	const std::string& getFileNameLowercase () const;

//
//  getFilePath
//
//  Purpose: To determine the file path of this MtlLibrary.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The file path of this MtlLibrary in the case it was
//           specified in.
//  Side Effect: N/A
//
	const std::string& getFilePath () const;

//
//  getFilePathLowercase
//
//  Purpose: To determine the file path of this MtlLibrary.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The file path of this MtlLibrary, in lowercase.
//  Side Effect: N/A
//
	const std::string& getFilePathLowercase () const;

//
//  getFileNameWithPath
//
//  Purpose: To determine the file name for this MtlLibrary,
//           relative to the current directory.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The relative file name and path for this
//           MtlLibrary, in the case it was specified in.
//  Side Effect: N/A
//
	std::string getFileNameWithPath () const;

//
//  getFileNameWithPathLowercase
//
//  Purpose: To determine the file name for this MtlLibrary,
//           relative to the current directory.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The relative file name and path for this
//           MtlLibrary, in lowercase.
//  Side Effect: N/A
//
	std::string getFileNameWithPathLowercase () const;

//
//  isLoadedSuccessfully
//
//  Purpose: To determine if this MtlLibrary was successfully
//           loaded.  This function does not determine if the
//           file was entirely valid; only if the file existed.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this MtlLibrary as successfully loaded.  If
//           this MtlLibrary was not loaded from a file, true is
//           returned.  If the file did not exist, this false is
//           returned.  Otherwise, true is returned.
//  Side Effect: N/A
//
	bool isLoadedSuccessfully () const;

//
//  isEmpty
//
//  Purpose: To determine if this MtlLibrary is empty.  An
//           MtlLibrary is said to be empty if it contains no
//           materials.  This is the state of an MtlLibrary
//           after it is created with the default constructor.
//           It is also the state of an MtlLibrary after its
//           makeEmpty or removeAll function is called.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this MtlLibrary is empty.
//  Side Effect: N/A
//
	bool isEmpty () const;

//
//  getMaterialCount
//
//  Purpose: To determine the number of Materials in this
//           MtlLibrary.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The number of Materials in this MtlLibrary.
//  Side Effect: N/A
//
	unsigned int getMaterialCount () const;

//
//  isMaterial
//
//  Purpose: To determine whether this MtlLibrary contains a
//           Material with the specified name.  Name
//           comparisons are case-insensitive.
//  Parameter(s):
//    <1> name: The name of the Material
//  Precondition(s):
//    <1> name != ""
//  Returns: Whether there is a Material in this MtlLibrary with
//           name name.
//  Side Effect: N/A
//
	bool isMaterial (const std::string& name) const;

//
//  getMaterialIndex
//
//  Purpose: To determine the index of the Material in this
//           MtlLibrary with the specified name.  Name
//           comparisons are case-insensitive.
//  Parameter(s):
//    <1> name: The name of the Material
//  Precondition(s):
//    <1> name != ""
//  Returns: The index of the Material with name name.  If there
//           is no such Material, NO_SUCH_MATERIAL is returned.
//  Side Effect: N/A
//
	unsigned int getMaterialIndex (const std::string& name)
	                                                      const;

//
//  getMaterialName
//
//  Purpose: To determine the name of the Material with the
//           specified index.
//  Parameter(s):
//    <1> index: The index
//  Precondition(s):
//    <1> index < getMaterialCount()
//  Returns: The name of Material index.
//  Side Effect: N/A
//
	const std::string& getMaterialName (unsigned int index)
	                                                      const;

//
//  isMaterialSeperateSpecular
//
//  Purpose: To determine whether the Material with the
//           specified index contains a seperate specular
//           component.
//  Parameter(s):
//    <1> index: The index
//  Precondition(s):
//    <1> index < getMaterialCount()
//  Returns: Whether Material index contains a seperate specular
//           component.
//  Side Effect: N/A
//
	bool isMaterialSeperateSpecular (unsigned int index) const;

//
//  getMaterial
//
//  Purpose: To retrieve the Material with the specified index.
//  Parameter(s):
//    <1> index: The index
//  Precondition(s):
//    <1> index < getMaterialCount()
//  Returns: The Material in this MtlLibrary with index index.
//  Side Effect: N/A
//
	Material* getMaterial (unsigned int index);
	const Material* getMaterial (unsigned int index) const;

//
//  getMaterial
//
//  Purpose: To retrieve the Material with the specified name.
//  Parameter(s):
//    <1> name: The name of the Material
//  Precondition(s):
//    <1> name != ""
//  Returns: The Material in this MtlLibrary with name name.  If
//           there is no such Material, NULL is returned.
//  Side Effect: N/A
//
	Material* getMaterial (const std::string& name);
	const Material* getMaterial (const std::string& name) const;

#ifndef OBJ_LIBRARY_SHADER_DISPLAY
//
//  activateMaterial
//
//  Purpose: To activate the Material with the specified name.
//           After the drawing has been completed, deactivate()
//           must be called before another material can be
//           activated.
//  Parameter(s):
//    <1> name: The name of the Material
//  Precondition(s):
//    <1> name != ""
//  Returns: N/A
//  Side Effect: The Material in this MtlLibrary with name name
//               is activated.  If there is no such Material,
//               there is no effect.
//
	void activateMaterial (const std::string& name) const;

//
//  activateMaterialSeperateSpecular
//
//  Purpose: To activate seperate specular component of the
//           Material with the specified name.  After the
//           drawing has been completed, deactivate() must be
//           called before another material can be activated.
//  Parameter(s):
//    <1> name: The name of the Material
//  Precondition(s):
//    <1> name != ""
//  Returns: N/A
//  Side Effect: The seperate specular component of the Material
//               in this MtlLibrary with name name is activated.
//               If there is no such Material, there is no
//               effect.  If the Material exists but does not
//               have a seperate specular component, drawing
//               will be disabled until deactivate() is called.
//
	void activateMaterialSeperateSpecular (
	                             const std::string& name) const;
#endif  // OBJ_LIBRARY_SHADER_DISPLAY is not defined

//
//  print
//
//  Purpose: To print the contents of this MtlLibrary.
//  Parameter(s):
//    <1> r_outstream: A reference to the output stream to
//              	 print to
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This MtlLibrary printed to r_outstream.  If
//               r_outstream is not specified, this MtlLibrary
//               is printed to standard output.
//
	void print () const;
	void print (std::ostream& r_outstream) const;

//
//  save
//
//  Purpose: To write the contents of this MtlLibrary to the
//           specified file.  A logfile may be specified as a
//           filename or an output stream.
//  Parameter(s):
//    <1> filename: The name of the file to create
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> filename != ""
//    <2> filename.find_last_of("/\\") == string::npos ||
//        filename.find_last_of("/\\") + 1 < filename.size()
//    <3> logfile != ""
//    <4> logfile.find_last_of("/\\") == string::npos ||
//        logfile.find_last_of("/\\") + 1 < logfile.size()
//  Returns: N/A
//  Side Effect: A file named filename is created.  If a file by
//               that name already exists, its contantes are
//               lost.  This MtlLibrary written to that file in
//               MTL format.  If a logfile or logging stream is
//               specified, any saving errors are written to
//               that file or stream.  Otherwise, any loading
//               errors are written to the standard error
//               stream.
//
	void save (const std::string& filename) const;
	void save (const std::string& filename,
	           const std::string& logfile) const;
	void save (const std::string& filename,
	           std::ostream& r_logstream) const;

//
//  makeEmpty
//
//  Purpose: To restore this MtlLibrary to an empty state.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All Materials in this MtlLibrary are removed.
//               The file name and relative path are set to
//               their default values.  This MtlLibrary is
//               marked as successfully loaded.
//
	void makeEmpty ();

//
//  load
//
//  Purpose: To load the contents of this MtlLibrary from a
//           file.  A logfile may be specified as a filename or
//           an output stream.
//  Parameter(s):
//    <1> filename: The name of the file
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> filename != ""
//    <2> filename.find_last_of("/\\") == string::npos ||
//        filename.find_last_of("/\\") + 1 < filename.size()
//    <3> logfile != ""
//    <4> logfile.find_last_of("/\\") == string::npos ||
//        logfile.find_last_of("/\\") + 1 < logfile.size()
//  Returns: N/A
//  Side Effect: This MtlLibrary is replaced with the
//               MtlLibrary specified in file filename.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  Any
//               existing Materials are lost.
//
	void load (const std::string& filename);
	void load (const std::string& filename,
	           const std::string& logfile);
	void load (const std::string& filename,
	           std::ostream& r_logstream);

//
//  setFileName
//
//  Purpose: To change the name of this MtlLibrary.
//  Parameter(s):
//    <1> name: The new name
//  Precondition(s):
//    <1> name != ""
//  Returns: N/A
//  Side Effect: This MtlLibrary is set to have name name.  This
//               does not change the file path.
//
	void setFileName (const std::string& name);

//
//  setFilePath
//
//  Purpose: To change the relative file path for this
//           MtlLibrary.
//  Parameter(s):
//    <1> file_path: The new file path
//  Precondition(s):
//    <1> ObjStringParsing::isValidPath(file_path)
//  Returns: N/A
//  Side Effect: This MtlLibrary is set to have relative file
//               path path.  This does not change the name of
//               this MtlLibrary.
//
	void setFilePath (const std::string& file_path);

//
//  setFileNameWithPath
//
//  Purpose: To change the name and file path for this
//           MtlLibrary.
//  Parameter(s):
//    <1> filename: The new file name including the relative
//                  file path
//  Precondition(s):
//    <1> filename != ""
//    <2> filename.find_last_of("/\\") == string::npos ||
//        filename.find_last_of("/\\") + 1 < filename.size()
//  Returns: N/A
//  Side Effect: The file name and path for this MtlLibrary are
//               calculated from filename.
//
	void setFileNameWithPath (const std::string& filename);

//
//  add
//
//  Purpose: To add the specified Material to this MtlLibrary.
//  Parameter(s):
//    <1> p_material: A pointer to the Material to add
//  Precondition(s):
//    <1> p_material != NULL
//    <2> !isMaterial(p_material->getName())
//  Returns: The index of the new Material.
//  Side Effect: p_material is added to this MtlLibrary.
//               p_material is assumed to be dynamically
//               allocated and will eventually be destroyed by
//               this MtlLibrary.
//
	unsigned int add (Material* p_material);

//
//  removeAll
//
//  Purpose: To remove all Materials from this MtlLibrary.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All Materials in this MtlLibrary are removed.
//
	void removeAll ();

//
//  loadDisplayTextures
//
//  Purpose: To load all textures that are actually used when
//           displaying any Material in this MtlLibrary.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All textures that would be needed to display
//               any Material in this MtlLibrary are loaded.
//
	void loadDisplayTextures ();

//
//  loadAllTextures
//
//  Purpose: To load all textures referenced by any Material in
//           this MtlLibrary, regardless of whether they are
//           actually used.  To conserve video memory, consider
//           using loadDisplayTextures() instead.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All textures referenced by any Material in this
//               MtlLibrary are loaded.
//
	void loadAllTextures ();

private:
//
//  warnIfLastMaterialIsInvisible
//
//  Purpose: To print a warning message if the
//           most-recently-added Material in this MtlLibrary is
//           entirely transparent.
//  Parameter(s):
//    <1> r_outstream: The stream to write the warning to
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: If the last Material in this MtlLibrary has a
//               transparency of 0.0, a warning message is
//               printed.  If there are no Materials in this
//               MtlLibrary, there is no effect.
//
	void warnIfLastMaterialIsInvisible (
	                           std::ostream& r_outstream) const;

//
//  readMaterialStart
//
//  Purpose: To begin a adding new Material to this MtlLibrary
//           corresponding to the information in a string.
//  Parameter(s):
//    <1> str: The string containing the material name
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a legal material name.
//  Side Effect: If string str specifies a valid, unused
//               material name, a new Material with that name is
//               added to this MtlLibrary.  The new Material is
//               considered to be the one being read in, so
//               future read commands (except readMaterialStart)
//               will affect that Material.  If str does not
//               specify a valid name for a new Material, there
//               is no effect.
//
	bool readMaterialStart (const std::string& str,
	                        std::ostream& r_logstream);

//
//  readIlluminationMode
//
//  Purpose: To read the illumination mode corresponding to the
//           information in a string to the current Material.
//  Parameter(s):
//    <1> str: The string containing the illumination mode
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid illumination
//           mode.
//  Side Effect: If string str specifies a valid illumination
//               mode, the current Material is set to have that
//               illumination mode.  Otherwise, there is no
//               effect.
//
	bool readIlluminationMode (const std::string& str,
	                           std::ostream& r_logstream);

//
//  readColour
//
//  Purpose: To read the colour corresponding to the information
//           in a string to the current Material.
//  Parameter(s):
//    <1> str: The string containing the colour
//    <2> target: Where this colour should be stored
//    <3> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> target < COLOUR_TARGET_TYPES
//  Returns: Whether the string specifies a valid colour.
//  Side Effect: If string str specifies a valid colour, the
//               current Material is set to have that colour
//               for the location specified by target.
//               Otherwise, there is no effect.
//
	bool readColour (const std::string& str,
	                 unsigned int target,
	                 std::ostream& r_logstream);

//
//  readSpecularExponent
//
//  Purpose: To read the specular exponent corresponding to the
//           information in a string to the current Material.
//  Parameter(s):
//    <1> str: The string containing the specular exponent
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid specular
//           exponent.
//  Side Effect: If string str specifies a valid specular
//               exponent, the current Material is set to have
//               that transparency.  Otherwise, there is no
//               effect.
//
	bool readSpecularExponent (const std::string& str,
	                           std::ostream& r_logstream);

//
//  readTransparency
//
//  Purpose: To read the transparency corresponding to the
//           information in a string to the current Material.
//  Parameter(s):
//    <1> str: The string containing the transparency
//    <2> r_logstream: The stream to write loading errors to
//    <3> is_tr_line: Whether the lines began with with "Tr "
//                    instead of "d "
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid transparency.
//  Side Effect: If string str specifies a valid transparency,
//               the current Material is set to have that
//               transparency.  Otherwise, there is no effect.
//               If is_tr_line is true and the
//               OBJ_LIBRARY_TR_0_IS_OPAQUE macro is defined,
//               the transparency will be reversed.
//
	bool readTransparency (const std::string& str,
	                       std::ostream& r_logstream,
	                       bool is_tr_line);

//
//  readOpticalDensity
//
//  Purpose: To read the optical density corresponding to the
//           information in a string to the current Material.
//  Parameter(s):
//    <1> str: The string containing the optical density
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid optical
//           density.
//  Side Effect: If string str specifies a valid optical
//               density, the current Material is set to have
//               that optical density.  Otherwise, there is no
//               effect.
//
	bool readOpticalDensity (const std::string& str,
	                         std::ostream& r_logstream);

//
//  readTransmissionFilter
//
//  Purpose: To read the transmission filter corresponding to
//           the information in a string to the current
//           Material.
//  Parameter(s):
//    <1> str: The string containing the colour
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid transmission
//           filter.
//  Side Effect: If string str specifies a valid transmission
//               filter, the current Material is set to have
//               that transmission filter.  Otherwise, there is
//               no effect.
//
	bool readTransmissionFilter (const std::string& str,
	                             std::ostream& r_logstream);

//
//  readBumpMapMultiplier
//
//  Purpose: To read the bump map multiplier corresponding to
//           the information in a string to the current
//           Material.
//  Parameter(s):
//    <1> str: The string containing the value
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s): N/A
//  Returns: Whether the string specifies a valid bump
//           multiplier.
//  Side Effect: If string str specifies a valid bump
//               multiplier, the current Material is set to have
//               that bump map multiplier.  Otherwise, there is no
//               effect.
//
	bool readBumpMapMultiplier (const std::string& str,
	                            std::ostream& r_logstream);

//
//  readMapColour
//
//  Purpose: To read the texture map filename corresponding to
//           the information in a string to the current
//           Material.
//  Parameter(s):
//    <1> str: The string containing the texture map filename
//    <2> target: Where this texture map should be stored
//    <3> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> target < COLOUR_TARGET_TYPES
//  Returns: Whether the string specifies a valid filename.
//  Side Effect: If string str specifies a valid texture map,
//               the current Material is set to have that
//               texture map for the location specified by
//               target.  Otherwise, there is no effect.
//
	bool readMapColour (const std::string& str,
	                    unsigned int target,
	                    std::ostream& r_logstream);

//
//  readMapChannel
//
//  Purpose: To read the texture map filename and colour channel
//           to use corresponding to the information in a string
//           to the current Material.
//  Parameter(s):
//    <1> str: The string containing the information
//    <2> target: Where this texture map should be stored
//    <3> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> target < CHANNEL_TARGET_TYPES
//  Returns: Whether the string specifies a valid filename and
//           channel.
//  Side Effect: If string str specifies a valid texture map
//               and channel, the current Material is set to
//               have that texture map and colour channel for
//               the location specified by target.  Otherwise,
//               there is no effect.
//
	bool readMapChannel (const std::string& str,
	                     unsigned int target,
	                     std::ostream& r_logstream);

//
//  copy
//
//  Purpose: To copy the values from another MtlLibrary to this
//           MtlLibrary.
//  Parameter(s):
//    <1> original: The MtlLibrary to copy
//  Precondition(s):
//    <1> mvp_materials.size() == 0
//  Returns: N/A
//  Side Effect: All Materials in original are deep-copied to
//               this MtlLibrary.
//
	void copy (const MtlLibrary& original);

//
//  invariant
//
//  Purpose: To determine if the class invariant is true.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whetherthe class invariant is true.
//  Side Effect: N/A
//
	bool invariant () const;

private:
	std::string m_file_name;
	std::string m_file_name_lowercase;
	std::string m_file_path;
	std::string m_file_path_lowercase;
	bool m_is_loaded_successfully;
	std::vector<Material*> mvp_materials;
};



}  // end of namespace ObjLibrary

#endif
