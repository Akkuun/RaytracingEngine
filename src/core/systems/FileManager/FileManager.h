#pragma once
#include <vector>
#include <string>

class FileManager
{
public:
    // Singleton pattern
    static FileManager &getInstance();

    // Delete copy constructor and assignment operator
    FileManager(const FileManager &) = delete;
    FileManager &operator=(const FileManager &) = delete;

    ~FileManager();

    inline const std::string &getActualProjectPath() const { return actualProjectPath; }
    inline void setActualProjectPath(const std::string &path) { actualProjectPath = path; }

    void saveProject(); // Save the current project to actualProjectPath (loop through SceneManager data, camera, lights)
    
private:
    FileManager(); // Private constructor
    std::string actualProjectPath;
    void loadProject(const std::string &projectPath); //  read JSON file and populate SceneManager, camera, lights, etc.

   
    void saveProjectAs(const std::string &newProjectPath); // Save the current project to a new path
    void createNewProjectSaveFile();
};