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

    void saveProject(); // function called after Ctrl + Z and create the save file if inexistant or update it

    
    inline bool getIsNewProjectSelected() const { return isNewProjectSelected; }
    inline void setIsNewProjectSelected(bool value) { isNewProjectSelected = value; }
    
    private:
    FileManager(); // Private constructor
    std::string actualProjectPath;
    std::string currentProjectName;
    void loadProject(const std::string &projectPath); //  read JSON file and populate SceneManager, camera, lights, etc.
    
    void saveProjectAs(const std::string &newProjectPath); // Save the current project to a new path
    void createNewProjectSaveFile();                       // create a new save file
    
    void updateRecentProjectsList();
    bool isNewProjectSelected = false;
    
    void createNewProject(); // open the main window with a default scene (sphere and square)
};