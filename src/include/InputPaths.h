#ifndef INPUTPATHS_H_
#define INPUTPATHS_H_

#include <string>

/// The folder of the input files other than the deck: -p, or the deck's own folder, with its separator at the end.
extern std::string pathArqExtEntrada;

/// How the files a deck names for its solvers are found. A relative name, of a parameter file or of a mesh or table
/// named inside one, is taken from the input folder, as the tables and meshes always were; an absolute name is taken
/// as it is. -l/--legacy-paths restores the old way: the parameter files by the name as written, from the folder the
/// program runs in, and the names inside them always after the input folder.
inline bool legacyInputPaths = false;

/// A name that starts at a root: a separator, or a drive letter and a colon.
inline bool isAbsoluteInputPath(const std::string &name) {
    if (name.empty())
        return false;
    if (name[0] == '/' || name[0] == '\\')
        return true;
    char drive = name[0];
    return name.size() > 1 && name[1] == ':' && ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z'));
}

/// A solver's parameter file, as the deck names it.
inline std::string auxiliaryInputPath(const std::string &name) {
    if (legacyInputPaths || isAbsoluteInputPath(name))
        return name;
    return pathArqExtEntrada + name;
}

/// A mesh or a table, as a solver's parameter file names it.
inline std::string namedInputPath(const std::string &name) {
    if (!legacyInputPaths && isAbsoluteInputPath(name))
        return name;
    return pathArqExtEntrada + name;
}

#endif
