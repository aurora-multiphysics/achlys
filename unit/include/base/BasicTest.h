#pragma once

#include "gtest/gtest.h"

#include <memory>

#include "AppFactory.h"
#include "MooseObject.h"
  
// using ActionList = std::map<std::string, std::vector<std::string>>

class BasicTest : public ::testing::Test 
{

 protected:

  BasicTest(std::string appNameIn) : _args(""), _app_name(appNameIn), _app_is_null(true) {};

  virtual void SetUp() override {};

  virtual void TearDown() override 
  {

    _app = nullptr;

  };

  void createApp() 
  {

    // Convert string arguments to char array
    char * cstr = new char [_args.length() + 1];
    std::strcpy (cstr, _args.c_str());

    // Split string by whitespace delimiter
    std::vector<char*> arg_list;
    char * next;
    next = strtok(cstr," ");
    while (next != NULL){
      arg_list.push_back(next);
      next = strtok(NULL, " ");
    }

    // Create array of char*
    size_t argc = arg_list.size();
    char ** argv = new char*[argc];
    for (size_t i = 0; i < argc; i++){
      argv[i]=arg_list.at(i);
    }

    try {
      _app = AppFactory::createAppShared(_app_name, argc, argv);
      _app_is_null = ( _app == nullptr );
    }
    catch(std::exception& e){
      std::cout<<e.what()<<std::endl;
      _app_is_null = true;
    }

    // Deallocate memory for C arrays created with new
    delete[] argv;
    delete[] cstr;
  };

  void check_object_registration(const std::vector<std::string>& expected_object_names)
  {
    check_object_registration(expected_object_names, _app_name);
  }


  void check_object_registration(const std::vector<std::string>& expected_object_names, std::string app_name)
  {
    ASSERT_FALSE(_app_is_null);

    // Fetch a reference to all objects that been registered (in the global singleton)
    const auto& all_registered_objects = Registry::allObjects();

    // Check the app exists in the registry
    bool app_is_registered = all_registered_objects.find((app_name)) != all_registered_objects.end();
    ASSERT_TRUE(app_is_registered);

    // Get all objects registered to the app
    const auto& registered_app_objects = all_registered_objects.at(app_name);

    // Create a list of names of all objects we expect to see registered
    std::map<std::string, bool> expected_object_register;
    for(const auto name : expected_object_names){
      expected_object_register[name] = false;
    }

    for( const auto & object : registered_app_objects) 
    {
      std::string name = object._classname; // action class name 
      // note task names registered under object._name
      bool expected_this_object = expected_object_register.find(name) != expected_object_register.end();

      if(expected_this_object)
      {
        EXPECT_FALSE(expected_object_register[name]) << "Duplicate enties of object "<< name;

        // Mark that we found this object
        expected_object_register[name] = true;
      }
    }

    // Now should have found all known objects
    for( const auto & [name, registered] : expected_object_register)
    {
      EXPECT_TRUE(registered) << "Object "<< name
                                   << " was not registered to "<<  app_name;
    }

  }

//   using ActionList = std::map<std::string, std::vector<std::string>>
  void check_app_action_registration(const std::vector<std::map<std::string, std::vector<std::string>>>& expected_object_names, std::string app_name)
  {
    ASSERT_FALSE(_app_is_null);

    // Fetch a reference to all objects that been registered (in the global singleton)
    const auto& all_registered_objects = Registry::allActions();

    // Check the app exists in the registry
    bool app_is_registered = all_registered_objects.find((app_name)) != all_registered_objects.end();
    ASSERT_TRUE(app_is_registered);

    // Get all objects registered to the app
    const auto& registered_app_objects = all_registered_objects.at(app_name);

    // Create a list of names of all objects we expect to see registered
    std::map<std::string, std::map<std::string, bool>> expected_object_register;
    for(const auto action_list : expected_object_names)
    {
        for (const auto [action_name, task_names]: action_list)
        {
            for (const auto task_name: task_names)
            {
                expected_object_register[action_name][task_name] = false;
            }
        }
    }

    for( const auto & object : registered_app_objects) 
    {
      std::string action_name = object._classname;
      std::string task_name = object._name;
      bool expected_this_action = expected_object_register.find(action_name) != expected_object_register.end();
      if (!expected_this_action) continue;

      bool expected_this_task = expected_object_register[action_name].find(task_name) 
                                                            != expected_object_register[action_name].end();

      if(expected_this_task)
      {
        // actions will have multiple tasks registered under the same 
        EXPECT_FALSE(expected_object_register[action_name][task_name]) << "Duplicate enties of action/task "
                                                    << action_name << " / " << task_name;

        // Mark that we found this object
        expected_object_register[action_name][task_name] = true;
      }
    }

    // Now should have found all known objects
    for( const auto & [action_name, task_map] : expected_object_register)
    {
        for(const auto & [task_name, registered]: task_map)
        {
      EXPECT_TRUE(registered) << "Action "<< action_name << ", task " << task_name
                                   << " was not registered to "<<  app_name;
        }
    }

  }

  // Arguments for our app
  std::string _args;

  // Pointer to our Moose App;
  std::shared_ptr<MooseApp> _app = nullptr;

  std::string _app_name;

  bool _app_is_null;

};

class InputFileTest : public BasicTest {

 protected:

  InputFileTest(std::string appName, std::string inputfile,std::string inputdir) :
    BasicTest(appName),
    tol(1.e-9) {
    inputdir_=inputdir;
    setInputFile(inputfile);
  };

  void setInputFile(std::string inputfile){
    if(inputfile==""){
      throw std::logic_error("Input file string is empty");
    }
    _args+= " -i " + inputdir_ + inputfile;
  };

  // Some file utility methods
  bool fileExists(std::string filename){
    std::ifstream f(filename.c_str());
    return f.good();
  }

  void fetchInputFile(std::string file, std::string newName=""){

    std::string fileNow = inputdir_ + file;
    ASSERT_TRUE(fileExists(fileNow));

    if(newName=="") newName=file;

    // This is ugly: system() can result in undefined behaviour. Fix me.
    std::string command = "cp "+fileNow +" "+newName;
    int retval = system(command.c_str());
    ASSERT_EQ(retval,0);
    ASSERT_TRUE(fileExists(newName));
  }

  void fetchInput(const std::vector<std::string>& inputFilesSrc,
                  const std::vector<std::string>& inputFilesDest){

    ASSERT_EQ(inputFilesSrc.size(),inputFilesDest.size());

    size_t nFiles = inputFilesSrc.size();
    for(size_t iFile=0; iFile<nFiles; iFile++){
      std::string fileSrc = inputFilesSrc.at(iFile);
      std::string fileDest = inputFilesDest.at(iFile);
      fetchInputFile(fileSrc,fileDest);
    }
  }

  void deleteFile(std::string filename){
    std::string err = "Failed to remove " + filename;
    EXPECT_EQ(remove(filename.c_str()),0) << err;
  }

  void deleteIfFileExists(const std::string& file){
    if(fileExists(file)){
      deleteFile(file);
    }
  }

  void deleteAll(const std::vector<std::string>& fileList){
    for(const auto file : fileList ){
      deleteIfFileExists(file);
    }
  }

  void checkFilesExist(const std::vector<std::string>& fileList){
    for(const auto file : fileList ){
      EXPECT_TRUE(fileExists(file)) << file << " was not found";
    }
  }

  // Define a tolerance for double comparisons
  double tol;
  std::string inputdir_;
};