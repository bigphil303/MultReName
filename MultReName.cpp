#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

//renames files
void renameFiles(const std::vector<std::pair<std::string, std::string>>& fileMappings, const std::string& flag, bool prefix) {
    for (const auto& [oldFile, newName] : fileMappings) {
        fs::path filePath(oldFile);

        //if file doesn't exist, skip
        if (!fs::exists(filePath)) {
            std::cerr << "Error: File not found -> " << oldFile << "\n";
            continue;
        }

        //if file is a directory, skip
        if (fs::is_directory(filePath)) {
            std::cerr << "Skipping directory: " << oldFile << " (Provide a file, not a folder)\n";
            continue;
        }

        //get file extension
        std::string extension = filePath.extension().string();
        //get file directory
        fs::path directory = filePath.parent_path();
        //get file base name
        std::string baseName = filePath.stem().string();

        // If user pressed Enter, keep the same name
        std::string finalBaseName = newName.empty() ? baseName : newName;

        //final name for file
        std::string finalName = prefix ? (flag + "_" + finalBaseName + extension) : (finalBaseName + "_" + flag + extension);
        fs::path newFilePath = directory / finalName;

        //rename file
        try {
            fs::rename(filePath, newFilePath);
            std::cout << "Renamed: " << oldFile << " -> " << newFilePath.string() << "\n";
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error renaming file: " << oldFile << " -> " << e.what() << "\n";
        }
    }
}

//renames files in folder
void renameFolder(const std::string& folderPath, const std::string& flag, bool prefix) {
    //if folder doesn't exist, skip
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "Error: Folder not found -> " << folderPath << "\n";
        return;
    }

    //file mappings (old name -> new name)
    std::vector<std::pair<std::string, std::string>> fileMappings;

    //iterate through files in folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        //if file is a regular file (not a directory)
        if (fs::is_regular_file(entry.path())) {
            //get file name
            std::string oldFileName = entry.path().string();
            std::string baseName = entry.path().stem().string();
            std::string newFileName;

            //Prompts user for new name (If user pressed Enter, keep the same name)
            std::cout << "Enter new name for file " << baseName << "(Enter to skip): ";
            std::getline(std::cin, newFileName);

            //add file mapping (old name -> new name)
            fileMappings.emplace_back(oldFileName, newFileName); 
        }
    }

    //if no files found in folder, skip
    if (fileMappings.empty()) {
        std::cerr << "No files found in folder: " << folderPath << "\n";
        return;
    }

    //renameFiles function to rename files in folder
    renameFiles(fileMappings, flag, prefix);
}

int main() {
    std::string flag;
    int modeChoice, positionChoice;

    //Prompts user for flag to apply to files
    std::cout << "Enter flag to apply to files: ";
    std::getline(std::cin, flag);

    //Prompts user to choose mode
    std::cout << "Choose mode:\n"
            << "1. Rename specific files\n"
            << "2. Rename all files in a folder\n";
    std::cin >> modeChoice;
    std::cin.ignore();

    //if modeChoice is 1, rename specific files
    if (modeChoice == 1) {
        std::vector<std::pair<std::string, std::string>> fileMappings;
        std::cout << "Enter file names ('done' to finish):\n";
        
        //Prompts user for file names to rename
        while (true) {
            std::string oldFileName, newFileName;
            std::cout << "Enter file name: ";
            std::getline(std::cin, oldFileName);
            //if user enters 'done', break
            if (oldFileName == "done") break;

            std::cout << "Enter new name for " << oldFileName << " (Enter to skip): ";
            std::getline(std::cin, newFileName);

            //add file mapping (old name -> new name)
            fileMappings.emplace_back(oldFileName, newFileName);
        }

        //choose flag position
        std::cout << "Choose flag position:\n"
                << "1. Prefix (flag_filename.ext)\n"
                << "2. Suffix (filename_flag.ext)\n";
        std::cin >> positionChoice;
        std::cin.ignore();

        //if positionChoice is 1, prefix is true
        bool prefix = (positionChoice == 1);
        renameFiles(fileMappings, flag, prefix);

    //if modeChoice is 2, rename all files in a folder
    } else if (modeChoice == 2) {
        //Prompts user for folder path
        std::string folderPath;
        std::cout << "Enter folder path: ";
        std::getline(std::cin, folderPath);

        //choose flag position
        std::cout << "Choose flag position:\n1. Prefix (flag_filename.ext)\n2. Suffix (filename_flag.ext)\n";
        std::cin >> positionChoice;
        std::cin.ignore();

        //if positionChoice is 1, prefix is true
        bool prefix = (positionChoice == 1);
        renameFolder(folderPath, flag, prefix);
    } else {
        std::cerr << "Invalid mode selected.\n";
        return 1;
    }

    return 0;
}
