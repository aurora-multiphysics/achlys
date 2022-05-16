#pragma once
#include "BasicTest.h"

class AchlysAppBasicTest : public BasicTest 
{
    public:
    AchlysAppBasicTest() : BasicTest("achlysApp") {};

    virtual void SetUp() override
    {
        createApp();
    }
};

class AchlysAppInputTest : public InputFileTest 
{
    public:
    AchlysAppInputTest(std::string input_file) 
    :InputFileTest("achlysApp", input_file, "unit_test_inputs/")
    {};

    virtual void SetUp() override
    {
        createApp();
    }
};